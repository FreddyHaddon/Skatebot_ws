cmake_minimum_required(VERSION 3.8)
project(quat_to_euler)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(pipebot_msgs REQUIRED)

include_directories(include)

add_executable(quat_to_euler
  src/imu_euler.cpp
)
ament_target_dependencies(quat_to_euler
  rclcpp
  sensor_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  std_msgs
  pipebot_msgs
)

install(TARGETS
  quat_to_euler
  DESTINATION lib/${PROJECT_NAME})

install(PROGRAMS
  src/euler_relative.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME euler_relative
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

install(DIRECTORY
	launch
	DESTINATION share/${PROJECT_NAME}
)

ament_package()
