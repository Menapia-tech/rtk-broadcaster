#pragma once

#include "definitions.h"
#include "serial-comms.h"
#include "gps_helper.h"

#include <memory>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/rtk/rtk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

class DriverInterface {
public:
DriverInterface(SerialComms& serial_comms, mavsdk::Mavsdk& mavsdk, const std::vector<uint8_t>& system_ids) :
    serial_comms_(serial_comms),
    mavsdk_(mavsdk),
    system_ids_(system_ids) {}

    static int callback_entry(GPSCallbackType type, void* data1, int data2, void* user);
    int callback(GPSCallbackType type, void* data1, int data2);

    void send_rtcm_data(const uint8_t* data, int data_len);
    // void send_rtcm_data(const uint8_t* data, int data_len, const std::shared_ptr<mavsdk::System>& system);

    void print_gps_fix_type(mavsdk::System& system);
    // mavsdk::System* find_system_by_id(uint8_t system_id);   
    std::shared_ptr<mavsdk::System> find_system_by_id(uint8_t system_id);


    // void print_gps_fix_type(const mavsdk::System& system);  // Add this line

    struct sensor_gps_s        gps_pos;
	struct satellite_info_s    sat_info;

private:
    SerialComms& serial_comms_;
    mavsdk::Mavsdk& mavsdk_;
    std::vector<uint8_t> system_ids_;
    std::shared_ptr<mavsdk::Rtk> rtk_plugin_{};
    std::shared_ptr<mavsdk::Telemetry> telemetry_plugin_{};
};
