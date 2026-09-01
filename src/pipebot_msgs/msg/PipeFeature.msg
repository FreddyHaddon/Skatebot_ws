# PipeFeature message that defines the pipe feature.

# Location types.  Extend this as needed.
uint8 LOCATION_UNKNOWN = 0
uint8 LOCATION_IN_PIPE = 1
uint8 LOCATION_MANHOLE = 2

# Location type must be defined from the above list.
uint8 location_type

# Location description.
uint8 exits
# Angles in radians from -Pi (not included) to Pi (inclusive).
# 0 angle = heading of the robot at all times.
# Positive angles are to the left.
float64[] angles_radians

# Pose of the robot in the manhole frame.
float64 pose_r
float64 pose_phi
float64 pose_theta
