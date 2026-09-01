cmake_minimum_required(VERSION 3.8)
project(low_control_with_imu)

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
find_package(geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pipebot_msgs REQUIRED)

include_directories(include)

set(SRC_FILES
  src/imu_controller.cpp
  src/controller.cpp
)

set(PROJECT_EXEC ${PROJECT_NAME}_exec)
add_executable(${PROJECT_EXEC} ${SRC_FILES})

ament_target_dependencies(${PROJECT_EXEC}
  rclcpp
  sensor_msgs
  geometry_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  pipebot_msgs)

install(TARGETS
  ${PROJECT_EXEC}
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY
  config
  launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
