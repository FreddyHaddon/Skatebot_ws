# Design Notes

## High level design

As the Dynamixel hardware uses a USB serial interface, this is what we use.  This gives us the following rough design.

```text
 __________________________
| RPi                      |     ___________       ___________       ___________
| ________________  USB    |    |           |     |           |     |           |
| | ROS2 Wrapper |--------------| Dynamixel |-----| Dynamixel |-...-| Dynamixel |
| |              | Serial  |    | Interface |     | 1         |     | N         |
| |______________|         |    |___________|     |___________|     |___________|
|__________________________|

```

The ROS 2 wrapper handles the communications with the rest of the system using the following topics:

| Topic | Pub/sub | Message |
|---|---|---|
| `/motor/cmd_vel` | Subscriber | `geometry_msgs/msg/Twist` |
| `/motor/left` | Subscriber | `pipebot_msgs/msg/MotorControl` |
| `/motor/right` | Subscriber | `pipebot_msgs/msg/MotorControl` |
| `/encoder/left` | Publisher | `pipebot_msgs/msg/Encoders` |
| `/encoder/right` | Publisher | `pipebot_msgs/msg/Encoders` |
| `/servo/turret` | subscriber | `pipebot_msgs/msg/Servo` |

The messages used are defined in the `pipebot_msgs` package.

## Communicating with the Dynamixels

The wrapper also communicates with the Dynamixel SDK to send and receive data. The Dynamixel SDK provides very low level methods to talk to one or more Dynamixels.  Some good example code can be found here:
<https://github.com/ROBOTIS-GIT/DynamixelSDK/blob/ros2/dynamixel_sdk_examples/src/read_write_node.cpp>

The ROS 2 wrapper defines an interface class with high level commands like this:

```c++
void SetRPM(const MotorInstance motor, double rpm);
```

What this command will do is send commands to a pair of Dynamixels that sets the RPM in whatever units the Dynamixels use.  It was quickly realised that there was a split of responsibilities needed, the first to setup the serial port and the second to manage the motors.

## The `DynamixelComms` class

The `DynamixelComms` class is responsible for:

* Setting up the Dynamixel `PortHandler` and `PacketHandler` objects.
* Creating, owning and destroying all instances of the `DynamixelServo` class and forwarding calls to the correct instance.
* Reporting connection status.

## The `DynamixelServo` class

The `DynamixelServo` class is responsible for:

* Implementing the communications to the attached Dynamixel servo.
* Initialising and terminating the servo.
* Sending commands to the servo to control speed, absolute position and relative position.
* Obtaining encoder pulse counts from the servo.
* Obtaining status information from the servos about voltage, temperature etc.

Much of the class was based on example code from the Dynamixel SDK repo.

## Physical arrangement

Motors are arranged like this.

|       | Left | Right |
|---|---|---|
| Front | 2 | 1 |
| Rear  | 3 | 4 |

The turret servo is ID 0.

Initially, the arrangement was not correct and it caused some very strange behaviour.  The above arrangement was verified with the Dynamixel Wizard 2.0 tool.  The Dynamixels follow the convention where motors with clockwise rotation turn the output shaft clockwise when looking at the motor from the output shaft end.  Once the above problem was corrected, the behaviour of the robot what fully tested using the ROS 2 commands below and found to be correct.

## Watchdog feature

We had a problem where the control package died and the robot kept on moving until it fell over.  We need to add a feature to stop if no message is received for a preset time.  The control package sends out messages to the motor every 200ms.  The node `teleop_twist_joy_node` sends messages between 5Hz and 20Hz. So, a timeout of 300ms seems like a good starting point.

The next question is where to put the time out.  The `twist` message calls the function `SetRPM` as do the motor topics, so putting the timer logic inside the `DynamixelComms` class looks good.  this proved to be a pain as the ROS 2 functions to manage a time need to be in a node, so the logic has to go into the `MotorNode` class instead.

## Total Encoder Count

