#include <cstdio>
#include <memory>

#include "PX4-GPSDrivers/src/gps_helper.h"
#include "PX4-GPSDrivers/src/ubx.h"
#include "serial-comms.h"
#include "driver-interface.h"

int main(int argc, char* argv[])
{
    if (argc != 5) {
        printf("\n");
        printf("argc = %d \n", argc);
        printf("usage: %s <serial device> <baudrate> <mavlink connection> <system_ids>\n", argv[0]);
        printf("e.g.: %s /dev/ttyUSB0 38400 udp://:24550 2,3,4,5\n", argv[0]);
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

// Parse system_ids from command line argument.
std::vector<uint8_t> system_ids;
std::string system_ids_string(argv[4]);
std::stringstream ss(system_ids_string);

for (uint8_t i; ss >> i;) {
    system_ids.push_back(static_cast<uint8_t>(i - '0'));  // Convert character to integer
    if (ss.peek() == ',')
        ss.ignore();
}


    printf("System ID's:");
    for (const auto& id : system_ids) {
        printf(" %d", id);
    }
    printf("\n");

    DriverInterface driver_interface(serial_comms, mavsdk, system_ids);

    auto driver = std::make_unique<GPSDriverUBX>(
            GPSDriverUBX::Interface::UART,
            &DriverInterface::callback_entry, &driver_interface,
            &driver_interface.gps_pos, &driver_interface.sat_info);

    constexpr auto survey_minimum_m = 20;//0.8;
    constexpr auto survey_duration_s = 10;//300;
    driver->setSurveyInSpecs(survey_minimum_m * 10000, survey_duration_s);

    GPSHelper::GPSConfig gps_config {};
    gps_config.output_mode = GPSHelper::OutputMode::RTCM;
    gps_config.gnss_systems = GPSHelper::GNSSSystemsMask::RECEIVER_DEFAULTS;

    if (driver->configure(baudrate, gps_config) != 0) {
        printf("configure failed\n");
        return 4;
    }

    printf("configure done!\n");

    while (true) {
        // Keep running, and don't stop on timeout.
        const unsigned timeout_ms = 5000;
        driver->receive(timeout_ms);
    }
    printf("timed out\n");

    return 0;
}
