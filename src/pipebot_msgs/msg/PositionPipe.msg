# The position of a robot with respect to a pipe network.
# This position will have to be understood with respect to a pipe network map.
# The position is given in the format of (discrete index of location, discrete direction in the location, continuous position in the location)

std_msgs/Header header   

int64 index
int64 direction
float32 position