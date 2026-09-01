# Localization and Mapping (Theme 5) Module Specification

Theme 5 Module high level rationale:

The goal is to develop (at least) two packages, one for localization and one for mapping. This is to ensure localization (i.e. Odometry) is always available on sprintbot. Mapping will be available as compute resources/communications allow.

The localization package will initially be designed to take encoder and IMU data (and robot geometry) and return Odometry information. Other packages may be developed that return similar Odometry outputs based on other available sensors .e.g. based on range or acoustic measurements.

The mapping package will take camera and IMU data and return a point cloud map for visualization. In future a different map format may be used which can inform control (e.g. occupancy grid maps).

Theme 5 will also implement a map server to inform localization in a known pipe network, to provide up-to-date maps formed by the robot (i.e. SLAM), or to inform control based on location within a pipe.

# Interface message definitions

## Subscriptions

### Camera images

To run visual odometry/visual SLAM algorithms we require access to raw camera images.

| Parameter | Value |
|-|-|
| Topic | camera/image_raw |
| Message type | [sensor_msgs/Image](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Image.msg) |
| QoS |  Best effort |
| Rate (per second) | 20 |
| | |

NOTE: QoS will need to be reviewed/tested to determine the minimum acceptable level, but 20Hz is approximately what is seen in the visual odometry literature.

Rationale: cameras are often used for visual odometry/SLAM, are widely available and small enough for a pipebot's goal overall size (unlike, say, a LIDAR array).

### Encoders

Simple 'dead reckoning' odometry requires access to the wheel encoder values.

