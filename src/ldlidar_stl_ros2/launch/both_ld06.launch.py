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



    # ros2 run diagnostic_aggregator aggregator_node

from launch import LaunchDescription
from launch_ros.actions import Node
def generate_launch_description():
    front_ldlidar_node = Node(
        package="ldlidar_stl_ros2",
        executable="ldlidar_stl_ros2_node",
        name="LD06_front",
        output="screen",
        parameters=[
            {"product_name": "LDLiDAR_LD06"},
            {"topic_name": "scan/front"},
            {"frame_id": "base_laser"},
            {"port_name": "/dev/ttyAMA4"},
            {"port_baudrate": 230400},
            {"laser_scan_dir": True},
            {"enable_angle_crop_func": False},
            {"angle_crop_min": 135.0},
            {"angle_crop_max": 225.0},
        ],
    )

#   top_ldlidar_node = Node(
#       package="ldlidar_stl_ros2",
#       executable="ldlidar_stl_ros2_node",
#       name="LD06_top",
#       output="screen",
#       parameters=[
#           {"product_name": "LDLiDAR_LD06"},
#           {"topic_name": "scan/top"},
#           {"frame_id": "base_laser"},
#           {"port_name": "/dev/ttyAMA3"},
#           {"port_baudrate": 230400},
#           {"laser_scan_dir": True},
#           {"enable_angle_crop_func": False},
#           {"angle_crop_min": 135.0},
#           {"angle_crop_max": 225.0},
#       ],
#   )

    # base_link to base_laser tf node
    base_link_to_laser_tf_node = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="base_link_to_base_laser_ld06",
        arguments=["0", "0", "0.18", "0", "0", "0", "base_link", "base_laser"],
    )
    ld = LaunchDescription()

    ld.add_action(front_ldlidar_node)
#   ld.add_action(top_ldlidar_node)
    ld.add_action(base_link_to_laser_tf_node)

    return ld
