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

#include "servo_turret_node.hpp"

#include <chrono>
#include <memory>
#include <string>

#include "pipebot_msgs/msg/servo.hpp"
#include "rclcpp/rclcpp.hpp"

// Topic name.
static const char * kTopicName = "servo/turret";
// Node name.
static const char * kNodeName = "servo_publisher_node";
// Delay between calls.
static const std::chrono::milliseconds kWaitDelayMs(1000);


ServoTurretNode::ServoTurretNode(const rclcpp::NodeOptions & options)
: Node(kNodeName, options)
{
  RCLCPP_INFO(get_logger(), "%s: Called", __func__);
  publisher_ = create_publisher<pipebot_msgs::msg::Servo>(kTopicName, 10);
}

void ServoTurretNode::SetPosition(int16_t degrees)
{
  // Allow messages to be sent at a maximum rate.
  static std::chrono::steady_clock::time_point last_called;
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  if (now - last_called > kWaitDelayMs) {
    last_called = now;
    // Add the objects to the message.
    pipebot_msgs::msg::Servo message;
    message.angle_degrees = degrees;
    RCLCPP_INFO(get_logger(), "%s: degrees %d", __func__, message.angle_degrees);
    // Publish the message.
    publisher_->publish(message);
  }
}

// Necessary boiler plate code.

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(ServoTurretNode)
