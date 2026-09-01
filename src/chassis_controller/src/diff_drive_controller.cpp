// Copyright 2023 University of Leeds.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "diff_drive_controller.hpp"

#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <string>

#include "geometry_msgs/msg/twist.hpp"
#include "pipebot_msgs/msg/motor_control.hpp"
#include "pipebot_msgs/msg/multiple_wheel_drive.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/string.hpp"

#define USE_MODEL (1)

using namespace std::chrono_literals;

DiffDriveController::DiffDriveController()
    : Node("diff_drive_controller_node") {
  RCLCPP_INFO(get_logger(), "%s: Starting...", __func__);
#if USE_MODEL
  // Note: the QoS lengths are set to 1 to reduce latency.
  rclcpp::Parameter simulation = get_parameter("use_sim_time");
  if (simulation.as_bool()) {
    simulation_motors_publisher_ =
        this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "velocity_controller/commands", 1);
  } else {
#endif
    all_motors_publisher_ =
        create_publisher<pipebot_msgs::msg::MultipleWheelDrive>("all_motors",
                                                                1);
#if USE_MODEL
  }
#endif
  vel_subscriber_ = create_subscription<geometry_msgs::msg::Twist>(
      "controlled_cmd_vel", 1,
      std::bind(&DiffDriveController::CommandCallback, this,
                std::placeholders::_1));
  RCLCPP_INFO(get_logger(), "%s: Pub and sub created.", __func__);
#if USE_MODEL
  // Create QoS Profile with history_depth =1 (Its message queue size)
  rclcpp::QoS qos_profile(1);
  // Set to transient local durability policy so that the subscriber can always
  // read the robot description.
  qos_profile.transient_local();
  // Reliable policy is used to ensure that the subscriber receives all the
  // messages.
  qos_profile.reliable();
  robot_description_subscriber_ = create_subscription<std_msgs::msg::String>(
      "robot_description", qos_profile,
      std::bind(&DiffDriveController::DescriptionCallback, this,
                std::placeholders::_1));
#endif
  RCLCPP_INFO(get_logger(), "%s: Ready", __func__);
}

void DiffDriveController::CommandCallback(
    const geometry_msgs::msg::Twist::SharedPtr msg) {
  // RCLCPP_INFO(get_logger(), "%s: called", __func__);
  double wheel_distance = GetWheelDistance();
  // Convert from metres per second to radians per second.
  double wheel_radius = GetWheelRadius();
  // Only process if values from model are valid.
  if (wheel_distance > 0.0 || wheel_radius > 0.0) {
    // Radians per second.
    double angular_z_rad_s = static_cast<double>(msg->angular.z);
    double rotation_speed_effect = angular_z_rad_s * wheel_distance;
    // Metres per second.
    double linear_x_m_s = static_cast<double>(msg->linear.x);
    double right_m_s = linear_x_m_s + rotation_speed_effect;
    double left_m_s = linear_x_m_s - rotation_speed_effect;
    // Calculate the wheel speed in radians per second.
    double wheel_circumference = 2 * M_PI * wheel_radius;
    double meters_2_radians = 2 * M_PI / wheel_circumference;
    double left_rad_s = left_m_s * meters_2_radians;
    double right_rad_s = right_m_s * meters_2_radians;
    RCLCPP_INFO(get_logger(), "%s: left %f rad/s, right %f rad/s", __func__,
                left_rad_s, right_rad_s);
    // Publish message.
#if USE_MODEL
    rclcpp::Parameter simulation = get_parameter("use_sim_time");
    if (simulation.as_bool()) {
      PublishSimulationMessage(left_rad_s, right_rad_s, left_rad_s,
                               right_rad_s);
    } else {
#endif
      PublishAllMotorsMessage(left_rad_s, right_rad_s, left_rad_s, right_rad_s);
#if USE_MODEL
    }
#endif
  }
}

void DiffDriveController::DescriptionCallback(
    const std_msgs::msg::String::SharedPtr msg) {
  RCLCPP_INFO(get_logger(), "%s: called", __func__);
  if (!model_loaded_) {
    std::shared_lock lock(model_mutex_);
    if (!model_.initString(msg->data)) {
      RCLCPP_WARN(get_logger(),
                  "Failed to parse URDF/XACRO file, using hardcoded values");
      return;
    } else {
      RCLCPP_INFO(get_logger(), "Successfully parsed URDF/XACRO file");
      model_loaded_ = true;
    }
  }
}

