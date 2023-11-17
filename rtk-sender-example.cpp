#include <cstdio>
#include <memory>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib> // For getenv
#include <yaml-cpp/yaml.h>

#include "PX4-GPSDrivers/src/gps_helper.h"
#include "PX4-GPSDrivers/src/ubx.h"
#include "serial-comms.h"
#include "driver-interface.h"


int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("\n");
        printf("usage: %s <serial device> <baudrate> <mavlink connection>\n", argv[0]);
        printf("e.g.: %s /dev/ttyUSB0 38400 udp://:24550\n", argv[0]);
        printf("Note: use baudrate 0 to determine baudrate automatically\n");
        return 1;
    }

    unsigned baudrate = std::stoi(argv[2]);

    SerialComms serial_comms;
    if (!serial_comms.init(argv[1])) {
        return 2;
    }

    mavsdk::Mavsdk mavsdk;

    // Required when connecting to a flight controller directly via USB.
    mavsdk::Mavsdk::Configuration config{mavsdk::Mavsdk::Configuration::UsageType::GroundStation};
    config.set_always_send_heartbeats(true);
    mavsdk.set_configuration(config);

    auto connection_result = mavsdk.add_any_connection(argv[3]);
    if (connection_result != mavsdk::ConnectionResult::Success) {
        printf("Mavsdk connection failed\n");
        return 3;
    }

    DriverInterface driver_interface(serial_comms, mavsdk);

    auto driver = std::make_unique<GPSDriverUBX>(
            GPSDriverUBX::Interface::UART,
            &DriverInterface::callback_entry, &driver_interface,
            &driver_interface.gps_pos, &driver_interface.sat_info);
  

    // Check to see if a base position has been saved as a yaml file in the user's home directory
    // If yaml is present, proceed to use the fixed postion data
    // If not present, perform an RTK survey

    const char* homeDir = getenv("HOME"); // Get the user's home directory
    if (homeDir) {
        std::filesystem::path yamlFilePath = std::filesystem::path(homeDir) / "fixed_position.yaml";

        if (std::filesystem::exists(yamlFilePath)) {
            std::cout << "Found fixed postion YAML - Using fixed coordinates." << std::endl;
            // Parse the YAML file
            try {
                const YAML::Node config = YAML::LoadFile(yamlFilePath.string());

                // Access and assign values, and set base position
                double latitude = config["latitude"].as<double>(); //[deg]
                double longitude = config["longitude"].as<double>(); //[deg]
                float altitude = config["altitude"].as<float>(); //[m]
                float position_accuracy = config["position_accuracy"].as<float>(); //[mm]
                driver->setBasePosition(latitude, longitude, altitude, position_accuracy);

            
            } catch (const YAML::Exception& e) {
                std::cerr << "Error parsing YAML: " << e.what() << std::endl;
                return 1;
            }
        } else {
            std::cout << "No fixed postion YAML - Beginning RTK survey." << std::endl;
            // Set survey paramaters and begin survey
            constexpr auto survey_minimum_m = 0.4;
            constexpr auto survey_duration_s = 1500;
            driver->setSurveyInSpecs(survey_minimum_m * 10000, survey_duration_s);
        }
    } else {
        std::cerr << "Unable to retrieve the user's home directory." << std::endl;
        return 1;
    }
    

    GPSHelper::GPSConfig gps_config {};
    // to test if RTCM is not available
    //gps_config.output_mode = GPSHelper::OutputMode::GPS;
    gps_config.output_mode = GPSHelper::OutputMode::RTCM;
    gps_config.gnss_systems = GPSHelper::GNSSSystemsMask::RECEIVER_DEFAULTS;

    if (driver->configure(baudrate, gps_config) != 0) {
        printf("configure failed\n");
        return 4;
    }

    printf("configure done!\n");


    while (true) {
        // Keep running, and don't stop on timeout.
        // Every now and then it timeouts but I'm not sure if that's actually
        // warranted given correct messages are still arriving.
        const unsigned timeout_ms = 5000;
        driver->receive(timeout_ms);
    }
    printf("timed out\n");

    return 0;
}
