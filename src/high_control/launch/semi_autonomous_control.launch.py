#!/usr/bin/env python3

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    obstacle_detection = Node(
        package="high_control",
        executable="obstacle_detection",
        name="obstacle_detection",
        output="screen",
        parameters=[
            {"scan_topic": "/scan/front"},
            {"stop_topic": "/stop_obstacle"},
            {"front_angle_deg": 90},
            {"stop_distance_m": 0.05},
            {"min_close_points": 20},
            {"lidar_timeout": 1.0},
        ],
    )
    
    ellipse_model = Node(
        package="high_control",
        executable="ellipse_model",
        name="ellipse_model",
        output="screen",
    )
    
    auto_drive = Node(
        package="high_control",
        executable="auto_drive",
        name="auto_drive",
        output="screen",
        parameters=[
            {"joy_topic": "/joy"},
            {"stop_topic": "/stop_obstacle"},
            {"cmd_vel_topic": "/cmd_vel"},
            {"toggle_button_index": 0},
            {"forward_speed": 0.15},
            {"publish_rate": 10.0},
        ],
    )

    return LaunchDescription(
        [
            obstacle_detection,
            ellipse_model,
            auto_drive,
        ]
    )