| Parameter | Value |
|-|-|
| Topic | encoder |
| Message type | [pipebots_msgs/Encoders](https://github.com/pipebots/t3-bot-test/blob/main/pipebot_msgs/msg/Encoders.msg) |
| QoS | Best effort |
| Rate (per second) | 20 |
| | |

NOTE: QoS is set to "best effort" as messages can be lost without affecting the operation of the system.

NOTE: The topic `/encoder` is for a single encoder.  If multiple encoders are used, a topic name and publisher should be added for each e.g.
`/encoder_left`, `/encoder_right`.

_Rationale:_ Positional information is required for mapping the pipe network.
There is no ROS2 common interface message for lower level encoder information, just higher level [Odometry](https://github.com/ros2/common_interfaces/blob/foxy/nav_msgs/msg/Odometry.msg) which was deemed unsuitable for this case.
The full and up to date definition can be found in the file `Encoders.msg`.
It currently includes both the raw encoder count and conversions to relate this to wheel revolutions and angle.
This message can easily be modified and may be updated as the navigation stack develops.

### IMU

IMU values will be combined with encoder values and/or camera images to produce odometry outputs and/or point cloud maps.

| | |
|-|-|
| Topic | imu |
| Message type | [sensor_msgs/Imu](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Imu.msg) |
| QoS | Best Effort |
| Rate (per second) | 100 |
| | |

Rationale: 100 Hz sampling is a good target and is a standard in the literature for what we have seen. Note, the higher the better.

### Range

Alongside the camera/IMU sensor information, the on-board range sensors are likely to be very useful/important for accurate state estimation.

| | |
|-|-|
| Topic | range |
| Message type | [sensor_msgs/Range](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Range.msg) |
| QoS | Default |
| | |

Rationale: Basic range sensor already implemented for obstacle avoidance.

NOTE: The topic `/range` is for a single range sensors.  If n+1 range sensors are
used, a topic name and publisher should be added for each sensor i.e.
`/range_0` to `/range_n`.

### Acoustic_range

Theme 5 will subscribe to, and make use of, acoustic and ultrasonic 'range' information provided by Theme 2 for robot localization.

| Parameter | Value |
|-|-|
| Topic | acoustic_range |
| Message type |[msg/SensingPipeFeature](https://github.com/pipebots/pipebot-msgs/blob/t2-spec/msg/SensingPipeFeature.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

*Rationale*: In theory acoustic range messages will provide information about distances to far objects (2-10s of meters) which can be used for robot localization.

### Ultrasonic_range

Theme 5 will subscribe to, and make use of, acoustic and ultrasonic 'range' information provided by Theme 2 for robot localization.

| Parameter | Value |
|-|-|
| Topic | ultrasonic_range |
| Message type |[msg/SensingPipeFeature](https://github.com/pipebots/pipebot-msgs/blob/t2-spec/msg/SensingPipeFeature.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

*Rationale*: Ultrasonic sensors are used for short-range detection (normally <2m) compared with acoustic methods. The detection range will provide the distance information of any blockages or other junctions of the pipe that can reflect ultrasonic wave. This information can be used for robot localization. Additionally ultrasonic_range messages could provide other information, such as pipe inner diameter, which could be recorded in a map.

### Diagnostics

Theme 5 will need access to a number of run-time hardware/software configuration parameter such as: robot geometry information (stored in a YAML file?),
LED status (for illumination),
camera operation status (FPS, exposure time/settings if available),
IMU/other sensor operating info (modes, sampling rates etc).

Some of which will be covered by the micro-ROS Configuration Service.

### micro-ROS Configuration Service

The client will read the YAML file and send a service request to the
microcontroller containing those values. The response will be a simple acknowledgement.

| Parameter | Value |
|-|-|
| Service | pipebot_msgs/srv/updateMicroConfig |
| Request Message | pipebot_msgs/msg/locoParams config |
| Return Message type | bool success, string message|
| | |

Rationale: to ensure any Theme 5 algorithms have updated parameters for good operation.

### Camera info

| Parameter | Value |
|-|-|
| Topic | camera_info |
| Message type | [sensor_msgs/CameraInfo](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/CameraInfo.msg) |
| QoS | Default |
| | |

Rationale: This information may be important for setting parameters of visual odometry methods.

### IMU/Camera synchronization status

*WIP* This would store the relative timing of IMU and Camera reads. Perhaps it's determined by comparing time stamps, perhaps it is enforced by e.g. the teensy triggering the camera shutter.

Format TBD

### Future: Motor commands/state

Alongside the encoder information the motor commands are likely to be very useful/important for accurate state estimation. In addition the motor state of the robot should probably be considered e.g. moving/not moving; doing an ultrasound test; initializing. This would ensure the state estimation is only being done when it's supposed to (but when is it supposed to??).

As a placeholder the below messages are copied from the platform-specification.md.

### Twist

| Parameter | Value |
|-|-|
| Topic | cmd_vel |
| Message type | [geometry_msgs/Twist](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/Twist.msg) |
| QoS | Default |
| | |

### MotorControl

| Parameter | Value |
|-|-|
| Topic | cmd_motor |
| Message type | [gazebo_ros_simple_motor_msgs/MotorControl](https://github.com/pipebots/ros2-gazebo-simple-motor-plugin/blob/main/gazebo_ros_simple_motor_msgs/msg/MotorControl.msg) |
| QoS | Default |
| Rate (per second) | 30 |
| | |

### Future: Odometry/sensor information/point clouds/maps from other robots

Rationale: in future the pipebots project will work towards multi-robot operation, for control and localization/mapping. It is worth considering, even at this stage, how this might be done.

## Publishers

### Odometry

Theme 5 will provide odometry estimates, based on encoders/IMU and images if compute resources are available

| Parameter | Value |
|-|-|
| Topic | odometry |
| Message type | [nav_msgs/Odometry](https://github.com/ros2/common_interfaces/blob/foxy/nav_msgs/msg/Odometry.msg) |
| QoS | Default |
| | |

### Point Cloud Map

*WIP* ~~It's not clear what map format we should be aiming to provide. We haven't looked in anger at algorithms that return e.g. grid maps as found in nav_msgs/OccupancyGrid](https://github.com/ros2/common_interfaces/blob/foxy/nav_msgs/msg/OccupancyGrid.msg)~~

Following the conventions of [VINS-Fusion](https://github.com/HKUST-Aerial-Robotics/VINS-Fusion) we will return a point cloud 'map', which can be used for vizualization and (ultimately) conversion into occupancy/grid maps for control.

| Parameter | Value |
|-|-|
| Topic | point_cloud |
| Message type | [sensor_msgs/PointCloud2](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/PointCloud2.msg) |
| QoS | Default |
| | |

### XYZ pipe map

Some localization algorithms require a known prior map. We will provide this using a mapping server. Current plan is to use graph based representations following [graph_mapping](http://library.isr.ist.utl.pt/docs/roswiki/graph_mapping.html) and [topological_navigation](http://wiki.ros.org/topological_navigation).

From what I can see a graph can be shared by sending the pose of the nodes (plus some other TBD info)

| Parameter | Value |
|-|-|
| Topic | node_pose |
| Message type | [geometry_msgs/Pose](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/Pose.msg) |
| QoS | Default |
| | |

Rationale: This is a more compact representation of the world map than a point cloud or grid map. Graphs are how many pipe networks are stored in databases, and it is how some of our algorithm expect a map to be provided.

### Pose Graph

*WIP* In addition to a point cloud map and instantaneous odometry estimates Theme 5 will provide a Pose Graph of the history of camera poses. NOTE there are a number of pose/pose array formats including time stamps, frames of reference or covariance. It is unclear at this stage which format is most appropriate.

| Parameter | Value |
|-|-|
| Topic | pose_graph |
| Message type | [geometry_msgs/PoseArray](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/PoseArray.msg) |
| QoS | Default |
| | |

Rationale: PoseArray has been chose initially over the other options as it seems closest to what we are currently able to offer. Here are all of the common_interface Pose messages:

[geometry_msgs/PoseArray](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/PoseArray.msg). PoseArray: An array of poses with a header for global reference.

[geometry_msgs/Pose](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/Pose.msg). Pose: A representation of pose in free space, composed of position and orientation.

[geometry_msgs/PoseStamped](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/PoseStamped.msg). PoseStamped: A Pose with reference coordinate frame and timestamp.

[geometry_msgs/PosewithCovariance](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/PoseWithCovariance.msg). PoseWithCovariance: A pose in free space with uncertainty.

[geometry_msgs/PoseWithCovarianceStamped](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/PoseWithCovarianceStamped.msg). PoseWithCovarianceStamped: An estimated pose with a reference coordinate frame and timestamp.

### FUTURE: Performance diagnostics/ stop and start requests

NOTE: don't use diagnostic for this. Needs to be a proper topic.

It's probably a good idea to output some localization/mapping performance diagnostic if possible. E.g. it should be possible to re-initialize the IMU if the drift is too large. This will require the robot to stop moving.

It's also possible to imagine an initialization motor routine that invoves a stereotyped movement to help calibrate the camera/IMU parameters. Theme 5 could request for this to be done, for Theme 4 to schedule.

### Ask IMU Calibration

Theme 5 will publish this message to inform the control module that the IMU (and other sensors potentially) need to be re-calibrated. Note as of 09.02.21 the IMU cannot be re-calibrated during operation

| | |
|-|-|
| Topic | ask_imu |
| Message type | [std_msgs/Empty](https://github.com/ros2/common_interfaces/blob/foxy/std_msgs/msg/Empty.msg) |
| QoS | Default |
| | |

Rationale: The IMU may accumulate significant drift during robot operation. Theme 5 will monitor this and send a message to the control module indicating the IMU needs to be restarted/recalibrated.

## ROS2 Parameters Overview

To quote from: <https://index.ros.org/doc/ros2/Tutorials/Parameters/Understanding-ROS2-Parameters/>

> A parameter is a configuration value of a node. You can think of parameters
> as node settings. A node can store parameters as integers, floats, booleans,
> strings and lists. In ROS 2, each node maintains its own parameters.

In practice, ROS2 parameters are often read from a YAML file when a node is
started, so they should be considered as read only by other nodes.  This allows
the same code to be used for different robots by changing the YAML files.

## ROS2 Parameters Specification

Rational: To make the node more portable to other hardware a number of values
should be set as parameters allowing them to be changed at runtime without
rebuilding the node. Currently this feature is not available  in micro-ros,
hence limited specification below, but is expected May 2021 (in line with ROS2
Galactic release). In the meantime a config file is used as part of the
micro-ros code on the Teensy but this must be recompiled to make changes.

The ROS parameters for the platform module are specified below.

TODO Fill out this section with meaningful info.  I have had a go to show
how it might work.

### Node namespace: /theme5-slam

#### Parameter: TO DO

TODO Add something about what this parameter is for and what it means.  Consider:

* Valid range and units if numerical.  I like to put the units on the end of
the variable name.
* Valid strings if used like an enum.
* Structure of the list for lists.

#### NOTES:

Camera calibration. Add as a process to run camera calibration when sprintbot is in 'desktop/maintenance mode' vs driving about. Further, it is important to check/set the camera settings for good operation.

Lights: what determines the LED light mode/brightness? This could be done as part of the camera calibration/startup routine.

Map => Odometry => base_link coordinate frames
Following from [REP 105](https://www.ros.org/reps/rep-0105.html) it is important to compute/maintain the correct coordinate frames for Sprintbot. Unclear who's job this is - probably Theme 5...
