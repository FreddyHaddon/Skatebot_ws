#!/usr/bin/env python3

import os
from glob import glob
from setuptools import setup

package_name = "pipebot_4wd"

setup(
    name=package_name,
    version="0.0.0",
    packages=[package_name],
    data_files=[
        ("share/ament_index/resource_index/packages", ["resource/" + package_name]),
        ("share/" + package_name, ["package.xml"]),
        # Include model and simulation files
        (os.path.join("share", package_name + "/description"), glob("description/*.xacro")),
        (os.path.join("share", package_name + "/config"), glob("config/*.rviz")),
        (os.path.join("share", package_name + "/config"), glob("config/*.yaml")),
        (
            os.path.join("share", package_name + "/launch"),
            glob("launch/*launch.[pxy][yma]*"),
        ),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="Andy Blight",
    maintainer_email="a.j.blight@leeds.ac.uk",
    description="Main repo for the Pipebot 4 wheel drive robot",
    license="MIT",
    tests_require=["pytest"],
    entry_points={
        "console_scripts": [
            # "state_publisher = " + package_name + ".state_publisher:main"
            "lidar_subscriber = pipebot_4wd.lidar_subscriber:main",
            "lidar_average_subscriber = pipebot_4wd.lidar_average_distance:main",
            "lidar_segmentation_subscriber = pipebot_4wd.lidar_segmentation:main",
            "lidar_kmeans_subscriber = pipebot_4wd.lidar_kmeans_clustering:main",
            "lidar_DBSCAN_subscriber = pipebot_4wd.lidar_DBSCAN:main",
            "camera_preview_node = pipebot_4wd.camera_preview_node:main",
        ],
    },
)
