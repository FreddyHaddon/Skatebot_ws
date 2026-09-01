cmake_minimum_required(VERSION 3.5)
# Make sure the project name to matches the <name> value in package.xml.
project(rpi_monitor)

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# Find dependencies.
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(diagnostic_msgs REQUIRED)
find_package(diagnostic_updater REQUIRED)

include_directories(include)

# Source files.
set(SRC_FILES
  src/rpi_monitor.cpp
  src/rpi_monitor_node.cpp
)

set(PROJECT_EXEC ${PROJECT_NAME}_exec)
add_executable(${PROJECT_EXEC} ${SRC_FILES})

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${PROJECT_EXEC} PUBLIC -Wall -Wextra -pedantic)
endif()

ament_target_dependencies(${PROJECT_EXEC}
  rclcpp
  rclcpp_components
  diagnostic_msgs
  diagnostic_updater
)

install(TARGETS ${PROJECT_EXEC}
  # PROJECT_NAME __MUST__ be used here for launch files to work.
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
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
