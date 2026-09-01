# The robot state estimated on inputs: lidar range sensors, IMU, and wheel encoders

# This robot state is published so support localization, which then will be fed back to control to self-correct

# Timestamp the data being sent
std_msgs/Header header
string str_state        # Immediate state by Text
int8 num_state          # Immediate state by Number
int8[] robotstates      # Array of historical states, last element is the current/most updated state
                        # State value and meaning
                        # has the size HISTORY_SIZE, FIFO array, robotstates[HISTORY_SIZE-1] is latest
int8 LEFT_BRANCH = 0
int8 STRAIGHT_CENTER = 1
int8 STRAIGHT_SIDED = 2
int8 SIDED = 3
int8 LEFT_TURN = 4
int8 CRASH_LEFT = 5
int8 CRASH_RIGHT = 6
int8 CROSS = 7
int8 T_JUNCTION = 8
int8 RIGHT_TURN = 9
int8 RIGHT_BRANCH = 10
int8 BLOCKAGE = 11
int8 DEAD_END = 12
int8 FLIP_RISK = 13
int8 SELF_CORRECT = 14
int8 MANHOLE = 15
int8 OPENSPACE = 16
int8 OP_ENDED = 17

