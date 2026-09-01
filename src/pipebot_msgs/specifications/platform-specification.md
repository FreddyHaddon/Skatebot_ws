# Platform Specification

To minimise development effort, all Pipebot platforms shall offer a minimum standard set of sensor and actuator interfaces. The interfaces are:

* Motor control
* LEDs
* IMU
* Camera
* Battery Status
* Diagnostics

_Rationale:_ A well defined generic software interface allows the controlling software to be developed independently of any hardware and/or simulation implementations. Although we aim to keep this specification generic there are some places where we have geared the specification towards Sprintbot to speed development.  The message have been updated for the Swarm Trooper software and the pipebot_4wd series of robots.

Whenever possible the software interfaces use ROS2 common interface messages and services from this repo:
<https://github.com/ros2/common_interfaces/tree/humble>.

_Rationale:_

* ROS2 has been chosen for the architecture.
* These messages are also compatible with micro-ROS.
* We can re-use existing ROS2 packages, e.g. tele-op.

# Interface message definitions

## Subscribers

### Twist

The platform module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | cmd_vel |
| Message type | [geometry_msgs/Twist](https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/Twist.msg) |
| QoS | Default |
| | |

_Rationale:_ Most turtle robots are controlled using this message and robots such as SprintBot have similar differential drive hardware to a turtle robot.
This standard message allows us to manually drive the robots using the ROS2 package: <https://github.com/ros2/teleop_twist_joy>

Only the linear X and Angular Z components of this message are used,
all other values are ignored.

NOTE: At some point, the `Twist` message will be replaced by the `TwistStamped`message, see <https://github.com/rosplanning/navigation2/issues/1594>.

<https://github.com/ros2/common_interfaces/blob/foxy/geometry_msgs/msg/TwistStamped.msg>

NOTE: Ideally, both `Twist` and `TwistStamped` messages should be implemented
but `Twist` is needed first.

### MotorControl

