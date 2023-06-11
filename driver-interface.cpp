#include "driver-interface.h"
#include <iostream>

int DriverInterface::callback_entry(GPSCallbackType type, void* data1, int data2, void* user)
{
    auto self = reinterpret_cast<DriverInterface*>(user);
    return self->callback(type, data1, data2);
}

int DriverInterface::callback(GPSCallbackType type, void* data1, int data2)
{
    // int typeValue = static_cast<int>(type);

    switch (type) {
        case GPSCallbackType::readDeviceData:
            // std::cout << "readDeviceData case executed\n";
            // std::cout.flush();
            return serial_comms_.read(static_cast<uint8_t*>(data1), data2);

        case GPSCallbackType::writeDeviceData:
            return serial_comms_.write(static_cast<const uint8_t*>(data1), data2);

        case GPSCallbackType::setBaudrate:
            return (serial_comms_.set_baudrate(data2) ? 0 : 1);

        case GPSCallbackType::gotRTCMMessage:
            // std::cout << "gotRTCMMessage case executed\n";
            // std::cout.flush();
            send_rtcm_data(static_cast<const uint8_t*>(data1), data2);
            return 0;

        case GPSCallbackType::gotRelativePositionMessage:
            std::cout << "gotRelativePositionMessage case executed\n";
            std::cout.flush();
            // Handle the relative position message
            // ...
            return 0;

        case GPSCallbackType::surveyInStatus: {
            std::cout << "surveyInStatus case executed\n";
            std::cout.flush();
            // Cast the data to the SurveyInStatus struct
            auto survey_status = static_cast<SurveyInStatus*>(data1);

            // Print the contents of the surveyInStatus message
            std::cout << "Survey In Status:\n";
            std::cout << "Latitude: " << survey_status->latitude << " deg\n";
            std::cout << "Longitude: " << survey_status->longitude << " deg\n";
            std::cout << "Altitude: " << survey_status->altitude << " m\n";
            std::cout << "Mean Accuracy: " << survey_status->mean_accuracy << " mm\n";
            std::cout << "Duration: " << survey_status->duration << " s\n";
            std::cout << "Flags: " << static_cast<int>(survey_status->flags) << '\n';

            return 0;
        }

        case GPSCallbackType::setClock:
            // Handle the setClock case
            // ...
            return 0;

        default:
            std::cout << "Unknown case executed: " << static_cast<int>(type) << '\n';
            std::cout.flush();
            return 0;
    }
}

// void DriverInterface::print_gps_fix_type(mavsdk::System& system)
// {
//     // Creates telemetry plugin object with the referenced system
//     mavsdk::Telemetry telemetry(system);

//     auto gps_info = telemetry.gps_info();
//     std::cout << "GPS fix type: " << gps_info.fix_type << '\n';
// }

void DriverInterface::send_rtcm_data(const uint8_t* data, int data_len)
{
    std::cout << "send_rtcm_data called\n";
    // std::cout.flush();

    if (mavsdk_.systems().empty()) {
        std::cout << "No systems available\n";
        std::cout.flush();
        return;
    }

    mavsdk::Rtk::RtcmData rtcm_data;
    rtcm_data.data.insert(rtcm_data.data.end(), data, data + data_len);

    for (const auto& system : mavsdk_.systems()) {
        auto system_id = system->get_system_id();
        if (std::find(system_ids_.begin(), system_ids_.end(), system_id) == system_ids_.end()) {
            std::cout << "System ID " << static_cast<int>(system_id) << " not found\n";
            std::cout.flush();
            continue;
        }

        std::cout << "System ID: " << static_cast<int>(system_id) << "\n";

        mavsdk::Rtk rtk_plugin(*system);
        rtk_plugin.send_rtcm_data(rtcm_data);

        // mavsdk::Telemetry telemetry(*system);
        // auto gps_info = telemetry.gps_info();
        // std::cout << "GPS fix type for system with ID " << static_cast<int>(system_id) << ": " << gps_info.fix_type << '\n';
    }
}





// std::shared_ptr<mavsdk::System> DriverInterface::find_system_by_id(uint8_t system_id)
// {
//     auto systems = mavsdk_.systems();

//     for (auto& system : systems) {
//         std::cout << "Checking system ID: " << system->get_system_id() << std::endl;

//         if (system->get_system_id() == system_id) {
//             std::cout << "Found system ID: " << system->get_system_id() << std::endl;
//             return system;  // Return shared_ptr directly
//         }
//     }

//     return nullptr;  // Return nullptr if no matching system is found
// }

// mavsdk::System* DriverInterface::find_system_by_id(uint8_t system_id)
// {
//     auto systems = mavsdk_.systems();

//     for (auto& system : systems) {
//         std::cout << "Checking system ID: " << system->get_system_id() << std::endl;

//         if (system->get_system_id() == system_id) {
//             std::cout << "Found system ID: " << system->get_system_id() << std::endl;
//             return system.get();  // Extract raw pointer from shared_ptr
//         }
//     }

//     return nullptr;  // Return nullptr if no matching system is found
// }









// void DriverInterface::send_rtcm_data(const uint8_t* data, int data_len)
// {
//     if (!rtk_plugin_) {
//         if (mavsdk_.systems().empty()) {
//             printf("No system available yet\n");
//             return;
//         }

//         rtk_plugin_ = std::make_shared<mavsdk::Rtk>(mavsdk_.systems()[0]);
//         telemetry_plugin_ = std::make_shared<mavsdk::Telemetry>(mavsdk_.systems()[0]);
//     }

//     mavsdk::Rtk::RtcmData rtcm_data;
//     rtcm_data.data.insert(rtcm_data.data.end(), data, data + data_len);
//     rtk_plugin_->send_rtcm_data(rtcm_data);

//     std::cout << "Fix type: " << telemetry_plugin_->gps_info().fix_type << '\n';
// }
