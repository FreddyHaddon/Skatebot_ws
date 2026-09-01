cmake_minimum_required(VERSION 3.5)
# Make sure the project name to matches the <name> value in package.xml.
project(joystick_action_controller)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Find dependencies.
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(pipebot_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

include_directories(include)

# Source files.
set(SRC_FILES
  src/acoustic_action_client.cpp
  src/joystick_action_controller.cpp
  src/joystick_node.cpp
  src/servo_turret_node.cpp
  src/ultrasonic_action_client.cpp
)

# set(TEST_ULTRASONIC_FILES
#   test/fake_ultrasonic_server.cpp
# )

set(PROJECT_EXEC ${PROJECT_NAME}_exec)
add_executable(${PROJECT_EXEC} ${SRC_FILES})
# add_executable(fake_ultrasonic_server ${TEST_ULTRASONIC_FILES})

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${PROJECT_EXEC} PUBLIC -Wall -Wextra -pedantic)
endif()

ament_target_dependencies(${PROJECT_EXEC}
  rclcpp
  rclcpp_action
  rclcpp_components
  pipebot_msgs
  sensor_msgs
)

# ament_target_dependencies(fake_ultrasonic_server
#   rclcpp
#   rclcpp_action
#   rclcpp_components
#   pipebot_msgs
# )

# install(TARGETS ${PROJECT_EXEC} fake_ultrasonic_server
install(TARGETS ${PROJECT_EXEC}
  # PROJECT_NAME __MUST__ be used here for launch files to work.
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  config
  launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  # Linting
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()  # BUILD_TESTING

# Installs the package.xml file.
ament_package()
