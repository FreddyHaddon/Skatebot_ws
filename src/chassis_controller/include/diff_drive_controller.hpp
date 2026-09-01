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

#ifndef DIFF_DRIVE_CONTROLLER_HPP_
#define DIFF_DRIVE_CONTROLLER_HPP_

#include <atomic>
#include <shared_mutex>

#include "geometry_msgs/msg/twist.hpp"
#include "pipebot_msgs/msg/multiple_wheel_drive.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/string.hpp"
#include "urdf/model.h"

class DiffDriveController : public rclcpp::Node
{
public:
  DiffDriveController();

private:
  void CommandCallback(const geometry_msgs::msg::Twist::SharedPtr msg);
  void DescriptionCallback(const std_msgs::msg::String::SharedPtr msg);
  double GetWheelDistance();
  double GetWheelRadius();
  void PublishAllMotorsMessage(
    double front_left_rad_s, double front_right_rad_s, double rear_left_rad_s,
    double rear_right_rad_s);
  void PublishSimulationMessage(
    double front_left_rad_s, double front_right_rad_s, double rear_left_rad_s,
    double rear_right_rad_s);

  // Direction commands.
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr vel_subscriber_;
  // To get the robot description.
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr robot_description_subscriber_;
  // TODO(AJB): Can these two publishers be rationalised into one?
  // For simulation only.
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr simulation_motors_publisher_;
  // For SkateBot.
  rclcpp::Publisher<pipebot_msgs::msg::MultipleWheelDrive>::SharedPtr all_motors_publisher_;

  urdf::Model model_;
  std::atomic<bool> model_loaded_{false};
  std::shared_mutex model_mutex_;
};

#endif  // DIFF_DRIVE_CONTROLLER_HPP_
