#pragma once

#include "definitions.h"
#include "serial-comms.h"
#include "gps_helper.h"

#include <memory>
#include <mavconn/interface.h>

class DriverInterface {
public:
    DriverInterface(SerialComms& serial_comms, mavconn::MAVConnInterface::Ptr mav_interface) :
        serial_comms_(serial_comms),
        mav_interface_(mav_interface) {}

    static int callback_entry(GPSCallbackType type, void* data1, int data2, void* user);

    int callback(GPSCallbackType type, void* data1, int data2);

    void send_rtcm_data(const uint8_t* data, int data_len);

    struct sensor_gps_s        gps_pos;
    struct satellite_info_s    sat_info;

private:
    SerialComms& serial_comms_;
    mavconn::MAVConnInterface::Ptr mav_interface_;
};