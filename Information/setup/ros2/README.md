# Raspberry Pi ROS2 Installation

## Pre-requisites

* Ubuntu 22.04LTS setup and working on your Raspberry Pi.
* `git` is installed.

## Installation on a Raspberry Pi

Clone this repo on the Raspberry Pi using these command:

```bash
mkdir -p ~/git
cd ~/git
git clone https://github.com/pipebots/pipebot_4wd.git
```

Then run:

```bash
cd ~/git/pipebot_4wd/pipebot_4wd/setup/ros2
./install_rpi.bash
```

This install took 17 minutes on my RPi 4, so watch out for the `sudo` password request near the end.  When done, perform the following basic test to verify that ROS2 is working.

```bash
. /opt/ros/humble/setup.bash
ros2 --help
```

Add the following to your `.bash_aliases` file:

```bash
. /opt/ros/humble/setup.bash
. ~/pipebot_4wd_ws/install/setup.bash
```

### NOTES

The changes to the `.bash_aliases` will only take effect when using a new terminal shell so open one and verify that the changes work.

User permissions were changed by some of the scripts. To make use of these, either log out and in again or reboot.

## Hardware drivers

### PiGPIO

The PiGPIO daemon is used to control the servos.  The code is installed by the script `install_rpi_servo.bash` but needs to be configured so that the daemon `pigpiod` is always running as per [this post](https://forums.raspberrypi.com/viewtopic.php?t=103752).  The commands are:

```bash
sudo crontab -e
```

Add the line below to the end of the file.

```text
@reboot              /usr/local/bin/pigpiod
```

Save and exit.  Reboot and check that `pigpiod` is running using `ps aux | grep pigpoid`.

### Raspberry Pi Camera

I spent a lot of time messing around trying to get the camera working on the RPi using Ubuntu 22.04LTS.  The RPi OS 64 bit code uses `libcamera` but this OS is dated August 22, so is later than the LTS release so is not supported.  So, I decided to build the RPi `libcamera` support from source.

The instructions in the [README](https://github.com/raspberrypi/libcamera) seemed pretty straightforward, so I coded them in the file `setup/ros2/install_rpi_camera.bash`.  Run this file to build and install `libcamera`.  This script took about 8 minutes to complete.  Test using the `cam` utility and you should see something like this.

```bash
cam -l
[0:10:30.065920356] [3655]  INFO Camera camera_manager.cpp:299 libcamera v0.0.4+19-58e0b6e1
[0:10:30.355043359] [3656]  WARN RPI raspberrypi.cpp:1357 Mismatch between Unicam and CamHelper for embedded data usage!
[0:10:30.359957078] [3656]  INFO RPI raspberrypi.cpp:1476 Registered camera /base/soc/i2c0mux/i2c@1/imx219@10 to Unicam device /dev/media0 and ISP device /dev/media1
Available cameras:
1: 'imx219' (/base/soc/i2c0mux/i2c@1/imx219@10)
```

Then test the camera using the camera launch script

```bash
ros2 launch pipebot_4wd camera.launch.py
```

Check that you have topics being published and that `RQt` can show the video.

#### Notes on getting `libcamera` working

```bash
LIBCAMERA_LOG_LEVELS=*:DEBUG cam -l
...
[0:02:16.345482507] [2001] DEBUG RPI dma_heaps.cpp:43 Failed to open /dev/dma_heap/linux,cma: Permission denied
...
Available cameras:
```

This was easily fixed by adding the user to the video group and then we get:

```bash
cam -l
[0:10:30.065920356] [3655]  INFO Camera camera_manager.cpp:299 libcamera v0.0.4+19-58e0b6e1
[0:10:30.355043359] [3656]  WARN RPI raspberrypi.cpp:1357 Mismatch between Unicam and CamHelper for embedded data usage!
[0:10:30.359957078] [3656]  INFO RPI raspberrypi.cpp:1476 Registered camera /base/soc/i2c0mux/i2c@1/imx219@10 to Unicam device /dev/media0 and ISP device /dev/media1
Available cameras:
1: 'imx219' (/base/soc/i2c0mux/i2c@1/imx219@10)
```

Success.  Now to install a ROS node for the camera.

#### The ROS 2 camera package

The package we are using, `camera_ros`, has a dependency on the package `ros-humble-camera-info-manager`.  This should have been installed in the previous section, so all we have to do is clone and build using `colcon`.  If you have used the [`setup_ws.bash`](workspace/setup_ws.bash) script, the following steps will have been done for you.

```bash
. /opt/ros/humble/setup.bash
cd ~/pipebot_4wd_ws/src
git clone https://github.com/pipebots/camera_ros.git
cd ~/pipebot_4wd_ws
colcon build
```

### I2S Audio

The robots use a pair of I2S microphones and a single I2C speaker for audio testing.  Details to set up the RPi for these are in the (README)[https://github.com/pipebots/rpi4_adafruit_mic_speaker].
These instructions have been implemented in (this script)[install_rpi_audio.bash].
