# Building ROS2 from source code

The Raspberry Pi5 needs to use Debian Bookworm, which is not yet supported by ROS2. The following steps are used to build ROS2 Iron from source code.

Based on this post: https://forums.raspberrypi.com/viewtopic.php?t=361746

Install Raspberry Pi OS using the Raspberry Pi Imager tool.  There version used is Raspberry Pi OS (64 bit) `Bookworm` with full desktop and was dated 2023-12-05.

```bash
sudo apt install -y git colcon python3-rosdep2 vcstool wget python3-flake8-docstrings python3-pip python3-pytest-cov python3-flake8-blind-except python3-flake8-builtins python3-flake8-class-newline python3-flake8-comprehensions python3-flake8-deprecated python3-flake8-import-order python3-flake8-quotes python3-pytest-repeat python3-pytest-rerunfailures python3-vcstools libx11-dev libxrandr-dev libasio-dev libtinyxml2-dev
mkdir -p ~/ros2_iron/src
cd ~/ros2_iron
vcs import --input https://raw.githubusercontent.com/ros2/ros2/iron/ros2.repos src
sudo rm /etc/ros/rosdep/sources.list.d/20-default.list
sudo apt upgrade
sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src --rosdistro iron -y --skip-keys "fastcdr rti-connext-dds-6.0.1 urdfdom_headers python3-vcstool"
colcon build
```

## Additional packages

There are a number of additional packages that need to be installed and setup steps.


### I2C

Make sure the RPi user is part of the `i2c` group.  If not add them.

```bash
sudo apt-get install i2c-tools
```

Check the file `/boot/firmware/config.txt` and add the following lines if not found:

```text
dtparam=i2c_vc=on
dtparam=i2s=on
dtoverlay=hifiberry-dac
dtoverlay=i2s-mmap
```

Save and reboot.

Verify that the ultrasonic sensors can be seen at addresses 0x70 and 0x71 using the command `i2cdetect -y 1`.

Verify that the IMU can be seen at addresses 0x70 and 0x71 using the command `i2cdetect -y 0`.

### Adafruit-blinka circuitpython and bno08x packages

Based on this: https://learn.adafruit.com/circuitpython-on-raspberrypi-linux/installing-circuitpython-on-raspberry-pi

```bash
sudo apt install python3.11-venv
python -m venv env --system-site-packages
source env/bin/activate
cd ~
pip3 install --upgrade adafruit-python-shell
wget https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/master/raspi-blinka.py
sudo -E env PATH=$PATH python3 raspi-blinka.py
```

Reboot and then install the following:

```bash
cd ~
source env/bin/activate
pip3 install adafruit-circuitpython-bno08x
pip3 install adafruit-extended-bus
```

## Building the workspace

Clone extra repos.
dynamixelSDK
vision_opencv
ros/diagnostics


sudo apt install libboost-python-dev


build_deps.bash
build.bash

xacro not found.

Cloned branch `ros2` and built.  One step further forward.
Package `joy` not found.  Cloned branch `ros2` and built.
Package `teleop_twist_joy` not found.  Cloned branch `iron` and built.
Package `diagnostic_updater` not found.  From `diagnostics` repo so just built the package and sourced.

Finally runs!

### Graphics settings

By default, the Wayland graphics are used on the RPi 5.  To get RViz to work, we need to change to back to X11.  Execute `sudo raspi-config` and navigate as follows `6 Advanced Options` -> `A6 Wayland` -> `W1 X11 ...`, select `OK` and exit `raspi-config` when you will be asked to reboot.  Does so and Rviz should work

