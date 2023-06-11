import subprocess
import os
import sys

def install_service(service_name, line_to_execute, run_as_user, restart_delay_secs, timeout_secs, restart_condish, service_type, call_args, working_dir=None):
    """
    This function creates a systemd service file, moves it to the appropriate directory, and starts the service.

    Args:
        service_name (str): The name of the service.
        line_to_execute (str): The command line to execute.
        run_as_user (str): The user to run the service as.
        restart_delay_secs (str): The delay before the service is restarted.
        timeout_secs (str): The timeout before the service is considered failed.
        restart_condish (str): The condition under which the service should be restarted.
        service_type (str): The type of service (e.g., for an executable, this would be an empty string).
        call_args (str): The arguments to pass to the command line.
        working_dir (str, optional): The working directory for the service. Defaults to None.

    Returns:
        None
    """
    service_file_name = f"{service_name}.service"
    complete_exec_line = f"{service_type} {line_to_execute} {call_args}"

    # Check if the service already exists
    service_status = subprocess.run(["systemctl", "is-active", "--quiet", service_name])
    if service_status.returncode == 0:
        print(f"Service {service_name} already exists.")
        return

    # Create the service file
    with open(service_file_name, 'w') as f:
        f.write("[Unit]\n")
        f.write(f"Description=\"Start {service_name} at boot\"\n")
        f.write("StartLimitIntervalSec=0\n")
        f.write("After=network.target\n")

        f.write("[Service]\n")
        f.write(f"User={run_as_user}\n")
        if working_dir:
            f.write(f"WorkingDirectory={working_dir}\n")
        f.write(f"ExecStart={complete_exec_line}\n")
        f.write(f"Restart={restart_condish}\n")
        f.write(f"RestartSec={restart_delay_secs}\n")
        f.write(f"TimeoutStartSec={timeout_secs}\n")

        f.write("[Install]\n")
        f.write("WantedBy=multi-user.target\n")

    # Move the service file to the systemd directory
    subprocess.run(["sudo", "mv", service_file_name, "/etc/systemd/system"])

    # Start and enable the service
    subprocess.run(["sudo", "systemctl", "start", service_name])
    subprocess.run(["sudo", "systemctl", "enable", service_name])
    subprocess.run(["sudo", "systemctl", "daemon-reload"])

# Usage
if len(sys.argv) != 2:
    print("Usage: python3 install_rtk-broadcaster_service.py <system_ids>")
    print("e.g.: python3 install_rtk-broadcaster_service.py 2,3,4,5")
    sys.exit(1)

system_ids = sys.argv[1]
rtk_broadcaster_path = "/home/menapia/dev/rtk-broadcaster/build/rtk-broadcaster"  # replace with the actual path
service_name = "rtk-broadcaster"
run_as_user = "menapia"
restart_condish = "always"
restart_delay_secs = "2"
timeout_secs = "10"
service_type = ""  # For an executable
line_to_execute = rtk_broadcaster_path
serial_device = "/dev/serial/by-id/usb-u-blox_AG_-_www.u-blox.com_u-blox_GNSS_receiver-if00"  # replace with your actual serial device
baudrate = "38400"  # replace with your actual baudrate
mavlink_connection = "tcp://:5760"  # replace with your actual MAVLink connection
#system_ids = "2,3,4,5"  # replace with your actual system IDs
call_args = f"{serial_device} {baudrate} {mavlink_connection} {system_ids}"


install_service(service_name, line_to_execute, run_as_user, restart_delay_secs, timeout_secs, restart_condish, service_type, call_args)
