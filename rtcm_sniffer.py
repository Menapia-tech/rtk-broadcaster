from serial import Serial
from pyrtcm import RTCMReader

stream = Serial('/dev/ttyACM1', 38400, timeout=3)
rtr = RTCMReader(stream)

while 1:
    (raw_data, parsed_data) = rtr.read()
    print(parsed_data)


# <RTCM(1005, DF002=1005, DF003=0, DF021=0, DF022=1, DF023=1, DF024=1, DF141=0, DF025=3773945.9319, DF142=1, DF001_1=0, DF026=-103509.2982, DF364=0, DF027=5123634.870800001)>

# "DF025": (INT38, 0.0001, "Antenna Ref. Point ECEF-X"),
# "DF026": (INT38, 0.0001, "Antenna Ref. Point ECEF-Y"),
# "DF027": (INT38, 0.0001, "Antenna Ref. Point ECEF-Z"),