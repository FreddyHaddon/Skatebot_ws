#!/usr/bin/env python3

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():

    imu_euler = Node(
        package="quat_to_euler",
        executable="quat_to_euler",
        name="imu_euler",
        output="screen",
        remappings=[
            ("imu/data", "imu"),
        ],
    )

    return LaunchDescription([
        imu_euler,
    ])
