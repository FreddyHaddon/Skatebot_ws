#!/bin/bash
# Install and setup Tank Bot Raspberry Pi Camera packages.

# Stop on first error.
set -e

# Tell the user what is going on.
echo
echo "Running $0..."
echo


mkdir -p ~/git
# libcamera
if [ ! -e /usr/local/bin/cam ]
then
    # Get the code.
    cd ~/git
    git clone https://git.libcamera.org/libcamera/libcamera.git
    cd libcamera
    # Dependencies.  Should be pretty much the minimal set required.
    sudo apt update
    sudo apt install -y --no-install-recommends \
        g++ meson ninja-build pkg-config \
        libyaml-dev python3-yaml python3-ply python3-jinja2 \
        libssl-dev openssl \
        libdw-dev libunwind-dev \
        libudev-dev libboost-dev \
        libglib2.0-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
        libevent-dev \
        libcamera-dev libjpeg-dev libtiff5-dev \
        libavcodec-dev libavdevice-dev libavformat-dev libswresample-dev \
        libgnutls28-dev openssl libtiff5-dev \
        qtbase5-dev libqt5core5a libqt5gui5 libqt5widgets5 \
        python3-pip ros-humble-camera-info-manager
    sudo pip3 install pyyaml ply
    sudo pip3 install --upgrade meson
    # Build and install.
    meson build --buildtype=release -Dpipelines=raspberrypi -Dipas=raspberrypi -Dv4l2=true -Dgstreamer=enabled -Dtest=false -Dlc-compliance=disabled -Dcam=enabled -Dqcam=enabled -Ddocumentation=disabled
    ninja -C build
    sudo ninja -C build install
fi

# libcamera-apps
if [ ! -e /usr/local/bin/libcamera-hello ]
then
    # Get the code.
    cd ~/git
    git clone https://github.com/raspberrypi/libcamera-apps.git
    cd libcamera-apps/
    # Install dependencies.
    mkdir -p build
    cd build
    cmake .. -DENABLE_DRM=1 -DENABLE_X11=1 -DENABLE_QT=1 -DENABLE_OPENCV=0 -DENABLE_TFLITE=0
    make -j4
    sudo make install
    sudo ldconfig
fi

# Add user to video group
sudo adduser $USER video

echo
echo "Test using 'qcam' or 'libcamera-hello --qt-preview'."
echo
echo "$0 took $SECONDS seconds."
echo
