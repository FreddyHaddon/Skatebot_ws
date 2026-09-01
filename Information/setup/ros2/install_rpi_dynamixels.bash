#!/bin/bash
# Install and setup Dynamixel pre-requsites.

# Stop on first error.
set -e

# Tell the user what is going on.
echo
echo "Running $0..."
echo

## Pre-requisites.
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    ros-${ROS_DISTRO}-dynamixel-sdk \
    ros-${ROS_DISTRO}-dynamixel-sdk-custom-interfaces \
    ros-${ROS_DISTRO}-dynamixel-sdk-examples

# Add user to dialout group for serial port access.
sudo adduser $USER dialout

echo
echo "$0 took $SECONDS seconds."
echo
