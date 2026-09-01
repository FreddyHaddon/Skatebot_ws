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

#ifndef SERVO_TURRET_NODE_HPP_
#define SERVO_TURRET_NODE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "pipebot_msgs/msg/servo.hpp"

/**
 * @brief Publish servo position messages.
 */
class ServoTurretNode : public rclcpp::Node
{
public:
  explicit ServoTurretNode(const rclcpp::NodeOptions & options);

  /**
   * @brief Send a message to set the position of the turret.
   *
   * @param degrees The new orientation, -360 to 360 degrees.
   */
  void SetPosition(int16_t degrees);

private:
  // The publisher.
  rclcpp::Publisher<pipebot_msgs::msg::Servo>::SharedPtr publisher_;
};

#endif  // SERVO_TURRET_NODE_HPP_
