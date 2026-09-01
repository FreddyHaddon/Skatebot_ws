#!/bin/bash
# Install and setup IMU pre-requsites.

# Stop on first error.
set -e

# Tell the user what is going on.
echo
echo "Running $0..."
echo

## Pre-requisites.
pip install -U pip setuptools wheel
pip install -U Adafruit-Blinka
pip install -U adafruit-circuitpython-lsm6ds \
    adafruit_circuitpython_dps310 \
    adafruit_circuitpython_lis3mdl \
    adafruit_circuitpython_lps2x \
    adafruit-circuitpython-mlx90640
pip install -U squaternion

# Add user to I2C group.
sudo adduser $USER i2c

echo
echo "$0 took $SECONDS seconds."
echo