The platform module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | motor |
| Message type | [pipebot_msgs/MotorControl](https://github.com/pipebots/pipebot-msgs/blob/main/msg/MotorControl.msg) |
| QoS | Default |
| | |

NOTE: The topic `/motor` is for a single motor.  If multiple motors are
used, a topic name and publisher should be added for each e.g.
`/motor/right`, `/motor/left`.

_Rationale:_ This message allows an individual motor to be controlled. This low level of control provides flexibility for driving robots that don't use differential drive. It also allows fine-grained motor control to be abstracted to another module.

Note: The modes of this message may or may not be implemented on a given platform.  For instance, when DC motors are being used, the duty cycle mode should be implemented to allow calibration.  The absolute and relative modes only make sense when encoders are fitted.

Note: This message was originally defined in the [motor gazebo plug-in](https://github.com/pipebots/ros2-gazebo-simple-motor-plugin) but has been migrated to the `pipebot-msgs`` repo to keep all custom messages in one place to improve maintainability.

### MultipleWheelDrive

The platform module may provide a subscriber to this message using the values:

| Parameter | Value |
|-|-|
| Topic | all_motors |
| Message type | [pipebot_msgs/MultipleWheelDrive](https://github.com/pipebots/pipebot-msgs/blob/main/msg/MultipleWheelDrive.msg) |
| QoS | Default |
| | |

_Rationale:_ This message allows the speed of all motors to be set at once.

NOTE: The speeds and motor identifiers are defined by the sub-message [MotorControl](https://github.com/pipebots/pipebot-msgs/blob/main/msg/MotorControl.msg).

### LEDs

The platform module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | leds |
| Message type | [pipebot_msgs/Leds](https://github.com/pipebots/pipebot-msgs/blob/main/msg/Leds.msg) |
| QoS | Default |
| | |

_Rationale:_ There is no ROS2 common interface message to control LEDs so a custom message will be used.  The full and up to date definition can be found in the file `Leds.msg`.

This message enumerates the LED names and modes. The common ones we expect to use are included but may be expanded if required. These can safely be added to, as if the hardware does not support the requested LED/colour/mode it simply ignores the message, but the existing items should not be removed or reordered to avoid breaking existing software.

### Servo

The platform module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | servo |
| Message type | [pipebot_msgs/Servo](https://github.com/pipebots/pipebot-msgs/blob/main/msg/Servo.msg) |
| QoS | Default |
| | |

NOTE: The topic `/servo` is for a single motor.  If multiple servos are used, a topic name and publisher should be added for each e.g. `/servo/right`, `/servo/left`.

_Rationale:_ This message allows an individual servo to be controlled.

Notes: Servos may or may not be fitted depending on the robot. Most servos operate in the range 0 - 180 degrees but some have a range of 0 - 359 degrees.

## Publishers

### Encoders

The platform module shall publish this message using the values:

| Parameter | Value |
|-|-|
| Topic | encoder |
| Message type | [pipebot_msgs/Encoders](https://github.com/pipebots/pipebot-msgs/blob/main/msg/Encoders.msg) |
| QoS | Best effort |
| Rate (per second) | 20 |
| | |

NOTE: QoS is set to "best effort" as messages can be lost without affecting the operation of the system.

NOTE: The topic `/encoder` is for a single encoder.  If multiple encoders are used, a topic name and publisher should be added for each e.g.
`/encoder/left`, `/encoder/right`.

_Rationale:_ Positional information is required for mapping the pipe network.  There is no ROS2 common interface message for lower level encoder information. There is the higher level [Odometry](https://github.com/ros2/common_interfaces/blob/foxy/nav_msgs/msg/Odometry.msg) but this was deemed unsuitable for this use case.  The full and up to date definition can be found in the file `Encoders.msg`. It currently includes both the raw encoder count and conversions to relate this to wheel revolutions and angle. This message can easily be modified and may be updated as the navigation stack develops.

### IMU

Not all topics listed below may be published as this is dependent on the IMU being used.  As a minimum, all platforms shall publish the topic `/imu/imu` using the `sensor_msgs/Imu` message.

| Parameter | Value |
|-|-|
| Topic | /imu/imu |
| Message type | [sensor_msgs/Imu](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Imu.msg) |
| Rate (per second) | 100 |
| Topic | /imu/imu_raw |
| Message type | [sensor_msgs/Imu](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Imu.msg) |
| Rate (per second) | 100 |
| Topic | /imu/temp|
| Message type | [sensor_msgs/Temperature](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Temperature.msg) |
| Rate (per second) | 100 |
| Topic | /imu/mag|
| Message type | [sensor_msgs/MagneticField](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/MagneticField.msg) |
| Rate (per second) | 100 |
| Topic | /imu/calib_status|
| Message type | [pipebot_msgs/IMUCalibrationStatus](https://github.com/pipebots/pipebot-msgs/blob/main/msg/IMUCalibrationStatus.msg) |
| Rate (per second) | 1.0 |
| | |

Default QoS is used for all messages.

_Rationale:_ Every Pipebot requires a minimum of one IMU for navigation so one is specified for the platform. This IMU should publish as a generic device but currently the only supported hardware uses the Bosch BNO055 sensor that incorporates on-board sensor fusion.
The calibration status is published as 4 integer values for sys/gyro/acc/mag and a single Boolean value, calibrated.  Each of the 4 integer values are in the range 0 (not calibrated) to 3 (fully calibrated).  The boolean value is true when all integer values equal 3.

### Camera

When a camera is fitted to a pipebot, the platform module shall publish this message:

| Parameter | Value |
|-|-|
| Topic | /camera/image |
| Message type | [sensor_msgs/Image](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Image.msg) |
| Rate (per second) | Configurable |
| | |

_Rationale:_ Most pipebots will have a camera.  Theme 5 can use these messages for SLAM.  Theme 7 and 8 need video recordings for the customer.  Video recordings will probably streamed directly to a storage device rather be streamed live.   See <https://github.com/klintan/ros2_usb_camera> for an example implementation.

When a camera is fitted to a pipebot, the platform module will publish this message:

| Parameter | Value |
|-|-|
| Topic | /camera/camera_info |
| Message type | [sensor_msgs/CameraInfo](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/CameraInfo.msg) |
| Rate (per second) | Implementation dependent |
| | |

_Rationale:_ This information can be used to to guide post-processing of the camera images.  This may or may not be implemented dependent on the pipebot being used (image processing power may be limited).  For example, the camera package used on Sprintbot is <https://gitlab.com/boldhearts/ros2_v4l2_camera>.  Also this camera only publishes the `camera_info` topic when the subscriber is part of the same ROS2 process.

### Range

When suitable hardware is fitted, the platform module shall publish to this message using the values:

| Parameter | Value |
|-|-|
| Topic | `range` |
| Message type | [sensor_msgs/Range](https://github.com/ros2/common_interfaces/blob/foxy/sensor_msgs/msg/Range.msg) |
| QoS | Default |
| | |

Rationale: All robots need some idea of their surroundings.  5 time of Flight sensors were used on the Sprintbot, so we need to support this feature.  Other Pipebots can use different numbers or types of sensors.

NOTE: The topic `/range` is for a single range sensor.  If n+1 range sensors are
used, a topic name and publisher should be added for each sensor i.e.
`/range_0` to `/range_n`.

### Laser Scan

When suitable hardware is fitted, the platform module shall publish to this message using the values:

| Parameter | Value |
|-|-|
| Topic | `laser_scan` |
| Message type | [sensor_msgs/msg/LaserScan.msg](https://docs.ros2.org/latest/api/sensor_msgs/msg/LaserScan.html) |
| QoS | Default |
| | |

Rationale: All robots need some idea of their surroundings.  The `LaserScan.msg` should be used for full rotating lidars or solid state lidar where many time of flight sensors scan a single plane.

### Diagnostics

Diagnostics are useful during debugging and production.  ROS has a well
established set of tools for providing diagnostic data so we might as well use
it.

#### Self test service

The platform module shall implement this service using the values:

| Parameter | Value |
|-|-|
| Topic | self_test |
| Message type | [diagnostic_msgs/srv/SelfTest](https://github.com/ros2/common_interfaces/blob/master/diagnostic_msgs/srv/SelfTest.srv) |
| QoS | Default |
| | |

_Rationale:_ The self test will be used before a pipebot is used and for
verifying hardware connections.  It should light LEDs in sequence and operate
all actuators (motors or anything else) in a sequence to verify that the
hardware is operating correctly.

#### Diagnostic message

The platform module shall publish this message using the values:

| Parameter | Value |
|-|-|
| Topic | diagnostics |
| Message type | [diagnostic_msgs/DiagnosticArray](https://github.com/ros2/common_interfaces/blob/master/diagnostic_msgs/msg/DiagnosticArray.msg) |
| QoS | Best effort |
| Rate (per second) | 1 |
| | |

NOTE: QoS is set to "best effort" as messages can be lost without affecting the
operation of the system.

_Rationale:_ Regular diagnostic messages can be used to monitor a pipebot and detect fault conditions.  The diagnostic messages can include generic `(key, value)` pairs so can easily be extended. Using this format allows standard ROS tools to be used for monitoring and analysis.

A diagnostic array `(diagnostic_msgs/msg/DiagnosticArray)` is a set of component level statuses `(diagnostic_msgs/msg/DiagnosticStatus)`.
Each status can in turn contain an array of key-values pairs `(diagnostic_msgs/msg/KeyValue)`.

_Rationale:_ The "Platform Driver" status covers all aspects of the platform module which are not component specific.  As each robot wil be different, the data passed in this message may be defined as the project requires.

### Battery status

The platform module shall publish this message using the values:

| Parameter | Value |
|-|-|
| Topic | battery |
| Message type | [sensor_msgs/BatteryState](https://github.com/ros2/common_interfaces/blob/master/sensor_msgs/msg/BatteryState.msg) |
| QoS | Best effort |
| Rate (per second) | 1 |
| | |

NOTE: The topic `battery` is for a single battery.  If multiple batteries are
used, a topic name and publisher should be added for each battery e.g.
`battery_motors`, `battery_computer`.

NOTE: QoS is set to "best effort" as messages can be lost without affecting the
operation of the system.

_Rationale:_ It is very useful to know the current state of the batteries.

## Services

None.

## Actions

None.

## ROS2 Parameters Overview

To quote from: <https://index.ros.org/doc/ros2/Tutorials/Parameters/Understanding-ROS2-Parameters/>

> A parameter is a configuration value of a node. You can think of parameters
> as node settings. A node can store parameters as integers, floats, booleans,
> strings and lists. In ROS 2, each node maintains its own parameters.

In practice, ROS2 parameters are often read from a YAML file when a node is
started, so they should be considered as read only by other nodes.  This allows
the same code to be used for different robots by changing the YAML files.

Currently, we only use ROS 2 parameters for passing values from launch files to nodes.
