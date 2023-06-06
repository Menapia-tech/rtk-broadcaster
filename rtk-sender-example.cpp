#include <cstdio>
#include <memory>

#include "PX4-GPSDrivers/src/gps_helper.h"
#include "PX4-GPSDrivers/src/ubx.h"
#include "serial-comms.h"
#include "driver-interface.h"

#include <mavconn/interface.h>
#include <mavlink/v2.0/common/mavlink.h>
#include <mavlink/v2.0/common/gps_rtcm_data.h>


int main(int argc, char* argv[])
{
    if (argc != 4) {
        printf("\n");
        printf("usage: %s <serial device> <baudrate> <mavlink connection>\n", argv[0]);
        printf("e.g.: %s /dev/ttyUSB0 38400 udp://:14550\n", argv[0]);
        printf("Note: use baudrate 0 to determine baudrate automatically\n");
        return 1;
    }

    unsigned baudrate = std::stoi(argv[2]);

    SerialComms serial_comms;
    if (!serial_comms.init(argv[1])) {
        return 2;
    }

    auto mav_interface = mavconn::MAVConnInterface::open_url(argv[3], 1, 1); //system_id and component_id is set to 1, but these values are ignored because we send with system_id and component_id set to 0 (broadcast)

    DriverInterface driver_interface(serial_comms, mav_interface);

    auto driver = std::make_unique<GPSDriverUBX>(
            GPSDriverUBX::Interface::UART,
            &DriverInterface::callback_entry, &driver_interface,
            &driver_interface.gps_pos, &driver_interface.sat_info);

    constexpr auto survey_minimum_m = 0.8;
    constexpr auto survey_duration_s = 300;
    driver->setSurveyInSpecs(survey_minimum_m * 10000, survey_duration_s);


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
