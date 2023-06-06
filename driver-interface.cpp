#include "driver-interface.h"
#include <mavconn/mavconn_interface.h>
#include <iostream>

int DriverInterface::callback_entry(GPSCallbackType type, void* data1, int data2, void* user)
{
    auto self = reinterpret_cast<DriverInterface*>(user);
    return self->callback(type, data1, data2);
}

int DriverInterface::callback(GPSCallbackType type, void* data1, int data2)
{
    switch (type) {
        case GPSCallbackType::readDeviceData:
            return serial_comms_.read(static_cast<uint8_t*>(data1), data2);

        case GPSCallbackType::writeDeviceData:
            return serial_comms_.write(static_cast<const uint8_t*>(data1), data2);

        case GPSCallbackType::setBaudrate:
            return (serial_comms_.set_baudrate(data2) ? 0 : 1);

        case GPSCallbackType::gotRTCMMessage:
            send_rtcm_data(static_cast<const uint8_t*>(data1), data2);
            return 0;

        default:
            // Ignore rest.
            return 0;
    }
}

void DriverInterface::send_rtcm_data(const uint8_t* data, int data_len)
{
    mavlink_message_t message;
    mavlink_gps_rtcm_data_t rtcm_data;
    rtcm_data.len = data_len;
    memcpy(rtcm_data.data, data, data_len);

    mavlink_msg_gps_rtcm_data_encode(0, 0, &message, &rtcm_data);

    mav_interface_->send_message(message);
}