// Copyright 2023 University of Leeds.
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

#ifndef JOYSTICK_NODE_HPP_
#define JOYSTICK_NODE_HPP_

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "acoustic_action_client.hpp"
#include "servo_turret_node.hpp"
#include "ultrasonic_action_client.hpp"

/**
 * @brief Subscribe to ROS messages.
 */
class JoystickNode : public rclcpp::Node
{
public:
  explicit JoystickNode(const rclcpp::NodeOptions & options);

  void AddAcousticClient(std::shared_ptr<AcousticActionClient> client);
  void AddServoTurret(std::shared_ptr<ServoTurretNode> servo_turret);
  void AddUltrasonicClient(std::shared_ptr<UltrasonicActionClient> client);

private:
  enum TurretPosition
  {
    kTurretUnknown,
    kTurretForward,
    kTurretSide1,
    kTurretBackward,
    kTurretSide2
  };
  void Callback(const sensor_msgs::msg::Joy::SharedPtr msg);
  void RotateTurret(int16_t new_angle);
  void StartAcousticAction();
  void StartUltrasonicAction();
  // The subscriber instance.
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription_;
  // Button mappings.
  int button_rotate_turret_forward_;
  int button_rotate_turret_backward_;
  int button_start_acoustic_;
  int button_start_ultrasonic_;
  // Publishers and clients.
  std::shared_ptr<AcousticActionClient> acoustic_client_;
  std::shared_ptr<ServoTurretNode> servo_turret_;
  std::shared_ptr<UltrasonicActionClient> ultrasonic_client_;
  // Variables.
  TurretPosition last_turret_position_;
};

#endif  // JOYSTICK_NODE_HPP_
