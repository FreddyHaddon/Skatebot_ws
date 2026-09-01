#!/usr/bin/env python3

import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, ExecuteProcess, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    sensors_launch = os.path.join(
        get_package_share_directory("pipebot_4wd"),
        "launch",
        "sensors.launch.py"
    )


    recorder = ExecuteProcess(
        cmd=[
            "bash", "-c",
            "mkdir -p /home/skatebot-ubuntu/Recordings &&"
            "ros2 bag record "
            
# Comment out any topics you don't want to record
            "/camera/image_raw/compressed "
            "/camera/camera_info "
            "/imu "
            "/imu/euler "
            "/imu_euler_relative
            "/magnetic_field "
            "/scan/front "
#           "/scan/top "
            "/tf "
            "/tf_static "
            "/encoder/left "
            "/encoder/right "
#           "/motor/left "
#           "/motor/right "
            "/cmd_vel "
            "/controlled_cmd_vel "
            "/joy "
#           "/joint_states "
            "/robot_description "
            "/diagnostics "
            "/diagnostics_agg "
#           "/diagnostics_toplevel_state "
            "/battery_state "
            "/turret_joystick/is_forward "
            "/servo/turret "
            "/stop_obstacle "
            "/ellipse_model/distances "

            "-o /home/skatebot-ubuntu/Recordings/run_$(date +%Y%m%d_%H%M%S)"
        ],
        output="screen"
    )

    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(sensors_launch)
        ),
        TimerAction(
            period=3.0,
            actions=[recorder]
        )
    ])
