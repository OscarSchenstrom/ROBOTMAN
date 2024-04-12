#!/usr/bin/env python3

import os
import platform
import subprocess
import sys

def run_command(command):
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error:{e}")
        sys.exit(1)

def main():
    sketch_name = "robotman/robotman.ino"
    fqbn = "arduino:renesas_uno:unor4wifi"

    if platform.system() == "Windows":
        port = "COM5"   #change this to fit your setup
    else:
        port = "/dev/ttyUSB0" #change this to fit your setup

    compile_command = f"arduino-cli compile -b {fqbn} {sketch_name}"
    upload_command = f"arduino-cli upload -p {port} --fqbn {fqbn} {sketch_name}"

    run_command(compile_command)
    run_command(upload_command)

if __name__ == "__main__":
    main()