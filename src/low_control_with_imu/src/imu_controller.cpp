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

#include "imu_controller.hpp"

#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"

// TODO(Lenka): remove this
using namespace std::chrono_literals;

IMU_Controller::IMU_Controller()
: Node("low_control_with_imu")
{
  imu_roll_degrees_ = 0.0;
  imu_pitch_degrees_ = 0.0;
  imu_yaw_degrees_ = 0.0;
  d_pad_direction_ = 0;

  // All parameters are in degrees.
  declare_parameter("critical_roll", 30.0);
  critical_roll_degrees_ = this->get_parameter("critical_roll").as_double();
  declare_parameter("critical_pitch", 30.0);
  critical_pitch_degrees_ = this->get_parameter("critical_pitch").as_double();
  declare_parameter("critical_yaw", 720.0);
  critical_yaw_degrees_ = this->get_parameter("critical_yaw").as_double();

  // Defaults are for Logitech controller.  Uses axis 7.
  declare_parameter("d_pad_forward_key", 7);
  d_pad_forward_key_ = get_parameter("d_pad_forward_key").get_parameter_value().get<int>();
  // D Pad speed in m/s.
  declare_parameter("d_pad_speed_m_s", 0.1);
  d_pad_speed_m_s_ = get_parameter("d_pad_speed_m_s").get_parameter_value().get<double>();
  
  // Create pubs and subs.
  imu_subscriber_ = this->create_subscription<pipebot_msgs::msg::EulerAngles>(
    "imu/euler_relative", 10, std::bind(&IMU_Controller::imu_callback, this, std::placeholders::_1));
  joy_subscriber_ = create_subscription<sensor_msgs::msg::Joy>(
    "joy", 10, std::bind(&IMU_Controller::joy_callback, this, std::placeholders::_1));
  vel_subscriber_ = this->create_subscription<geometry_msgs::msg::Twist>(
    "cmd_vel", 10, std::bind(&IMU_Controller::vel_callback, this, std::placeholders::_1));
  vel_publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("controlled_cmd_vel", 1);
  timer_ = this->create_wall_timer(200ms, std::bind(&IMU_Controller::timer_callback, this));
  {
    std::unique_lock lock(timer_mutex_);
    time_since_last_twist_msg_ = std::chrono::system_clock::now();
  }
  RCLCPP_INFO(get_logger(), "%s: Started", __func__);
}

void IMU_Controller::imu_callback(const pipebot_msgs::msg::EulerAngles::SharedPtr msg)
{
  std::unique_lock lock(imu_mutex_);

  imu_roll_degrees_ = msg->roll;
  imu_pitch_degrees_ = msg->pitch;
  imu_yaw_degrees_ = msg->yaw;
}

void IMU_Controller::joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg)
{
  // RCLCPP_INFO(get_logger(), "%s: Called", __func__);
  // Store for later use.
  d_pad_direction_ = static_cast<int>(msg->axes[d_pad_forward_key_]);
}

void IMU_Controller::vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg)
{
  // RCLCPP_INFO(get_logger(), "%s: Called", __func__);
  // Limit scope of mutex lock.
  {
    std::unique_lock lock(timer_mutex_);
    time_since_last_twist_msg_ = std::chrono::system_clock::now();
  }
  // Limit scope of mutex lock.
  {
    std::unique_lock lock(twist_mutex_);
    twist_msg_ = *msg;
    // RCLCPP_INFO(get_logger(), "%s: %0.1f %0.1f", __func__, twist_msg_.linear.x, twist_msg_.angular.z);
  }
}

void IMU_Controller::timer_callback()
{
  // RCLCPP_INFO(get_logger(), "%s: Called", __func__);
  // This function takes a while to run so copy the mutex protected values to local variables.
  bool timeout = false;
  double roll_degrees = 0.0;
  double pitch_degrees = 0.0;
  double yaw_degrees = 0.0;
  {
    // Limit scope of mutex lock.
    const auto now = std::chrono::system_clock::now();
    std::shared_lock lock(timer_mutex_);
    std::chrono::duration<double> elapsed_seconds = now - time_since_last_twist_msg_;
    if (elapsed_seconds.count() > k_when_to_stop_s_) {
      timeout = true;
    }
  }
  {
    // Limit scope of mutex lock.
    std::shared_lock lock(imu_mutex_);
    roll_degrees = imu_roll_degrees_;
    pitch_degrees = imu_pitch_degrees_;
    yaw_degrees = imu_yaw_degrees_;
  }
  // Now work out what to do.
  RCLCPP_INFO(
    get_logger(), 
    "%s: relative roll %0.1f pitch %0.1f yaw %0.1f",
    __func__, roll_degrees, pitch_degrees, yaw_degrees);
  geometry_msgs::msg::Twist new_twist_msg;
  new_twist_msg.linear.x = 0.0;
  new_twist_msg.linear.y = 0.0;
  new_twist_msg.linear.z = 0.0;
  new_twist_msg.angular.x = 0.0;
  new_twist_msg.angular.y = 0.0;
  new_twist_msg.angular.z = 0.0;
  
  // Stop the robot if we haven't received a message for a while.
  if (timeout) {
    // Leave the twist message as zero.
    RCLCPP_INFO(get_logger(), "%s: timeout", __func__);
  } else if (
    std::abs(roll_degrees) > critical_roll_degrees_ ||
    std::abs(pitch_degrees) > critical_pitch_degrees_)
  {
    // Leave the twist message as zero.
    RCLCPP_WARN(
      get_logger(),
      "%s: critical tilt reached: roll=%.1f pitch=%.1f yaw=%.1f",
      __func__,
      roll_degrees,
      pitch_degrees,
      yaw_degrees);
  } else {
    
    if (d_pad_direction_ != 0) {
      // We have a D-pad command.  Use it.
      new_twist_msg.linear.x = d_pad_speed_m_s_ * d_pad_direction_;
      new_twist_msg.angular.z = 0;
      RCLCPP_INFO(get_logger(), "%s: desired d-pad %0.1f %0.1f", __func__,
                  new_twist_msg.linear.x, new_twist_msg.angular.z);
    } else {
      // Use the cmd_vel message.
      {
        std::shared_lock lock(twist_mutex_);
        new_twist_msg.linear.x = twist_msg_.linear.x;
        new_twist_msg.angular.z = twist_msg_.angular.z;
      }
      RCLCPP_INFO(get_logger(), "%s: desired vel   %0.1f %0.1f", __func__,
                  new_twist_msg.linear.x, new_twist_msg.angular.z);
    }
    // Now correct for roll.
    // 8 degrees roll prevents over-correction. NOTE: IMU reads -3.5 when flat.
    // Any turn greater than 0.1 rad/s is not corrected.
    if ((std::abs(roll_degrees) > 8.0) && (std::abs(new_twist_msg.angular.z) < 0.1)) {
      double error_degrees = k_required_roll_degrees_ - roll_degrees;
      double new_angular_speed = error_degrees * angular_P_;
      new_twist_msg.angular.z = -new_angular_speed;
    }
    // Slow the robot down significantly if we have lots of roll.
    if (std::abs(roll_degrees) > 20.0) {
      new_twist_msg.linear.x /= 3.0;
    }
  }
  vel_publisher_->publish(new_twist_msg);
   RCLCPP_INFO(
    get_logger(),
    "%s: publishing %.2f %.2f | relative roll %.1f pitch %.1f yaw %.1f",
   __func__,
    new_twist_msg.linear.x,
    new_twist_msg.angular.z,
   roll_degrees,
   pitch_degrees,
   yaw_degrees);
}
