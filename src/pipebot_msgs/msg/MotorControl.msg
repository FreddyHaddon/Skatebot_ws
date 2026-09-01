# A list of the motor identifiers.  Add more if needed.
# Only used by /all_motors topic.
int8 LEFT = 0  # Not working yet.
int8 RIGHT = 1  # Not working yet.
int8 FRONT_LEFT = 2
int8 FRONT_RIGHT = 3
int8 REAR_LEFT = 4
int8 REAR_RIGHT = 5

# The motor identifier.  One of the above enums.
int8 id

# Control modes.
int8 MODE_ABSOLUTE=0
int8 MODE_RELATIVE=1
int8 MODE_SPEED=2
int8 MODE_DUTY=3
int8 MODE_RADIANS_SECOND=4

# The mode to use.  One of the above control mode values.
int8 mode

# The angle in radians. +ve angle is clockwise when looking at the
# motor from the drive shaft end.
# When mode = MODE_ABSOLUTE, set the angle relative to the starting position.
# The values should be in the range -PI to +PI radians.
# When mode = MODE_RELATIVE, move the motor angle degrees relative to the
# current position.
float64 angle_radians

# When mode = MODE_SPEED, the motor will change speed to the given RPM.
# The change in speed is controlled by the SDF file parameters update_rate and
# max_change_rpm.
# +ve rpm is anti-clockwise when looking at the motor from the drive shaft
# end.  This is one of those un-written standards.
float64 rpm

# When mode = MODE_DUTY, the motor will be controlled using the duty value.
# Range -100.0% to 100.0%.
# +ve duty is anti-clockwise when looking at the motor from the drive shaft
# end.
float64 duty

# When mode = MODE_RADIANS_SECOND, the motor will change speed to the given
# radians per second value.
# The change in speed is controlled by the SDF file parameters update_rate and
# max_change_rpm.
# +ve rotation is anti-clockwise when looking at the motor from the drive shaft
# end.  This is one of those un-written standards.
float64 speed_radians_s
