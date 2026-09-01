cmake_minimum_required(VERSION 3.8)
project(chassis_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(pipebot_msgs REQUIRED)
find_package(urdf REQUIRED)

include_directories(include)

# This package can build multiple executables and libraries.
set(EXEC_NAME chassis_controller_exec)
# The names of these executables are listed below.
add_executable(${EXEC_NAME}
  src/controller.cpp
  src/diff_drive_controller.cpp
)

ament_target_dependencies(${EXEC_NAME}
  rclcpp
  std_msgs
  geometry_msgs
  pipebot_msgs
  urdf
)
# Add other executable names here.


install(TARGETS
  ${EXEC_NAME}
  # Add other executable names here.
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
