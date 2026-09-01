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

#include "servo_node.hpp"

#include <chrono>
#include <memory>
#include <string>

#include "communications.hpp"
#include "pipebot_msgs/msg/servo.hpp"
#include "rclcpp/rclcpp.hpp"

// Constants
const double kEncoderTicksPerDegree = 4096.0 / 360.0;

// Default node name.
static const char* kNodeName = "servo_node";

ServoNode::ServoNode(const rclcpp::NodeOptions& options)
    : Node(kNodeName, options),
      instance_(kServoNone),
      turret_forward_degrees_(0),
      turret_backward_degrees_(0) {
  RCLCPP_INFO(get_logger(), "%s: Called", __func__);
  // Create the publisher using a mangled node name.
  std::string topic_name = get_name();
  // Replace _ with /
  size_t index = topic_name.find('_');
  topic_name[index] = '/';
  subscription_ = create_subscription<pipebot_msgs::msg::Servo>(
      topic_name, 1,
      std::bind(&ServoNode::Callback, this, std::placeholders::_1));
  // Set the instance value.
  bool found_turret = (topic_name.find("turret") != std::string::npos);
  if (found_turret) {
    instance_ = kServoTurret;
  }
}

void ServoNode::AddComms(std::shared_ptr<Communications> comms) {
  // The copy of the parameter adds 1 to the shared_ptr reference count.
  comms_ = comms;
  RCLCPP_INFO(get_logger(), "%s: added comms", __func__);
}

void ServoNode::SetTurretLimits(int32_t turret_forward_degrees,
                                int32_t turret_backward_degrees) {
  RCLCPP_INFO(get_logger(), "%s: Set servo limits to %d, %d", __func__,
              turret_forward_degrees, turret_backward_degrees);
  turret_forward_degrees_ = turret_forward_degrees;
  turret_backward_degrees_ = turret_backward_degrees;
}

void ServoNode::Callback(const pipebot_msgs::msg::Servo::SharedPtr msg) {
  // This implementation does not care about the real angle values, less than 90
  // degrees is forward else backward.
  double desired_angle_degrees = msg->angle_degrees;
  int32_t new_position_degrees = 0;
  if (desired_angle_degrees < 90.0) {
    new_position_degrees = turret_forward_degrees_;
  } else {
    new_position_degrees = turret_backward_degrees_;
  }
  // Set position.
  comms_->SetServoPosition(instance_, new_position_degrees);
  RCLCPP_INFO(get_logger(), "%s: desired angle %.1f, new angle %d", __func__,
              desired_angle_degrees, new_position_degrees);
}

// Necessary boiler plate code.

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(ServoNode)