double DiffDriveController::GetWheelDistance() {
  double wheel_distance = -1.0;
#if USE_MODEL
  if (model_loaded_) {
    // Locks until the end of scope (=this if)
    std::shared_lock lock(model_mutex_);
    std::shared_ptr<const urdf::Joint> wheel_joint =
        model_.getJoint("joint_front_left");
    if (wheel_joint != nullptr) {
      wheel_distance =
          std::abs(wheel_joint->parent_to_joint_origin_transform.position.y);
      RCLCPP_DEBUG(get_logger(), "%s: distance from robot center: %f", __func__,
                   wheel_distance);
    } else {
      RCLCPP_ERROR(get_logger(),
                   "%s: requested joint doesn't exist in the model", __func__);
    }
  } else {
    RCLCPP_ERROR(get_logger(), "%s: robot description isn't loaded", __func__);
  }
#else
  wheel_distance = 0.06;
#endif
  return wheel_distance;
}

double DiffDriveController::GetWheelRadius() {
  double wheel_radius = -1.0;
#if USE_MODEL
  if (model_loaded_) {
    // Locks until the end of scope (=this if)
    std::shared_lock lock(model_mutex_);
    std::shared_ptr<const urdf::Link> wheel_link = model_.getLink("front_left_wheel");
    std::shared_ptr<urdf::Geometry> geometry = wheel_link->visual->geometry;
    if (geometry->type == urdf::Geometry::CYLINDER) {
      urdf::Cylinder * wheel = dynamic_cast<urdf::Cylinder *>(geometry.get());
      RCLCPP_DEBUG(get_logger(), "%s: wheel_radius: %f", __func__, wheel->radius);
      wheel_radius = wheel->radius;
    } else {
      RCLCPP_ERROR(get_logger(), "%s: the link is expected to be cylinder", __func__);
    }
  } else {
    RCLCPP_ERROR(get_logger(), "%s: robot description isn't loaded", __func__);
  }
#else
  wheel_radius = 0.05;
#endif
  return wheel_radius;
}

void DiffDriveController::PublishAllMotorsMessage(double front_left_rad_s,
                                                  double front_right_rad_s,
                                                  double rear_left_rad_s,
                                                  double rear_right_rad_s) {
  auto all_motors_msg = pipebot_msgs::msg::MultipleWheelDrive();
  auto motor_control_msg = pipebot_msgs::msg::MotorControl();
  motor_control_msg.mode = pipebot_msgs::msg::MotorControl::MODE_RADIANS_SECOND;
  motor_control_msg.id = pipebot_msgs::msg::MotorControl::FRONT_LEFT;
  motor_control_msg.speed_radians_s = front_left_rad_s;
  all_motors_msg.motors.push_back(motor_control_msg);
  motor_control_msg.id = pipebot_msgs::msg::MotorControl::FRONT_RIGHT;
  motor_control_msg.speed_radians_s = front_right_rad_s;
  all_motors_msg.motors.push_back(motor_control_msg);
  motor_control_msg.id = pipebot_msgs::msg::MotorControl::REAR_LEFT;
  motor_control_msg.speed_radians_s = rear_left_rad_s;
  all_motors_msg.motors.push_back(motor_control_msg);
  motor_control_msg.id = pipebot_msgs::msg::MotorControl::REAR_RIGHT;
  motor_control_msg.speed_radians_s = rear_right_rad_s;
  all_motors_msg.motors.push_back(motor_control_msg);
  all_motors_publisher_->publish(all_motors_msg);
  RCLCPP_INFO(get_logger(), "Sent all_motors message");
}

void DiffDriveController::PublishSimulationMessage(double front_left_rad_s,
                                                   double front_right_rad_s,
                                                   double rear_left_rad_s,
                                                   double rear_right_rad_s) {
  auto message = std_msgs::msg::Float64MultiArray();
  message.data = {front_left_rad_s, front_right_rad_s, rear_left_rad_s,
                  rear_right_rad_s};
#if USE_MODEL
  simulation_motors_publisher_->publish(message);
#endif
  RCLCPP_INFO(get_logger(), "Sent sim message");
}
