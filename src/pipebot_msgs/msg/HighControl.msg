uint8 cmd # High level control manoeuvre_cmd
# This commands is encoded as an unsigned integer: 0:255 commands
uint8 STOP=0
uint8 FORWARD=1
uint8 FORWARD_INS=2
uint8 BACKWARD=3
uint8 REVERSE=4
uint8 TURN_RIGHT=5
uint8 TURN_LEFT=6
uint8 SELF_BALANCE=7
int8 TRIG_ACOUSTIC=8
uint8 TRIG_ULTRASONIC=9
uint8 TRIG_CAM1=10
uint8 TRIG_CAM2=11
uint8 TRIG_CAM3=12
uint8 TRIG_CAM4=13
uint8 TRIG_CAM12=14
uint8 TRIG_CAM34=15
uint8 TRIG_CAMALL=16


# This commands will be added more codes in later control stages, but structure is the TwistStamped

int8 mode # Control modes: control target can be angular or linear or both
int8 MODE_DISTANCE=0 # Robot is given a distance to move as closed-loop position control
int8 MODE_SPEED=1    # Robot is given a speed to move as closed-loop velocity control

# Control data
float64 data_angle
float64 data_linear

# Inspection direction
uint8 inspect_dir
