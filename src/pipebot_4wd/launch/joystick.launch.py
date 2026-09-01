#!/usr/bin/env python3

# MIT License
#
# Copyright (c) 2023-2024 University of Leeds
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import xacro
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration, TextSubstitution, Command
from launch.actions import DeclareLaunchArgument
from launch_ros.descriptions import ParameterValue

from ament_index_python.packages import get_package_share_directory

PACKAGE_NAME = "pipebot_4wd"

def generate_launch_description():
    ld = LaunchDescription()

    joy_config = LaunchConfiguration("joy_config")
    joy_dev = LaunchConfiguration("joy_dev")
    config_filepath = LaunchConfiguration("config_filepath")

    # Joystick configuration.
    # joy_config_arg = DeclareLaunchArgument("joy_config", default_value="urage")
    # joy_config_arg = DeclareLaunchArgument("joy_config", default_value="canyon")
    joy_config_arg = DeclareLaunchArgument("joy_config", default_value="logitech-f710")
    joy_device_arg = DeclareLaunchArgument("joy_dev", default_value="/dev/input/js0")
    joy_filepath_arg = DeclareLaunchArgument(
        "config_filepath",
        default_value=[
            TextSubstitution(
                text=os.path.join(
                    get_package_share_directory(PACKAGE_NAME),
                    "config",
                    "",
                )
            ),
            joy_config,
            TextSubstitution(text=".config.yaml"),
        ],
    )

    analyser_params_filepath = os.path.join(
        get_package_share_directory("pipebot_4wd"),
        "config",
        "diagnostics_analyser.yaml",
    )

    joy_node = Node(
        package="joy",
        executable="joy_node",
        name="joy_node",
        parameters=[
            {
                "dev": joy_dev,
                "deadzone": 0.05,
                "autorepeat_rate": 10.0,
            }
        ],
    )

    teleop_twist_joy_node = Node(
        package="teleop_twist_joy",
        executable="teleop_node",
        name="teleop_twist_joy_node",
        parameters=[config_filepath],
#        remappings=[
#            ('cmd_vel', 'controlled_cmd_vel'),
# Remap removed to enable the low level control, if you are not using low level control, uncomment the remap above
#        ]
    )

    turret_joystick_node = Node(
        package="turret_joystick",
        executable="turret_joystick_node",
        name="turret_joystick_node",
        output="screen",
        parameters=[{
            "button_index": 2,
            "forward_command": 180,
            "backward_command": 0,
            "is_forward": True,
        }],
    )

    # Robot description configuration.
    use_sim_time_arg = DeclareLaunchArgument(
                "use_sim_time",
                default_value="false",
                description="Use sim time if true",
            )
    use_ros2_control_arg = DeclareLaunchArgument(
                "use_ros2_control",
                default_value="true",
                description="Use ros2_control if true",
            )

    # Robot description configuration.
    # Check if we're told to use sim time
    use_sim_time = LaunchConfiguration("use_sim_time")
    # use_ros2_control = LaunchConfiguration("use_ros2_control")

    # Process the xacro file
    pkg_path = os.path.join(get_package_share_directory(PACKAGE_NAME))
    xacro_file = os.path.join(pkg_path, "description", "skatebot.xacro")
    doc = xacro.parse(open(xacro_file))
    xacro.process_doc(doc)
    robot_description_config = doc.toxml()

    # Create a robot_state_publisher node
#   params = {
#       "robot_description": robot_description_config,
#       "use_sim_time": use_sim_time,
#   }

#   node_robot_state_publisher = Node(
#       package="robot_state_publisher",
#       executable="robot_state_publisher",
#       output="screen",
#       parameters=[params],
#   )
# Commented out because it is redundant, rsp.launch.py already does this, and is included in robot.launch.py

    # Chassis controller.
    chassis_controller_node = Node(
        package="chassis_controller",
        executable="chassis_controller_exec",
        # namespace="low_level_controller",
      #  executable="diff_controller_exec",
        # arguments=[
        #     "--ros-args",
        #     "--log-level",
        #     "debug",
        # ],
        remappings=[
            ('dynamixel_driver/all_motors', 'all_motors'),
        ]
    )

    # Dynamixel driver.
    dynamixel_driver_node = Node(
        package="dynamixel_driver",
        # namespace="dynamixel_driver",
        executable="dynamixel_driver_exec",
        # arguments=[
        #     "--ros-args",
        #     "--log-level",
        #     "debug",
        # ],
        parameters=[
            {
                # "device_id": "/dev/ttyUSB0",
                # SK81BOT
                # "device_id": "/dev/serial/by-id/"
                # + "usb-FTDI_USB__-__Serial_Converter_FT792AHR-if00-port0",
                # SK82BOT
                "device_id": "/dev/serial/by-id/"
                + "usb-FTDI_USB__-__Serial_Converter_FT8J0RW2-if00-port0",
                # Default is "MX-28AT".  Uncomment to use other dynamixels.
                # "dynamixel_model": "MX-64AT",
                # Specify the servo gearing ratio. n to 1.  Default = 1:1.
                # Skatebot needs 4200 encoder position units to turn 180 where
                # 2048 is 180 degrees on the servo. 4200 / 2048 = 2.05.
                "gear_ratio": 2.05,
            },
        ],
    )

    rpi_monitor_node = Node(
        package="rpi_monitor",
        executable="rpi_monitor_exec",
    )

    # ros2 run diagnostic_aggregator aggregator_node
    aggregator_node = Node(
        package="diagnostic_aggregator",
        executable="aggregator_node",
        parameters=[analyser_params_filepath],
    )

    ld.add_action(joy_config_arg)
    ld.add_action(joy_device_arg)
    ld.add_action(joy_filepath_arg)
    ld.add_action(joy_node)
    ld.add_action(use_sim_time_arg)
    ld.add_action(use_ros2_control_arg)
#   ld.add_action(node_robot_state_publisher)
    ld.add_action(teleop_twist_joy_node)
    ld.add_action(turret_joystick_node)
    ld.add_action(chassis_controller_node)
    ld.add_action(dynamixel_driver_node)
    ld.add_action(rpi_monitor_node)
    ld.add_action(aggregator_node)

    return ld
