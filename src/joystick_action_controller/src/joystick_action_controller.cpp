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

#include <memory>
#include <rclcpp/rclcpp.hpp>

#include "joystick_node.hpp"
#include "acoustic_action_client.hpp"
#include "servo_turret_node.hpp"
#include "ultrasonic_action_client.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  // Create nodes.
  rclcpp::NodeOptions options;
  options.arguments({""});
  auto joystick_node = std::make_shared<JoystickNode>(options);
  auto acoustic_action_client = std::make_shared<AcousticActionClient>(options);
  auto servo_turret_node = std::make_shared<ServoTurretNode>(options);
  auto ultrasonic_action_client = std::make_shared<UltrasonicActionClient>(options);

  // Connect joystick node to publisher and action servers.
  joystick_node->AddAcousticClient(acoustic_action_client);
  joystick_node->AddServoTurret(servo_turret_node);
  joystick_node->AddUltrasonicClient(ultrasonic_action_client);

  // Add nodes to executor.
  rclcpp::executors::SingleThreadedExecutor exec;
  exec.add_node(joystick_node);
  exec.add_node(acoustic_action_client);
  exec.add_node(servo_turret_node);
  exec.add_node(ultrasonic_action_client);

  // Spin until killed.
  exec.spin();

  // Tidy up.
  rclcpp::shutdown();
  return 0;
}
