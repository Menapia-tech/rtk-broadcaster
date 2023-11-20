# rtk-sender-example
Example that connects to a GPS and sends RTCM RTK data via MAVLink using MAVSDK.

## Prerequisites

1. Install [MAVSDK](https://github.com/mavlink/MAVSDK) on your system.
2. Install `cmake` and a compiler such as `GCC`.
3. Get the git submodule: `git submodule update --init --recursive`.

## Build

```
cmake -Bbuild -H.
cmake --build build
```

## Run

Connect GPS over serial, find the serial device, as well as baudrate.
Also find the MAVSDK connection URL to connect to the vehicle using MAVLink.

```
usage: build/rtk-sender-example <serial device> <baudrate> <mavlink connection>

e.g.: build/rtk-sender-example /dev/ttyUSB0 38400 udp://:14550

```
Note: use baudrate 0 to determine baudrate automatically


## Fixed Mode

The RTK sender can survey-in to get a base position, or use a pre-defined fixed base position.

By default, a survey-in (0.4m, 1500s) will be performed, unless there is a file named 'fixed_position.yaml' in the home directory.


## RTCM Sniffer

Use the rtcm_sniffer.py script to stream and view the RTCM messages in real time. 

RTCM1005 should be streaming if RTK is working correctly.

The position of the RTK base can be checked using the folowing data frames, and converted to LLA.
"DF025": Point ECEF-X
"DF026": Point ECEF-Y
"DF027": Point ECEF-Z
