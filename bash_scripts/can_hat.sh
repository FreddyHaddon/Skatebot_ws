#!/bin/bash

RED="\e[31m"
GREEN="\e[32m"
YELLOW="\e[33m"
BLUE="\e[34m"
RESET="\e[0m"

set -e
trap 'echo -e "${RED} Oops! Something went wrong. Exiting.. ${RESET}" ' ERR

if [[ "$EUID" -ne 0 ]]; then
	echo -e "${RED} Please run this script with sudo: ${RESET}"
	echo -e "${RED} sudo ./setup.sh ${RESET}"
	exit 1
fi


echo -e "${YELLOW} Initialising CAN device.. ${RESET}"
ip link set can0 up type can bitrate 50000
ifconfig can0 txqueuelen 65536
ifconfig can0 up

if ifconfig can0 &> /dev/null; then
	echo -e "${YELLOW} Installing SSH server.. ${RESET}"
fi
