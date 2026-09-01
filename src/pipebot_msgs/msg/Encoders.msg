# Encoders message definition.

# Timestamp the data being sent
std_msgs/Header header

# Total raw encoder ticks
int64 ticks

# Total full revolutions of the wheel attached to the encoder
int64 wheel_revs

# Current angle of the wheel in DEGREES
float32 wheel_angle_deg