The encoder count reported by the Dynamixel servos that are used as wheel motors works well until a mode change occurs when the encoder count is modified.  The natural place to implement logic that works around this problem is in the `DynamixelServo` class.

The encoder values are read on a timer thread and operating modes can be changed on a separate thread, so thread safety needs to be considered.  We should also consider thread safety for all the Dynamixels as they are all connected using a single serial port, so this serial port should be mutex protected.  Implementing a mutex in the `DynamixelComms` class also solved the thread safety issues in the `DynamixelServo` class.

The updating of the value `total_encoder_count_` is implemented in a new function, `DynamixelServo::UpdateEncoderCount`.  This function is called by two other functions, `DynamixelServo::SetOperatingMode` and `DynamixelServo::GetTotalEncoderCount`.

### Case 1

The function `DynamixelServo::GetTotalEncoderCount` will be called at least once a second.  The action in this case is to read current position value, work out the change in value (current - last) and add the change to the running total.

### Case 2

The function `DynamixelServo::SetOperatingMode` can change the operation mode to any of the four different modes offered by the Dynamixels.  However, we are currently only using two modes,     `kVelocityControlMode` and `kExtendedPositionControlMode`, so this makes things a little easier.

When in `kVelocityControlMode`, the encoder count goes up to +/-2^31 counts.  When in `kExtendedPositionControlMode`, the encoder count goes up to +/-1048575 counts (around 256 revs each way).  This was tested and the  Dynamixel firmware limits the values that can be input to +/-1048575.  Sending an out of range value causes the Dynamixel to respond with a data value out of range message.

By experimenting using the Dynamixel wizard 2.0 program, the following operations were needed to use `kExtendedPositionControlMode` after an extended period of using `kVelocityControlMode`.

1. Drive motor so that the value of the `Present Position` register is greater than 1048575 (2^20).
2. Turn off torque.
3. Reboot the servo to reset the encoder count to within range.
4. Change to `kExtendedPositionControlMode`.
5. Enable torque.

This approach was implemented in the function `DynamixelServo::SetOperatingMode` and found to work correctly.

### Total encoder count logic

The logic needed to store the total number of encoder counts was then implemented.  Case 1 was simple to implement and was added to a new function `UpdateEncoderCount`.  This new function was called before returning the total in the function `GetTotalEncoderCount`.  After some thought, it was realised that the case 2 logic needed two function calls to `UpdateEncoderCount`.  The first call would read the current position and add the delta to the total, exactly the same as case 1, and the second call after the reboot to set the new value of the servo position to calculate the delta next time the function `UpdateEncoderCount` was called.

### Other considerations

The other concern about the encoder values is wrap round at counts of +/-2^31.  This value is equal to a 129882 full revolutions, so for the SkateBot with a wheel diameter of 100mm, it equals a total distance travelled of 2 * PI * 0.05m * 129882 = 40,803m or 40.8km.  This is not going to happen during scientific experiments so can be ignored.

## Turret rotation

The Skatebot has a turret that rotates 180 degrees controlled by a single servo.  The servo is connected to the turret by a 3D printed gear (blacklash city!) with a ratio of around 2:1. When using the Dynamixel Wizard to control the turret servo, the values that turned the turret though 180 degrees was -200 to 4000, so a range of just over the complete single turn of 4096 was needed.  As we need to rotate the servo though more than one complete turn, we need to use extended position mode.  The exact gear ratio is 4200 / 2048 = 2.05, so we need to add some compensation into the mix somewhere.

The servo message specifies an input range of -360 to +360 degrees, with increasing values rotating the servo in an anti-clockwise direction when looking at the end of the output shaft.  After the gear ratio is applied, an input value of 180 degrees should rotate the turret 180 degrees.

Modifications were made to many files as the communications interface needed to be changed, mostly from `int16_t` to `double` to cope with the gear ratio being applied.  The gear ratio is set in the launch file as a parameter `gear_ratio` so can be modified for other robots.  A velocity profile was also used on the turret the default settings tried to turn the turret too fast.  See `SetVelocityProfile()` for details.

