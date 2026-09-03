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

echo -e "${YELLOW} Updating package list.. ${RESET}"
apt update

echo -e "${YELLOW} Installing terminator.. ${RESET}"
apt install -y terminator

echo -e "${YELLOW} Installing system monitor.. ${RESET}"
apt install -y htop

echo -e "${YELLOW} Installing network and system tools.. ${RESET}"
apt install -y curl
apt install -y wget
apt install -y git
apt install -y gedit
apt install -y nano
apt install -y net-tools

if ! command -v ssh &> /dev/null; then
	echo -e "${YELLOW} Installing SSH server.. ${RESET}"
	apt update
	apt install -y openssh-server
	systemctl start ssh
	systemctl enable ssh
else
	echo -e "${YELLOW} SSH server is already installed. ${RESET}"
fi

echo -e "${YELLOW} Checking SSH service status.. ${RESET}"
if systemctl is-active --quiet ssh; then
	echo -e " ${GREEN} SSH is active and running. ${RESET}"
else
	echo -e " ${RED} SSH service is not running. ${RESET}"
fi

echo -e "${YELLOW} Installing chromium-browser.. ${RESET}"
apt install -y chromium-browser

echo -e "${YELLOW} Installing Python, pip and virtual environment tools.. ${RESET}"
apt install -y python3
apt install -y python3-pip
apt install -y python3-venv

echo -e "${YELLOW} Cleaning up unused packages.. ${RESET}"
apt autoremove -y
apt autoclean

echo -e "${GREEN} Setup complete! ${RESET}"
