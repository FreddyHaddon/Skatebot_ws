# Supervising Computer

TODO Change "supervising computer" to something better.

This document defines the ROS messages, services and actions between a pipebot and a supervising computer.

This document __does not__ consider what happens to the data gathered by the pipebot after it has been stored on the supervising computer.

## Table of Contents

- [Supervising Computer](#supervising-computer)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
    - [Responsibilities](#responsibilities)
  - [Interface message definitions](#interface-message-definitions)
    - [Subscriptions](#subscriptions)
      - [Acoustic range](#acoustic-range)
      - [Ultrasonic range](#ultrasonic-range)
      - [Camera images](#camera-images)
      - [Camera info](#camera-info)
      - [Battery status](#battery-status)
      - [IMU Calibrated](#imu-calibrated)
      - [Point Cloud Map](#point-cloud-map)
    - [XYZ pipe map](#xyz-pipe-map)
    - [Pose Graph](#pose-graph)
      - [Communications connected](#communications-connected)
      - [Received signal strength indicator](#received-signal-strength-indicator)
    - [Publishers](#publishers)
      - [Request IMU Calibration](#request-imu-calibration)
    - [Services](#services)
    - [Actions](#actions)
    - [Diagnostics](#diagnostics)
      - [Theme 2](#theme-2)
      - [Theme 3](#theme-3)
      - [Theme 4](#theme-4)
      - [Theme 5](#theme-5)
      - [Theme 6](#theme-6)
    - [Parameters](#parameters)
  - [Future work](#future-work)

## Overview

This section is intended to give the reader an overview of the general operation of a pipebot and supervising computer.  The supervising computer will most likely be a laptop that is used to monitor the performance of the pipebot(s) and store any data reported by the pipebot(s).

A typical pipebot survey mission might be as follows:

1. Before going to the survey location, the condition of the pipebot(s) is/are checked (self-tests performed, batteries charged etc.).
1. Once on site, the supervising computer will set the general mission parameters, e.g. map the sewer and return to base, and will also send the exact starting location to the pipebot (GPS co-ordinates or similar).
1. Once the pipebot has been started, the pipebot will perform the given mission autonomously.

    - If communications between the supervising computer and the pipebot can be maintained, video images and other data from the pipebot can be presented to the user.
    - If communications fail, the pipebot will continue to execute the mission, until done.  NOTE: How do we know if something has gone wrong with no communications?

1. When the pipebot returns to base, all data stored on the pipebot will be transferred to the supervising computer.
1. The supervising computer then transfers the recorded data to a server that also saves the data and may optionally perform some post processing, e.g. compare the data from this mission with a previous mission to identify any changes in the status of the pipe, generate human readable reports.

### Responsibilities

The supervising computer will be responsible for:

- Definition of the overall mission parameters, e.g.
  - Map from starting location to another manhole at this GPS location.
  - Explore while searching for defects and return to starting point.
- Storage of all data from the pipebot(s) that are necessary to generate customer reports.  This includes:
  - Asset mapping data - the location of the pipes.
  - Condition monitoring - the condition of the pipes including defects and blockages. This includes:
    - Pipe material.
    - Pipe construction method.
    - Pipe wall roughness.
    - Sediment build up.
    - Corrosion of pipe wall.
    - Location and nature of any blockages.
    - Intrusions into pipe.
    - Holes in pipe wall.
    - Failing joints.
    - Cracks in pipes.
    - Changes in pipe profile
    - Bio-film build up.
- Display of pipebot performance data such as battery status and any error conditions (when in communication range).
- Storage of all pipebot performance data for later analysis in case of failures.

__NOTE:__ The long list of features may not be fully implemented on all models of pipebots. For instance, Sprintbot can only report information that can be used to generate asset mapping data and the location of blockages.

## Interface message definitions

__NOTE__ These definitions are a work in progress and as such are subject to changes.

### Subscriptions

#### Acoustic range

Source: theme 2

The acoustic sensing module will provide this message with the values for the distance detection of blockage or other discontinuities in sewer pipes.

| Parameter | Value |
|-|-|
| Topic | acoustic_range |
| Message type |[msg/SensingPipeFeature](https://github.com/pipebots/pipebot-msgs/blob/t2-spec/msg/SensingPipeFeature.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

_Rationale:_ All blockages need to be reported to the customer.

#### Ultrasonic range

Source: theme 2

The ultrasonic sensing module will provide this message with the values for the distance detection of blockage or other discontinuities in sewer pipes.

| Parameter | Value |
|-|-|
| Topic | ultrasonic_range |
| Message type |[msg/SensingPipeFeature](https://github.com/pipebots/pipebot-msgs/blob/t2-spec/msg/SensingPipeFeature.msg) |
| QoS | Default |
| | |

_Rationale:_ All blockages need to be reported to the customer.

#### Camera images

Source: theme 3

| Parameter | Value |
|-|-|
| Topic | image_raw |
Message type | [sensor_msgs/Image](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Image.msg)
| QoS | Default |
| | |

_Rationale:_ The customer may want to examine the video data.  We need to store the highest resolution data from the camera somewhere (probably not using ROS).

#### Camera info

Source: theme 3

| Parameter | Value |
|-|-|
| Topic | camera_info |
| Message type | [sensor_msgs/CameraInfo](https://github.com/ros2/common_interfaces/blob/master/sensor_msgs/msg/CameraInfo.msg) |
| QoS | Default |
| | |

_Rationale:_ The customer may want to examine the video data.  This message provides supplementary information to the camera image messages.

#### Battery status

Source: theme 3

| Parameter | Value |
|-|-|
| Topic | battery |
| Message type | [sensor_msgs/BatteryState](https://github.com/ros2/common_interfaces/blob/master/sensor_msgs/msg/BatteryState.msg) |
| QoS | Best effort |
| Rate (per second) | 1 |
| | |

_Rationale:_ When being controlled by a person, the battery status is essential information.  This might be displayed on by user interface program run on the supervising computer.

#### IMU Calibrated

Source: theme 2

| Parameter | Value |
|-|-|
| Topic | platform/imu/calib_status|
| Message type | [pipebot_msgs/IMUCalibrationStatus](https://github.com/pipebots/pipebot-msgs/blob/main/msg/IMUCalibrationStatus.msg) |
| Rate (per second) | 1.0 |
| QoS | Default |
| | |

_Rationale:_ This message will be used to inform the operator about the calibration status of the IMU.

#### Point Cloud Map

Source: theme 5

| Parameter | Value |
|-|-|
| Topic | PointCloud2 |
| Message type | [sensor_msgs/PointCloud2](https://github.com/ros2/common_interfaces/blob/master/sensor_msgs/msg/PointCloud2.msg) |
| QoS | Default |

_Rationale:_ The point cloud data may be used to identify possible defects.  The processing for this may be done on or off the pipebot dependent on processing power.

### XYZ pipe map

Source: theme 5

| Parameter | Value |
|-|-|
| Topic | node_pose |
| Message type | [geometry_msgs/Pose](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/Pose.msg) |
| QoS | Default |
| | |

_Rationale:_ The pipe map is of great interest to the operator and the end customer.

### Pose Graph

Source: theme 5

| Parameter | Value |
|-|-|
| Topic | pose_graph |
| Message type | [geometry_msgs/PoseArray](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/PoseArray.msg) |
| QoS | Default |
| | |

_Rationale:_ The pipe map is of interest to the operator.

#### Communications connected

Source: theme 6

| Parameter | Value |
|-|-|
| Topic | comms/connected |
| Message type | [custom](https://github.com/pipebots/pipebot-msgs/blob/main/msg/Connected.msg) |
| QoS | Default |
| | |

_Rationale:_ Useful to know about communications on each pipebot to inform the operator.

#### Received signal strength indicator

Source: theme 6

| Parameter | Value |
|-|-|
| Topic | comms/rssi |
| Message type | [custom](https://github.com/pipebots/pipebot-msgs/blob/main/msg/Rssi.msg) |
| QoS | Default |
| | |

_Rationale:_ Useful to know about signal strength of communications on each pipebot to inform the operator.

### Publishers

#### Request IMU Calibration

Target: theme 4

| Parameter | Value |
|-|-|
| Topic | ask_imu |
| Message type | [std_msgs/Empty](https://github.com/ros2/common_interfaces/blob/foxy/std_msgs/msg/Empty.msg) |
| QoS | Default |
| | |

_Rationale:_ This message would be sent by the operator to ask the pipebot to calibrate the IMU when in maintenance mode.  Theme 4 is the overall controller so will decide when the calibration will happen, e.g. there is no point in calibrating when the pipebot is moving.

### Services

None.

### Actions

None.

### Diagnostics

Each theme may publish diagnostic messages.  When connection speed permits, all diagnostic messages being published by the pipebot can be monitored using the ROS runtime monitor program.

The theme 7 & 8 process will initially only have a fake implementation until more work is done on specifying how this process will interact with the operator.

The following is a list of the data being published in theme order.

#### Theme 2

None.

#### Theme 3

- Self test.
- Deadman Timer - Off/Triggered
- Emergency Stop - Off/On
- Headlights - Off/On
- Status: Left Motor Driver
- Status: Right Motor Driver
- Status: Left Encoder
- Status: Right Encoder
- Status: Battery

#### Theme 4

- Status: Forward
- Status: Soft Left
- Status: Soft Right
- Status: Hard Left
- Status: Hard Right
- Status: U-turn
- Status: Stop
- Status: Stuck
- Status: Sliding

#### Theme 5

- TBD

#### Theme 6

- Communications self test

### Parameters

When storing the information obtained from a survey, it will be useful to know which pipebot was used.  This information may be such things as:

- Pipebot UUID (universally unique identifier).
- Pipebot model and serial number
- Pipebot serial number.
- Camera make, model, serial number.

__NOTE:__ Many more parameters may well be stored on the pipebot.  It may be simplest to get all parameters and save a copy for each survey taken.  The size of the data will be a few kilobytes so will not be a problem.

## Future work

A maintenance mode will be provided at some point in the future.  This will do things like:

- Camera calibration.
- Acoustic calibration.
- Motor calibration.
- and much more!
