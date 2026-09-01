#!/bin/bash
# Install and setup ROS2.

# Stop on first error.
set -e

# Tell the user what is going on.
echo
echo "Running $0..."
echo

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )"
setup_dir=${script_dir}/..
. ${setup_dir}/vars.bash

# Upgrade everything to latest.
sudo apt update
sudo apt dist-upgrade -y
sudo apt autoremove -y
sudo apt autoclean -y

# Install ROS2 APT list.
if [ ! -e /etc/apt/sources.list.d/ros2.list ]
then
    # From https://foxglove.dev/blog/installing-ros2-humble-on-ubuntu
    sudo apt update && sudo apt install -y curl gnupg lsb-release
    sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(source /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null
fi

# Install ROS2.
sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    ros-${ROS2_DISTRO}-desktop \
    ros-${ROS2_DISTRO}-launch-testing-ament-cmake \
    ros-${ROS2_DISTRO}-rosidl-generator-dds-idl \
    python3-argcomplete \
    python3-colcon-common-extensions \
    python3-colcon-mixin \
    python3-pip \
    python3-pytest \
    python3-rosdep \
    python3-rosdep-modules \
    python3-rospkg \
    python3-rosdistro \
    python3-setuptools \
    python3-vcstool \
    python3-catkin-pkg \
    flake8

# Hack to prevent this warning:
# /usr/lib/python3/dist-packages/setuptools/command/install.py:34: SetuptoolsDeprecationWarning: setup.py install is deprecated. Use build and pip and other standards-based tools.
sudo pip3 install setuptools==58.2.0

# Bootstrap rosdep
sudo rm -f /etc/ros/rosdep/sources.list.d/20-default.list
sudo rosdep init
rosdep update

# Setup colcon mixin and metadata
mkdir -p ${COLCON_SRC_DIR}
cd ${COLCON_WS_DIR}
. /opt/ros/${ROS2_DISTRO}/setup.bash
if [ ! -e ${HOME}/.colcon/mixin/default/asan.mixin ]
then
    colcon mixin add default https://raw.githubusercontent.com/colcon/colcon-mixin-repository/master/index.yaml
fi
colcon mixin update
if [ ! -e ${HOME}/.colcon/metadata/default/Gazebo.meta ]
then
    colcon metadata add default https://raw.githubusercontent.com/colcon/colcon-metadata-repository/master/index.yaml
fi
colcon metadata update

echo
echo "$0 took $SECONDS seconds."
echo

