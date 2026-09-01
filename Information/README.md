# Low Level Control

ROS 2 low level control package that uses the IMU to stop the robot falling over in a pipe.

Usage:

```text
ros2 launch low_control_with_imu low_control_with_imu.launch.py
```

You can set the critical roll in degrees in `low_control_with_imu/config/controller_params.yaml`. Critical roll means the angle at which a robot should stop because it is in immediate danger of rolling over.

The controller publishes on `/controlled_cmd_vel` so any node which would normally listen to `/cmd_vel` needs to listen to `/controlled_cmd_vel` instead.

## Related packages

There are three related packages in this repo:

* `chassis_controller` - This package converts `/controlled_cmd_vel` into messages that control the motors of the robot.
* `quat_to_euler` - This package converts quaterion values from the IMU into Euler angles published on the topic `imu/euler`.
* `robot_state_publisher` - This package converts values from the IMU and publishes odometry values using a `tf2_ros::TransformBroadcaster`.

## Non-standard dependent packages

The packages in this repo need the following repos added to the workspace used to build the code:

* `pipebot-msgs` - Custom messages for Pipebots.

In addition, the repo `pipebot_4wd` is required to test with a joystick.  `pipebot_4wd` is the only repo with joystick configuration files and complex launch files.

## Acknowledgments

This work is supported by the UK's Engineering and Physical Sciences Research Council (EPSRC) Programme Grant EP/S016813/1

© 2023, University of Leeds.

The authors, A. Blight and L. Mudrich, have asserted their moral rights.
