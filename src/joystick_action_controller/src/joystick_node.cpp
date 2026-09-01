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

#include "joystick_node.hpp"

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "acoustic_action_client.hpp"
#include "servo_turret_node.hpp"
#include "ultrasonic_action_client.hpp"

// Node name.
static const char * kNodeName = "joystick_node";
// Topic name.
static const char * kTopicName = "joy";
// Turret position values in degrees.
static const int16_t kTurretAngleForward = 0;
// static const uint16_t kTurretAngleSide = kTurretAngleForward + 90;
static const int16_t kTurretAngleBackward = 180;

JoystickNode::JoystickNode(const rclcpp::NodeOptions & options)
: Node(kNodeName, options), last_turret_position_(kTurretUnknown)
{
  // Get the button mappings.
  // Defaults are for Logitech controller.
  declare_parameter("button_rotate_turret_forward", 3);
  button_rotate_turret_backward_ = get_parameter("button_rotate_turret_forward")
    .get_parameter_value()
    .get<int>();
  declare_parameter("button_rotate_turret_backward", 2);
  button_rotate_turret_forward_ = get_parameter("button_rotate_turret_backward")
    .get_parameter_value()
    .get<int>();
  declare_parameter("button_start_acoustic", 0);
  button_start_acoustic_ = get_parameter("button_start_acoustic")
    .get_parameter_value()
    .get<int>();
  declare_parameter("button_start_ultrasonic", 1);
  button_start_ultrasonic_ = get_parameter("button_start_ultrasonic")
    .get_parameter_value()
    .get<int>();
  RCLCPP_INFO(
    get_logger(), "%s: Using buttons: rf %d, rb %d, a %d, u %d", __func__,
    button_rotate_turret_forward_, button_rotate_turret_backward_, button_start_acoustic_,
    button_start_ultrasonic_);
  // Create the subscription.
  subscription_ = create_subscription<sensor_msgs::msg::Joy>(
    kTopicName, 10,
    std::bind(&JoystickNode::Callback, this, std::placeholders::_1));
  RCLCPP_INFO(get_logger(), "%s: Started", __func__);
}

void JoystickNode::AddAcousticClient(std::shared_ptr<AcousticActionClient> client)
{
  acoustic_client_ = client;
  RCLCPP_INFO(get_logger(), "%s: Added acoustic client", __func__);
}

void JoystickNode::AddServoTurret(std::shared_ptr<ServoTurretNode> servo_turret)
{
  // The copy of the parameter adds 1 to the shared_ptr reference count.
  servo_turret_ = servo_turret;
  RCLCPP_INFO(get_logger(), "%s: Added servo turret node", __func__);
}

void JoystickNode::AddUltrasonicClient(std::shared_ptr<UltrasonicActionClient> client)
{
  ultrasonic_client_ = client;
  RCLCPP_INFO(get_logger(), "%s: Added ultrasonic client", __func__);
}

void JoystickNode::Callback(const sensor_msgs::msg::Joy::SharedPtr msg)
{
  if (msg->buttons[button_rotate_turret_forward_]) {
    RotateTurret(kTurretAngleForward);
  }
  if (msg->buttons[button_rotate_turret_backward_]) {
    RotateTurret(kTurretAngleBackward);
  }
  if (msg->buttons[button_start_acoustic_]) {
    StartAcousticAction();
  }
  if (msg->buttons[button_start_ultrasonic_]) {
    StartUltrasonicAction();
  }
}

void JoystickNode::RotateTurret(int16_t new_angle)
{
  RCLCPP_INFO(get_logger(), "%s: new angle %d", __func__, new_angle);
  servo_turret_->SetPosition(new_angle);
}

void JoystickNode::StartAcousticAction()
{
  RCLCPP_INFO(get_logger(), "%s: Start acoustic pressed", __func__);
  acoustic_client_->Start();
}

void JoystickNode::StartUltrasonicAction()
{
  RCLCPP_INFO(get_logger(), "%s: Start ultrasonic pressed", __func__);
  // FIXME: This blocks.  Run on thread?
  ultrasonic_client_->Start();
}

// Necessary boiler plate code.

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(JoystickNode)
