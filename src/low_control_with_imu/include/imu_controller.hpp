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

#ifndef IMU_CONTROLLER_HPP_
#define IMU_CONTROLLER_HPP_

#include <math.h>

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <string>

#include "geometry_msgs/msg/twist.hpp"
#include "pipebot_msgs/msg/euler_angles.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "sensor_msgs/msg/joy.hpp"

class IMU_Controller : public rclcpp::Node
{
public:
  IMU_Controller();

private:
  void imu_callback(const pipebot_msgs::msg::EulerAngles::SharedPtr msg);
  void joy_callback(const sensor_msgs::msg::Joy::SharedPtr msg);
  void vel_callback(const geometry_msgs::msg::Twist::SharedPtr msg);
  void timer_callback();

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_publisher_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr vel_subscriber_;
  rclcpp::Subscription<pipebot_msgs::msg::EulerAngles>::SharedPtr imu_subscriber_;
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_subscriber_;

  // IMU variables.
  mutable std::shared_mutex imu_mutex_;
  double imu_roll_degrees_{0.0};
  double imu_pitch_degrees_{0.0};
  double imu_yaw_degrees_{0.0};

  // Initial angles
  bool imu_initial_set_{false};
  double imu_roll_initial_degrees_{0.0};
  double imu_pitch_initial_degrees_{0.0};
  double imu_yaw_initial_degrees_{0.0};
  double angle_difference_degrees(double current_degrees, double initial_degrees) const;
  
  // DPAD key values from parameters.
  int d_pad_forward_key_;
  double d_pad_speed_m_s_;
  // D-pad variables.
  std::atomic_int d_pad_direction_;

  // Critical angles from parameters.
  double critical_roll_degrees_{0};
  double critical_pitch_degrees_{0};
  double critical_yaw_degrees_{0};

  // Constants for the roll controller.
  const double angular_P_{0.03};
  const double k_required_roll_degrees_{0.0};
  const double k_when_to_stop_s_{0.5};

  mutable std::shared_mutex twist_mutex_;
  geometry_msgs::msg::Twist twist_msg_;

  mutable std::shared_mutex timer_mutex_;
  std::chrono::time_point<std::chrono::system_clock> time_since_last_twist_msg_;

  rclcpp::TimerBase::SharedPtr timer_;
};

#endif  // IMU_CONTROLLER_HPP_
