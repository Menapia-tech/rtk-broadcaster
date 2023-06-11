cmake_minimum_required(VERSION 3.16)

project(rtk-broadcaster CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MAVSDK REQUIRED)

add_executable(rtk-broadcaster
    rtk-broadcaster.cpp
    serial-comms.h
    serial-comms.cpp
    definitions.h
    driver-interface.h
    driver-interface.cpp
    PX4-GPSDrivers/src/gps_helper.h
    PX4-GPSDrivers/src/gps_helper.cpp
    PX4-GPSDrivers/src/ubx.h
    PX4-GPSDrivers/src/ubx.cpp
    PX4-GPSDrivers/src/rtcm.h
    PX4-GPSDrivers/src/rtcm.cpp
)

target_compile_options(rtk-broadcaster
    PRIVATE
    -Wall
    -Wextra
)

target_link_libraries(rtk-broadcaster
    MAVSDK::mavsdk
)

target_include_directories(rtk-broadcaster
    PRIVATE
    PX4-GPSDrivers/src/
)
