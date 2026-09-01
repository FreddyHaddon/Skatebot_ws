# Control Module Specification

To minimise development effort, all pipebots shall offer a minimum standard
control software interface.

Rationale: A well defined minimum software interface allows the controlling
software to be developed independently of any hardware and/or simulation
implementations.

The software interface to all pipebot control modules should use ROS2
common interface messages and services from this repo:
<https://github.com/ros2/common_interfaces/tree/humble>.

Rationale:

* ROS2 has been chosen for the architecture.
* Micro-ROS can be used to implement these messages.
* Existing messages have been used to improve the chances of using existing
ROS2 packages, e.g. tele-op.
* This is a _should_ as there is no common interface message to control lights
so a custom message had to be defined.  When adding a new message, __please
check the common interface messages first__.

# Interface message definitions

## Subscriptions

### Battery status

The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | battery |
| Message type | [sensor_msgs/BatteryState](https://github.com/ros2/common_interfaces/blob/master/sensor_msgs/msg/BatteryState.msg) |
| QoS | Best effort |
| Rate (per second) | 1 |
| | |

NOTE: The topic `/battery` is for a single battery.  If multiple batteries are
used, a topic name and publisher should be added for each battery e.g.
`/battery_motors`, `/battery_computer`.

NOTE: QoS is set to "best effort" as messages can be lost without affecting the
operation of the system.

Rationale: Control shall prioritise actions depending on battery status.

### Range Type 1: If robot has multiple single range sensors:
The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | range |
| Message type | [sensor_msgs/Range](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/Range.msg) |
| QoS | Default |
| | |

Rationale: Basic range sensor; Useful to detect obstacles.

NOTE: The topic `/range` is for a single range sensors.  If n+1 range sensors are
used, a topic name and publisher should be added for each sensor i.e.
`/range_0` to `/range_n`.

### Range Type 2: If robot has one 360deg LiDAR sensor:
The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | Scan |
| Message type | [sensor_msgs/LaserScan](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/LaserScan.msg) |
| QoS | Default |
| | |

Rationale: This is a 360 degrees scanning LiDAR; Useful to detect obstacle, branches, junctions and many more.

### RSSI

RSSI stands for "Received Signal Strength Indicator". The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | comms/rssi |
| Message type | Custom - [draft](../msg/Rssi.msg) |
| QoS | Default |
| Rate | TBD |
| | |

The message type is TBD, but will likely contain the following fields:

| Name | Data Type |
|-|-|
| network_id | \<string\> or \<uint8\> |
| rssi_value | \<float32\> |
| rssi_units | \<string\> or \<uint8\> |
| | |

NOTE: Not used in current algorithm.

Rationale: This should help the Pipebot control algorithm to always keep the Pipebot within communications range. It is possible for the Pipebot to have several communications interfaces with different RSSI values, and messages for these will be published separately. The `Units` field is there to accommodate different ways of reporting the RSSI, e.g. `0-100` scale, absolute power level, etc.

### IMU

The control module shall subscribe to this message using the values:

| | |
|-|-|
| Topic | imu |
| Message type | [sensor_msgs/Imu](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/Imu.msg) |
| QoS | Default |
| | |

NOTE: Not used in current algorithm.

Rationale: IMU data is necessary for any complex control algorithm.

### Encoders

The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | encoder |
| Message type | [pipebots_msgs/Encoders](https://github.com/pipebots/t3-bot-test/blob/main/pipebot_msgs/msg/Encoders.msg) |
| QoS | Best effort |
| Rate (per second) | 20 |
| | |

NOTE: QoS is set to "best effort" as messages can be lost without affecting the
operation of the system.

NOTE: The topic `/encoder` is for a single encoder.  If multiple encoders are
used, a topic name and publisher should be added for each e.g.
`/encoder_left`, `/encoder_right`.

Rationale: This will be an input of the control algorithm.

NOTE: The full and up to date definition can be found in the file `Encoders.msg`.
It currently includes both the raw encoder count and conversions to relate this to wheel revolutions and angle.
This message can easily be modified and may be updated as the navigation stack develops.

### Acoustic_range

The acoustic sensing module will provide this message with the values for the distance detection of blockage or other discontinuities in sewer pipes. This message is based on the ROS2 common interface [Range message](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/Range.msg) modified for acoustic sensors.

| Parameter | Value |
|-|-|
| Topic | acoustic_range |
| Message type |[msg/SensingPipeFeature](https://github.com/pipebots/pipebot-msgs/blob/t2-spec/msg/SensingPipeFeature.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

_Rationale:_ Acoustic sensors are used for long-range detection (normally >2m) compared with ultrasonic methods. The detection range will provide the distance information of any blockages or other junctions of the pipe that can reflect acoustic wave. Additionally to ROS2 common interface message [range], Acoustic_range message also provide the information of sound source (from the local pipebot or other ones) and the receiver (microphone channel).

### Ultrasonic_range

The ultrasonic sensing module will provide this message with the values for the distance detection of blockage or other discontinuities in sewer pipes. This message is based on the ROS2 common interface [Range message](https://github.com/ros2/common_interfaces/blob/humble/sensor_msgs/msg/Range.msg) modified for ultrasonic sensors.

| Parameter | Value |
|-|-|
| Topic | ultrasonic_range |
| Message type |[msg/SensingPipeFeature](https://github.com/pipebots/pipebot-msgs/blob/t2-spec/msg/SensingPipeFeature.msg) |
| QoS | Default |
| | |

The definition of the message contents can be found in the message file.

_Rationale:_ Ultrasonic sensors are used for short-range detection (normally <2m) compared with acoustic methods. The detection range will provide the distance information of any blockages or other junctions of the pipe that can reflect ultrasonic wave. Additionally to ROS2 common interface message [range], ultrasonic_range message could also provide other information, such as pipe inner diameter.

### Odometry

The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | odometry |
| Message type | [nav_msgs/Odometry](https://github.com/ros2/common_interfaces/blob/humble/nav_msgs/msg/Odometry.msg) |
| QoS | Default |
| | |

NOTE: Not used in current algorithm.

### Ask IMU Calibration

The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | ask_imu |
| Message type | [std_msgs/Empty](https://github.com/ros2/common_interfaces/blob/humble/std_msgs/msg/Empty.msg) |
| QoS | Default |
| | |

Rationale: T5 will need the control to calibrate the IMU when localisation error becomes too high.

### Data received

The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | comms/data_received |
| Message type | Custom - [draft](../msg/DataReceived.msg) |
| QoS | Default (TBC) |
| Rate | On-event |
| | |

The message type is TBD, but will likely contain the following fields:

| Name | Data Type |
|-|-|
| dest_subsystem | \<string\> or \<uint8\> |
| source_id | \<string\> or \<uint16\> |
| payload | \<byte[]\> |
| | |

Rationale: Due to the asynchronous nature of communication over a network, the communication subsystem cannot know in advance when and what data will arrive for what other subsystem. Once such data arrives, it will be processed by the communication subsystem and published on this topic. At this stage, all data will be published as an array of bytes, and it will be up to the receiving subsystem to interpret those correctly.

## Publishers
### High level control
From the top level control, an autonomous control algorithm shall analyse the sensor data, make a high level decision and publish a message

| Parameter | Values |
|-|-|
| Topic | highcontrol |
| Message type | [pipebot_msgs/HighControl](https://github.com/pipebots/pipebot-msgs/blob/main/msg/HighControl.msg) |
| QoS | Default |
| Rate (per second) | 1 |
| | |

Rationale: The lower level commands will be sent on special conditions if needed incluse Twist and MotorControl which both should be calculated by LowControl
### Twist

The control module shall publish this message using the values:

| Parameter | Value |
|-|-|
| Topic | cmd_vel |
| Message type | [geometry_msgs/Twist](https://github.com/ros2/common_interfaces/blob/humble/geometry_msgs/msg/Twist.msg) |
| QoS | Default |

Rationale: Linear velocity control. Most turtle robots are controlled using this message.  SprintBot
has similar hardware to a turtle robot.   Using this standard message allows us
to manually drive the robots using the ROS2 package:
<https://github.com/ros2/teleop_twist_joy>

Only the linear X and Angular Z components of this message are used,
all other values are ignored.

NOTE: At some point, the `Twist` message will be replaced by the `TwistStamped`
message, see <https://github.com/ros-planning/navigation2/issues/1594>.

<https://github.com/ros2/common_interfaces/blob/humble/geometry_msgs/msg/TwistStamped.msg>

NOTE: Ideally, both `Twist` and `TwistStamped` messages should be implemented
but `Twist` is needed first.

### MotorControl

The control module shall publish this message using the values:

| Parameter | Value |
|-|-|
| Topic | cmd_motor |
| Message type | [pipebot_msgs/MotorControl](https://github.com/pipebots/pipebot-msgs/blob/main/msg/MotorControl.msg) |
| QoS | Default |
| Rate (per second) | 30 |
| | |

NOTE: The topic `/cmd_motor` is for a single wheel.  If multiple wheels are
used, a topic name and publisher should be added for each e.g.
`/cmd_right_motor`, `/cmd_left_motor`.

Rationale: Higher control of robot motion than differential driving. In some cases, we might need to rotate wheels for reason that are not platform related, such as, e.g., aligning them for self-assembly.

### IMU Calibrated

The control module shall subscribe to this message using the values:

| Parameter | Value |
|-|-|
| Topic | platform/imu/calib_status|
| Message type | [pipebot_msgs/IMUCalibrationStatus](https://github.com/pipebots/pipebot-msgs/blob/main/msg/IMUCalibrationStatus.msg) |
| QoS | Default |
| | |

Rationale: T5 will need to know when the IMU has been calibrated.

## Servers

### Take measurements

The client details are as follows:

| Parameter | Value |
|-|-|
| Topic | take_measurements |
| Message type | [std_srvs/Empty](https://github.com/ros2/common_interfaces/blob/master/std_srvs/srv/Empty.srv) |
| QoS | Default |
| | |

### Send data

The client details are as follows:

| Parameter | Value |
|-|-|
| Topic | comms/send_data |
| Message type | Custom (TBD) |
| QoS | Default |
| | |

The message type is TBD, but will likely contain the following fields:

* Request:

    | Name | Data Type |
    |-|-|
    | Source | \<string\> or \<uint16\> |
    | Source Ref | \<uint16\> |
    | Destination | \<string\> or \<uint16\> or \<byte[16]\> |
    | Payload | \<byte[]\> |
    | | |

* Response:

    | Name | Data Type |
    |-|-|
    | Status code | \<string\> or \<uint16\> |
    | | |

NOTE: This will not be necessary until we move to swarm behaviours.

Rationale: This covers the scenario where a particular subsystem wants to send some data to the outside world. The `Destination` can either be a descriptive name, e.g. "Blockage database" which will then get translated to an IP address, it can be a numeric code, or it can be a straight-up IP address. The `Source Ref` will be used when acknowledging that data has been sent, or when a response has arrived back.

The response itself will be a status code, such as "OK", "ACK Received", "ACK Not Received", etc.

### IMU calibration client

| Parameter | Value |
|-|-|
| Service | platform/imu/calibrate |
| Message type | [std_srvs/Empty](https://github.com/ros2/common_interfaces/blob/master/std_srvs/srv/Empty.srv) |
| QoS | Reliable |
| | |

__NOTE:__ The calibration may take some time depending on make and model of the IMU.

_Rationale:_ The IMU will need to be calibrated from time to time. The control driver decides when this calibration should take place.

## ROS2 Parameters

The Control Driver shouldn't need any parameter.

## Future

### Multi-robot

Multi-robot implementation is not yet considered. Eventually, the control driver will other robots' odometry to create a swarm behaviour. This message might eventually need to be coupled to some std_msgs (<https://github.com/ros2/common_interfaces/tree/humble/std_msgs/msg>), which would be published to the comms by the control driver.

### Maintenance mode

Implement maintenance mode: camera calibration, audio calibration, motor calibration, etc.
