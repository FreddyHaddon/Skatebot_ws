#!/bin/bash

RED="\e[31m"
GREEN="\e[32m"
YELLOW="\e[33m"
BLUE="\e[34m"
RESET="\e[0m"

set -e
trap 'echo -e "\033[31m Oops! Something went wrong. Exiting.. \033[0m" ' ERR

if [[ "$EUID" -ne 0 ]]; then
	echo -e "${RED} Please run this script with sudo: ${RESET}"
	echo -e "${RED} sudo ./setup.sh ${RESET}"
	exit 1
fi

echo -e "${YELLOW} Cloning the ldlidar_ros2 repository.. ${RESET}"

cd ~
mkdir -p test/src
cd test/src

if [ -d "ldlidar_stl_ros2" ]; then
	echo -e "${YELLOW} Removing existing ldlidar_stl_ros2 folder.. ${RESET}"
	rm -rf ldlidar_stl_ros2
fi

git clone https://github.com/ldrobotSensorTeam/ldlidar_stl_ros2.git

echo -e "${YELLOW} Setting up system environment.. ${RESET}"

if [ -e /dev/ttyUSB0 ]; then
	echo -e "${YELLOW} Setting permissions for /dev/ttyUSB0.. ${RESET}"
	usermod -aG dialout $SUDO_USER
	chmod 777 /dev/ttyUSB0
else
	echo -e "${RED} /dev/ttyUSB0 not found, please check your device connection. ${RESET}"
	exit 1
fi

echo -e "${YELLOW} Replacing the existing ld06.launch.py file with the new one.. ${RESET}"
cp /home/lidar/scripts/ld06.launch.py ~/test/src/ldlidar_stl_ros2/launch/ld06.launch.py

echo -e "${YELLOW} Building the package.. ${RESET}"
cd ~/test
source /opt/ros/humble/setup.bash
colcon build

echo -e "${YELLOW} Setting up environment variables.. ${RESET}"
source install/setup.bash

if ! grep -q "source ~/test/install/setup.bash >> ~/.bashrc" ~/.bashrc; then
	echo "source ~/test/install/setup.bash >> ~/.bashrc" >> ~/.bashrc
fi

source ~/.bashrc

echo -e "${YELLOW} Starting the LD06 LiDAR node.. ${RESET}"
ros2 launch ldlidar stl ros2 ld06.launch.py

echo -e "${GREEN} LiDAR node started successfully! ${RESET}"