## Problems found during development

### Servo motor overload

I had several reports of one of the servo motors driving the robot wheels failing after a few minutes of driving.  After a week or so of testing, I finally caught the problem.

```text
...
[low_control_with_imu_exec-5] [INFO] [1706267202.612377233] [low_control_with_imu]: vel_callback: -0.0 -0.0
[low_control_with_imu_exec-5] [INFO] [1706267202.620308655] [low_control_with_imu]: vel_callback: 0.0 0.0
[low_control_with_imu_exec-5] [INFO] [1706267202.775325369] [low_control_with_imu]: timer_callback: publishing 0.0 0.0
[low_control_with_imu_exec-5] [INFO] [1706267202.975282755] [low_control_with_imu]: timer_callback: publishing 0.0 0.0
... (70 more the same)
[low_control_with_imu_exec-5] [INFO] [1706267217.175205570] [low_control_with_imu]: timer_callback: publishing 0.0 0.0
[low_control_with_imu_exec-5] [INFO] [1706267217.375157735] [low_control_with_imu]: timer_callback: publishing 0.0 0.0
[low_control_with_imu_exec-5] [INFO] [1706267217.575054271] [low_control_with_imu]: timer_callback: publishing 0.0 0.0
[dynamixel_driver_exec-7] [WARN] [1706267217.706989750] [dynamixel_driver.servo]: HardwareStatusToString: Id 4, status 32
[low_control_with_imu_exec-5] [INFO] [1706267217.775131620] [low_control_with_imu]: timer_callback: publishing 0.0 0.0
[dynamixel_driver_exec-7] [ERROR] [1706267217.898992716] [dynamixel_driver.servo]: [RxPacketError] Hardware error occurred. Check the error at Control Table (Hardware Error Status)!
...
```

From this trace, the robot was stopped for about 15 seconds before the error occurred and the motor has been shutdown (de-torqued).  The hardware status value of 32 means "Detects that persistent load that exceeds maximum output".  The robot was on the carpet of our office, short pile and hard wearing, so lots of friction.  My guess it that the overload was happened because the affected servo was trying to move a last little bit but couldn't, although that doesn't make much sense as the motor is in velocity mode and should not care about position.

My options are:

1. Stop the motors shutting down when an overload occurs.
2. Reboot the affected servo when an overload error happens.
3. Replace all the motors with stronger ones.

Option 2 won as it keeps the robot working for longer and hopefully rebooting the motor will also stop whatever is causing the overload.  It looks like the status information could also do with being updated to report the fact that the motor has overloaded.

#### Implementation

The first problem is where to put the logic in the driver and the second is how to trigger the reboot. `DynamixelComms.cpp` is too low level but needs a `Reboot` function to be added.  This was implemented and tested.  The control of the reboot functionality was added to this function `DiagnosticsNode::AddServoStatusMessage()`.

### Velocity profile

One problem that was found when testing the extended position mode was that one of the motors would shutdown after several minutes of testing turning.  No logs were captured, so the best guess is that the motor overheated or over torqued and shut down to protect itself.  This was prevent when using the `twist` messages by limiting the requested input when turning.  The robot would also try to move very rapidly which also made control very difficult.

To prevent the motor shutting down, extended position mode now has a profile set with a maximum velocity of 20RPM and an acceleration of 10 (2145.77 rev/min^2).  This should also make the robot move more smoothly as jerked around before.

## Dynamixel Bulk Mode

When using the motor message to turn the wheel by a set amount, say 2 radians, the motors on the same side start at different times.  This is messy at best and really difficult to control.  Try bulk mode to solve this and then extend to the `all_motors` message.

Call tree for motors message - turn set amount.

Message callback calls:
  comms_->SetRelativePosition(instance_, angle_radians_);
