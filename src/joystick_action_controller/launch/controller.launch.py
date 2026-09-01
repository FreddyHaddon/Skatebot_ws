#!/usr/bin/python3

# Copyright (c) 2023 University of Leeds.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# The author, A. Blight, has asserted his moral rights.

import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, TextSubstitution
from launch_ros.actions import Node


def generate_launch_description():
    ld = LaunchDescription()

    joy_config = LaunchConfiguration("joy_config")
    joy_dev = LaunchConfiguration("joy_dev")
    config_filepath = LaunchConfiguration("config_filepath")

    # Default to the Logitech joystick.
    joy_config_arg = DeclareLaunchArgument("joy_config", default_value="logitech-f710")
    joy_device_arg = DeclareLaunchArgument("joy_dev", default_value="/dev/input/js0")
    joy_filepath_arg = DeclareLaunchArgument(
        "config_filepath",
        default_value=[
            TextSubstitution(
                text=os.path.join(
                    get_package_share_directory("joystick_action_controller"),
                    "config",
                    "",
                )
            ),
            joy_config,
            TextSubstitution(text=".config.yaml"),
        ],
    )

    joy_node = Node(
        package="joy",
        executable="joy_node",
        name="joy_node",
        parameters=[
            {
                "dev": joy_dev,
                "deadzone": 0.05,
                "autorepeat_rate": 5.0,
            }
        ],
    )

    joystick_action_controller_node = Node(
        package="joystick_action_controller",
        executable="joystick_action_controller_exec",
        parameters=[config_filepath],
    )

#   ultrasonic_server_node = Node(
#       package="tank_ultra_pkg",
#       executable="ultrasonic_server",
#       parameters=[
#           # Sensor range parameter in Centimetre, default is 300cm
#           {"sensor_range": 300},
#           # Sensor i2c address, Front Sensor is h0x70, Back Upward Sensor is h0x71
#           {"sensor_address": 0x70},
#           # Sensor reading frequency, maximum is 5Hz, limited by Raspberry Pi hardware platform
#           {"reading_freq": 5},
#           # Sensor Angular Scanning Start:
#           {"sensor_angle_stt": -45},
#           # Sensor Angular Scanning End:
#           {"sensor_angle_end": 45},
#           # Sensor Angular Scanning Step:
#           {"sensor_angle_step": 15}
#       ]
#   )


    ld.add_action(joy_config_arg)
    ld.add_action(joy_device_arg)
    ld.add_action(joy_filepath_arg)
    ld.add_action(joy_node)
    ld.add_action(joystick_action_controller_node)
#   ld.add_action(ultrasonic_server_node)

    return ld
