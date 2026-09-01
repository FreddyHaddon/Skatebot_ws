cmake_minimum_required(VERSION 3.5)
project(pipebot_msgs)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)


set(msg_files
  "msg/AcousticFeature.msg"
  "msg/Connected.msg"
  "msg/DataReceived.msg"
  "msg/Encoders.msg"
  "msg/EstimatedRobotState.msg"
  "msg/EulerAngles.msg"
  "msg/HighControl.msg"
  "msg/ImpulseResponse.msg"
  "msg/Leds.msg"
  "msg/LocationMeasurementProbability.msg"
  "msg/ManoeuvreState.msg"
  "msg/MotorControl.msg"
  "msg/MultipleWheelDrive.msg"
  "msg/PipeFeature.msg"
  "msg/PoseInStructure.msg"
  "msg/PositionPipe.msg"
  "msg/Rssi.msg"
  "msg/SensingPipeFeature.msg"
  "msg/Servo.msg"
  "msg/UltraData.msg"
)

set(srv_files
  "srv/SendData.srv"
)

set(act_files
  "action/AcousticSensing.action"
  "action/ActionAcousticSensing.action"
  "action/Manoeuvre.action"
  "action/ShotUltra.action"
  "action/ScanUltra.action"
)

rosidl_generate_interfaces(${PROJECT_NAME}
  ${msg_files}
  ${srv_files}
  ${act_files}
  DEPENDENCIES std_msgs
  ADD_LINTER_TESTS
)

ament_package()