or
  comms_->SetRPM(instance_, rpm_);

comms_->SetRelativePosition(instance_, angle_radians_);
calls
    motor_front_left_.SetRelativePosition(angle_radians);
    motor_rear_left_.SetRelativePosition(angle_radians);

This calls:
  ConfigureControlMode(kExtendedPositionControlMode);
  int32_t position = GetPresentPosition();
  SetGoalPosition(goal_position);

This will not work well with bulk mode so I need a new message.  The new message will set both motors at the same time using bulk mode so there is only one path through the code and it is completely new.  So the task list is:

1. Create new message in pipebots messages.
2. Add new subscriber.
3. Add new interfaces to comms and dynamixel comms.
4. Get bulk mode working.

Decided to modify the existing `MulipleWheelDrive` message to use an array of the `MotorControl` message.  `MotorControl` message has motor ID and new mode to rotate at radians per second values. Now to get the message info down to the Dynamixel driver.  Done using two functions: `SetAllRPM` and `SetAllRelativePosition`.  These two functions will use bulk mode calls.  Time to write that code.

There is a simple example of using bulk read and write here: <https://github.com/ROBOTIS-GIT/DynamixelSDK/blob/master/c%2B%2B/example/protocol2.0/bulk_read_write/bulk_read_write.cpp>
Looks straight forward: initialise, add param multiple times, tx, clear.

Implemented the code.  Took much longer than I thought, 4 hours or so.  Now to test the new code.

The good news is that the new messages work and that bulk mode causes the motors to move at more or less the same time.  The bad news is that the motors turn for different amounts.  This seems to be because there is a reboot taking place and on the MX-64ATs I'm using for testing, the reboot is much slower than the MX-28ATs.  Time to look at the mode switching code again.

Fixed the function `Reboot` so it now polls until ready after reboot.  Reboot speed much improved but servos still not turning properly.

Just found out that the command I'm using causes the motors to do absolute position mode.  Changed the mode value in the ROS command line and no change.  The problem must be lower down.  Code now rejects mixed mode settings.  The problem is that the deadman's switch code is stopping the motors early.

Found the problem with the position not being relative. `GetCurrentPositions()` fails with `Comms error: [TxRxResult] There is no status packet!.` so the movement is similar to absolute mode.  Spent a while trying to get the bulk read to work but to no avail.  Ended up using the `GetPresentPosition()` function for each servo and the code now works.

Testing on the robot shows that 20RPM for turning on the spot is a bit fast.  Changed to 10RPM and it looks OK for now.  Just spotted another problem, the enums in the message do not match with numbers for the motors.  Simple fix but where is the problem?  Sent single motor messages to find out.

```bash
ros2 topic pub -1 /all_motors pipebot_msgs/msg/MultipleWheelDrive "{motors: [ {id: 2, mode: 1, angle_radians: 3.1}] }"
ros2 topic pub -1 /all_motors pipebot_msgs/msg/MultipleWheelDrive "{motors: [ {id: 3, mode: 1, angle_radians: 3.1}] }"
ros2 topic pub -1 /all_motors pipebot_msgs/msg/MultipleWheelDrive "{motors: [ {id: 4, mode: 1, angle_radians: 3.1}] }"
ros2 topic pub -1 /all_motors pipebot_msgs/msg/MultipleWheelDrive "{motors: [ {id: 5, mode: 1, angle_radians: 3.1}] }"
```

| Msg number | Msg name | Actual | +v value rotation |
|---|---|---|---|
| 2 | FRONT_LEFT| Front left | Clockwise |
| 3 | FRONT_RIGHT | Front right | Clockwise |
| 4 | REAR_LEFT | Rear left | Clockwise |
| 5 | REAR_RIGHT | Rear right | Clockwise |

So the directions are all correct and so are all the motors.  My bad.  Nothing wrong, I just assumed something that was wrong.

## TODO

- Add code to spit out message if wrong Dynamixel model is connected.
-