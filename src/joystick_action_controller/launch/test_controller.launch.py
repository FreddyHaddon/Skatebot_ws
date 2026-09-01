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
    # joy_config_arg = DeclareLaunchArgument("joy_config", default_value="urage")
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

    teleop_twist_joy_node = Node(
        package="teleop_twist_joy",
        executable="teleop_node",
        name="teleop_twist_joy_node",
        parameters=[config_filepath],
        remappings=[
            ("/cmd_vel", "/motor/cmd_vel"),
        ],
    )

    joystick_action_controller_node = Node(
        package="joystick_action_controller",
        executable="joystick_action_controller_exec",
        parameters=[config_filepath],
    )

    diff_drive_node = Node(
        package="dynamixel_diff_drive_motor",
        executable="dynamixel_diff_drive_motor_exec",
        parameters=[
            {
                "device_id": "/dev/ttyUSB0",
                # Default is "MX-28AT".  Uncomment to use other dynamixels.
                # "dynamixel_model": "MX-64AT",
                # Use the Dynamixel Wizard to set the values for forward and reverse.
                # The values can be read in the wizard. using Extended Position mode
                # with torque off so the servo can be moved by hand.
                # Set the forward direction, then the reverse direction.
                # Units are degrees.
                "turret_forward_ticks": 298, # 3400,
                "turret_backward_ticks": -74, #  -850,
                # Encoder publish rate in Hz.
                "encoder_rate_hz": 10,
            },
        ],
        # This sets the logging of __all__ packages and files to DEBUG.
        # arguments=["--ros-args", "--log-level", "debug"],
        # This sets a node to DEBUG.  Note used of node name.
        # arguments=["--ros-args", "--log-level", "servo_publisher_node:=debug"],
        # This sets just one non-node file to DEBUG.
        arguments=["--ros-args", "--log-level", "DynamixelServo:=debug"],
    )

    fake_acoustic_server_node = Node(
        package="sensing_subsystem",
        executable="fake_acoustic_sensing_server",
    )

    fake_ultrasonic_server_node = Node(
        package="tank_ultra_pkg",
        executable="fake_ultrasonic_server",
    )

    ld.add_action(joy_config_arg)
    ld.add_action(joy_device_arg)
    ld.add_action(joy_filepath_arg)
    ld.add_action(joy_node)
    ld.add_action(teleop_twist_joy_node)
    ld.add_action(joystick_action_controller_node)
    ld.add_action(diff_drive_node)
    ld.add_action(fake_acoustic_server_node)
    ld.add_action(fake_ultrasonic_server_node)

    return ld
