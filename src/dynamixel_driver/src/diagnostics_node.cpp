// Copyright 2022 University of Leeds.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "diagnostics_node.hpp"
#include <chrono>
#include <memory>
#include <string>
#include "rclcpp/rclcpp.hpp"
#include "diagnostic_msgs/msg/diagnostic_array.hpp"
#include "communications.hpp"


// Interval between each message, 1 second is default rate.
static const std::chrono::milliseconds kQueryIntervalMs(1000);

// Default node name and topic name.
static const char * kNodeName = "diagnostics_node";
static const char * kTopicName = "diagnostics";

DiagnosticsNode::DiagnosticsNode(const rclcpp::NodeOptions & options)
: Node(kNodeName, options)
{
  RCLCPP_INFO(get_logger(), "%s: Called", __func__);
  publisher_ = create_publisher<diagnostic_msgs::msg::DiagnosticArray>(kTopicName, 10);
  // Connect TimerCallback() to a timer to publish the message every kQueryIntervalMs.
  timer_ = create_wall_timer(kQueryIntervalMs, std::bind(&DiagnosticsNode::TimerCallback, this));
}

void DiagnosticsNode::AddComms(std::shared_ptr<Communications> comms)
{
  // The copy of the parameter adds 1 to the shared_ptr reference count.
  comms_ = comms;
  RCLCPP_INFO(get_logger(), "%s: added comms", __func__);
}

void DiagnosticsNode::TimerCallback()
{
  // Set header.
  diagnostic_msgs::msg::DiagnosticArray message;
  message.header.frame_id = "frame id";
  message.header.stamp = now();
  // Add the objects to the message.
  AddConnectedMessage(&message);
  AddServoStatusMessage(&message);
  // Publish the message.
  publisher_->publish(message);
}

void DiagnosticsNode::AddConnectedMessage(diagnostic_msgs::msg::DiagnosticArray * message)
{
  // Get the connected status.
  bool connected = comms_->Connected();
  // Fill out new status item.
  diagnostic_msgs::msg::DiagnosticStatus connected_item;
  connected_item.hardware_id = "Dynamixel Serial Link";
  connected_item.name = "Connected";
  if (connected) {
    connected_item.level = 0;
    connected_item.message = "OK";
  } else {
    // Report this as an error because the robot won't move if disconnected.
    connected_item.level = 2;
    connected_item.message = "ERROR";
  }
  RCLCPP_DEBUG(get_logger(), "%s: connected status %d", __func__, static_cast<int>(connected));
  // Don't need this so leave for now.
  // connected_item.values = ???;
  // Add item to array.
  message->status.push_back(connected_item);
}

void DiagnosticsNode::AddServoStatusMessage(diagnostic_msgs::msg::DiagnosticArray * message)
{
  // Get the hardware status of all servos.
  ServoStatusVector status = comms_->GetHardwareStatus();
  // Fill out status item for each servo.
  for (ServoStatus servo_status : status) {
    diagnostic_msgs::msg::DiagnosticStatus status_item;
    status_item.name = "Servo: " + servo_status.instance_name_;
    status_item.hardware_id = "Dynamixel";
    status_item.message = HardwareStatusToString(servo_status.hardware_status_);
    if (servo_status.hardware_status_ == 0) {
      // OK
      status_item.level = status_item.OK;
    } else {
      // Error
      status_item.level = status_item.ERROR;
      // Output warning message.
      RCLCPP_WARN(get_logger(),"%s: ERROR: Id %s, %s",
                  __func__, servo_status.instance_name_.c_str(), status_item.message.c_str());
      // Reboot only when overloaded.
      if (servo_status.hardware_status_ & 0x20) {
        comms_->Reboot(servo_status.instance_);
      }
    }
    // Add temperature.
    diagnostic_msgs::msg::KeyValue key_value;
    key_value.key = "Temperature C";
    key_value.value = std::to_string(servo_status.temperature_c_);
    status_item.values.push_back(key_value);
    // Add input voltage.
    key_value.key = "Input voltage";
    key_value.value = std::to_string(servo_status.input_voltage_);
    status_item.values.push_back(key_value);
    // Add item to message.
    message->status.push_back(status_item);
  }
}

std::string DiagnosticsNode::HardwareStatusToString(uint8_t status)
{
  // Status meanings from here:
  // https://emanual.robotis.com/docs/en/dxl/mx/mx-28-2/#hardware-error-status70
  std::string result;
  if (status == 0) {
    result += "OK";
  } else {
    if (status & 0x01) {
      result += "Over voltage,";
    }
    if (status & 0x04) {
      result += "Overheated,";
    }
    if (status & 0x08) {
      result += "Encoder failed,";
    }
    if (status & 0x10) {
      result += "Insufficent voltage or short circuit,";
    }
    if (status & 0x20) {
      result += "Overloaded,";
    }
  }
  return result;
}


// Necessary boiler plate code.

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(DiagnosticsNode)
