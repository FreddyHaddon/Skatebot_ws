#!/usr/bin/env python3

import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    imu_launch = os.path.join(
        get_package_share_directory("bno08x_driver"),
        "launch",
        "bno085_i2c.launch.py"
    )

    lidar_launch = os.path.join(
        get_package_share_directory("ldlidar_stl_ros2"),
        "launch",
        "both_ld06.launch.py"
    )

    camera_launch = os.path.join(
        get_package_share_directory("camera_ros"),
        "launch",
        "camera.launch.py"
    )

    preview_node = Node(
        package="pipebot_4wd",
        executable="camera_preview_node",
        name="camera_preview_node",
        output="screen",
        parameters=[
            {"input_topic": "/camera/image_raw"},
            {"output_topic": "/camera/preview/compressed"},
            {"width": 320},
            {"height": 240},
            {"fps": 15.0},
            {"jpeg_quality": 50},
        ]
    )

    quat_to_euler = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory("quat_to_euler"),
                "launch",
                "quat_to_euler.launch.py",
            )
        )
    )
    
    euler_relative = Node(
        package="quat_to_euler",
        executable="euler_relative",
        name="relative_euler",
        output="screen",
    )

    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(imu_launch)
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(lidar_launch)
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(camera_launch)
        ),
        preview_node,
        quat_to_euler,
        euler_relative,
    ])
