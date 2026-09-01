# Acoustic and Ultrasonic Sensing Module Specification

This document defines the software interface that all Pipebots offer blockage detection and feature characterisation using the acoustic and ultrasonic sensing platform. The acoustic sensing should be implemented before the robot starts moving forward in a pipe network, the distance _Acoustic_range_ between the robot and the closest feature will be offered. The feature information _Acoustic_PipeFeatures_ will also be classified. T4 can use the messages to control the robot movement distance (**normally >2m**). When the robot stops at an distance (**normally <2m**) away the feature, the ultrasonic sensing should be implemented to measure the distance _Ultrasonic_range_ and the feature size, such as a blockage area. The information can also help T5 for the localisation of the robot in pipe networks. Additionally, the raw data can be saved in a SD card for signal post-processing to determine _UNCERTAIN_FEATURE_.

The interface will use ROS2 messages and actions from this <https://github.com/ros2/common_interfaces/tree/humble>, however some of the sensing module functionality will require custom messages and services for different requirements.

Rationale:

* ROS2 has been chosen for the architecture.
* The data is transmitted and received on a Pipebot or separate Pipebots.

# Interface message definitions

## Publisher

### Acoustic readings `ImpulseResponse`

The acoustic sensing module will provide this message with the values of acoustic impulse response recorded by two microphones in sewer pipes.

| Parameter | Value |
|---|---|
| Topic | `impulse_response` |
| Message type |[msg/ImpulseResponse](../msg/ImpulseResponse.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

_Rationale:_ Acoustic sensors are used for long-range detection (normally >2m) compared with ultrasonic methods.  This message is for scientific use.

### Ultrasonic range `SensingPipeFeature`

The ultrasonic sensing module will provide this message with the values for the distance detection of blockage or other discontinuities in sewer pipes.  This message is based on the ROS2 common interface [Range message](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/Range.msg) modified for ultrasonic sensors.

| Parameter | Value |
|---|---|
| Topic | `ultrasonic_range` |
| Message type |[msg/SensingPipeFeature](../msg/SensingPipeFeature.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

_Rationale:_ Ultrasonic sensors are used for short-range detection (normally <2m) compared with acoustic methods. The detection range will provide the distance information of any blockages or other junctions of the pipe that can reflect ultrasonic wave. Additionally to ROS2 common interface message [range], ultrasonic_range message also provides information such as pipe feature found and distance.

### Ultrasonic readings `UltraData`

This message is used to record experimental data so is completely custom.

| Parameter | Value |
|---|---|
| Topic | `ultra_read` |
| Message type |[msg/UltraData](../msg/UltraData.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

## Actions

### Acoustic sensing `AcousticSensing`

This action is used to trigger the playing of a waveform and a recording for the sound being returned to the robot.  Once the action is complete, the recorded data is published on the topic `impulse_response` for further analysis.

| Parameter | Value |
|---|---|
| Topic | `action_acoustic_sensing` |
| Action |[action/AcousticSensing](../action/AcousticSensing.action) |
| QoS | Default |
| | |

The definition of the action contents can be found in the message file.  Booleans are used for the request and the result, feedback is a percentage.

_Rationale:_ Acoustic sensors are used for long-range detection (normally >2m) compared with ultrasonic methods.  An action is used to trigger the playing of the waveform and recording of the data as this process takes several seconds.

### Ultrasonic Scan `ScanUltra`

This action is used to start a set of ultrasonic readings that takes several seconds to complete.  This action is different from `action/ShotUltra` in that the ultrasonic sensor is moved through a number of angles during the scan.  The angles are defined in the launch file or using ROS parameters.  Once the action is complete, the recorded data is published on the topic `ultra_read` for further analysis.

| Parameter | Value |
|---|---|
| Topic | `action_scanultra` |
| Action |[action/ScanUltra](../action/ScanUltra.action) |
| QoS | Default |
| | |

The definition of the action contents can be found in the message file.  Booleans are used for the request and the result, feedback is a percentage.

_Rationale:_ An action is used to trigger the sequence of ranging as this process takes several seconds.  This action is intended for scientific use.

### Ultrasonic Single Shot `ShotUltra`

This action is used to start a set of ultrasonic readings that takes several seconds to complete.  This action is different from `action/ShotUltra` in that the ultrasonic sensor is set to 0 degrees while the readings are taken.  Once the action is complete, the recorded data is published on the topic `ultra_read` for further analysis.

| Parameter | Value |
|---|---|
| Topic | `action_shotultra` |
| Action |[action/ShotUltra](../action/action/ShotUltra.action) |
| QoS | Default |
| | |

The definition of the action contents can be found in the message file.  Booleans are used for the request and the result, feedback is a percentage.

_Rationale:_ An action is used to trigger the sequence of ranging as this process takes several seconds.  This action is intended for scientific use.

## Node namespace: /sensing

### Parameter: sensors

* The each sensor can be simulated as a node and the position on the robot is determined by T3.

## Hardware Interface

This is to define which physical interface of the host computer is connected (Wi-Fi) to the sensing hardware (Raspberry Pi).
