#!/usr/bin/env python3

# MIT License
#
# Copyright (c) 2023 University of Leeds
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

from ament_index_python.packages import get_package_share_directory


from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.parameter_descriptions import ParameterValue
from launch.substitutions import Command, FindExecutable
from launch.actions import RegisterEventHandler
from launch.event_handlers import OnProcessStart

from launch_ros.actions import Node




def generate_launch_description():
    # Include the robot_state_publisher launch file, provided by our own package.
    # Force sim time to be enabled.
    # !!! MAKE SURE YOU SET THE PACKAGE NAME CORRECTLY !!!

    package_name = "pipebot_4wd"  # <--- CHANGE ME

    xacro_file = os.path.join(
        get_package_share_directory(package_name),
        "description",
        "robot.urdf.xacro"
    )
   
    rsp = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [
                os.path.join(
                    get_package_share_directory(package_name), "launch", "rsp.launch.py"
                )
            ]
        ),
        launch_arguments={"use_sim_time": "false", "use_ros2_control": "true"}.items(),
    )

    joystick = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            [
                os.path.join(
                    get_package_share_directory(package_name),
                    "launch",
                    "joystick.launch.py",
                )
            ]
        )
    )
    
    low_control = IncludeLaunchDescription(
         PythonLaunchDescriptionSource(
             [
                 os.path.join(
                     get_package_share_directory("low_control_with_imu"),
                     "launch",
                     "low_control_with_imu.launch.py",
                 )
             ]
         )
     )
    delayed_low_control = TimerAction(
        period=2.0,
        actions=[low_control]
)
#    twist_mux_params = os.path.join(
#        get_package_share_directory(package_name), "config", "twist_mux.yaml"
#    )
#    twist_mux = Node(
#        package="twist_mux",
#        executable="twist_mux",
#        parameters=[twist_mux_params],
#        remappings=[("/cmd_vel_out", "/diff_cont/cmd_vel_unstamped")],
#    )

#    robot_description = Command(
#        ["ros2 param get --hide-type /robot_state_publisher robot_description"]
#    )

    robot_description = ParameterValue(
        Command([FindExecutable(name='xacro'), ' ', xacro_file]),
        value_type=str
    )

    controller_params_file = os.path.join(
        get_package_share_directory(package_name), "config", "4w_controller_velocity"
    )

#    controller_manager = Node(
#        package="controller_manager",
#        executable="ros2_control_node",
#        parameters=[{"robot_description": robot_description}, controller_params_file],
#    )

    controller_manager = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[{"robot_description": robot_description}, controller_params_file],
    )
    delayed_controller_manager = TimerAction(period=3.0, actions=[controller_manager])
    
#   robot_state_publisher_node = Node(
#       package="robot_state_publisher",
#       executable="robot_state_publisher",
#       parameters=[{"robot_description": robot_description}],
#   )

    diff_drive_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["diff_cont"],
    )

    delayed_diff_drive_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=controller_manager,
            on_start=[diff_drive_spawner],
        )
    )

    joint_broad_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_broad"],
    )

    delayed_joint_broad_spawner = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=controller_manager,
            on_start=[joint_broad_spawner],
        )
    )

    # Launch them all!
    return LaunchDescription(
        [
            rsp,
            joystick,
            delayed_low_control,
#           twist_mux,
            delayed_controller_manager,
            delayed_diff_drive_spawner,
            delayed_joint_broad_spawner,
        ]
    )
