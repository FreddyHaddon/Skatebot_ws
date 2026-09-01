# Single range reading from an active ranger that emits energy and reports
# one range reading that is valid along an arc at the distance measured.
#
# This message also can represent a fixed-distance (binary) ranger.  This
# sensor will have min_range===max_range===distance of detection.
# These sensors follow REP 117 and will output -Inf if the object is detected
# and +Inf if the object is outside of the detection range.

std_msgs/Header header # timestamp in the header is the time the ranger
                       # returned the distance reading

# Source of information values.
int8 ACOUSTIC=1
int8 ULTRASONIC=2
# The source of information value.
int8 source

float32 field_of_view   # the size of the arc that the distance reading is
                        # valid for [rad]
                        # the object causing the range reading may have
                        # been anywhere within -field_of_view/2 and
                        # field_of_view/2 at the measured range.
                        # 0 angle corresponds to the x-axis of the sensor.

float32 min_range       # minimum range value [m]
float32 max_range       # maximum range value [m]
                        # Fixed distance rangers require min_range==max_range

float32 range           # range data [m]
                        # (Note: values < range_min or > range_max should be discarded)
                        # Fixed distance rangers only output -Inf or +Inf.
                        # -Inf represents a detection within fixed distance.
                        # (Detection too close to the sensor to quantify)
                        # +Inf represents no detection within the fixed distance.
                        # (Object out of range)

# PipeFeatures definition.

# Blockage and discontinueties in pipes can reflect acoustic/ultrasonic wave
# which can be classified according to the information contained
# in the reflected wave.

# PipeFeature
int8 FEATURE_BLOCKAGE=0
int8 FEATURE_T_JUNCTION=1
int8 FEATURE_L_JUNCTION=2
int8 FEATURE_LATERAL=3
int8 FEATURE_MANHOLE=4
int8 FEATURE_JOINTS=5
int8 FEATURE_UNCERTAIN=6

# Pipe feaature value - one of the pipe feature values.
int8 pipe_feature

# Probability of pipe features, range 0.0 to 1.0
float32 probability

# Area of blockage as a percentage, 0% to 100%.
float32 blockage_area
