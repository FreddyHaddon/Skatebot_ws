#!/bin/bash
# Install pre-built packages to control the robot.

sudo apt update
sudo apt install -y --no-install-recommends \
    ros-${ROS_DISTRO}-dynamixel-sdk \
    ros-${ROS_DISTRO}-dynamixel-sdk-custom-interfaces \
    ros-${ROS_DISTRO}-dynamixel-sdk-examples
