# Low Level Controller with IMU Stabilisation

This package is intended to allow the a robot to stay in the bottom of a pipe whilst moving forward or reversing.  If the robot is slightly off-centre, the robot will start to climb the wall of the pipe and eventually roll over.  This controller is designed to detect smallish changes in the pitch and take action based to prevent the roll over.

## ROS Interfaces

The controller subscribes to the following messages: `/cmd_vel`, `/imu/euler` and `/joy` and publishes the corrected values on `/controlled_cmd_vel`.

There are several parameters that can be set in a launch file or using the file
`low_control_with_imu/config/controller_params.yaml`.

Critical roll, pitch and yaw means the angle at which a robot should stop because it is in immediate danger of falling over.  At this point, the robot will stop and refuse to move so you will need to recover the robot manually!

There are 2 D-pad parameters: `d_pad_forward_key` that sets which key (axis) on the joystick controller to use and `d_pad_speed_m_s` that sets the speed of the robot when using the D-pad.

## Design notes

The basic idea is that the controller uses the IMU input to correct the orientation of the robot.

The controller will also stop the robot if no message is received for a preset time.

Euler angles are used by the driver as this is the most intuitive way to understand the attitude of the robot.

Twist messages are only sent when the deadman's buttons are pressed, even when the D-pad is being used.  The low control publisher only publishes when twist messages are being received on `/cmd_vel` due to the time out feature built into this module.

### Kinematic model of the robot

From a random paper!
x˙ i = vi cos θi, y˙i = vi sin θi, ˙θi = ωi
