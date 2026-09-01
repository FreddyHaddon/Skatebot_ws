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

#ifndef ULTRASONIC_ACTION_CLIENT_HPP_
#define ULTRASONIC_ACTION_CLIENT_HPP_

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "pipebot_msgs/action/scan_ultra.hpp"

/**
 * @brief Start ultrasonic action.
 */
class UltrasonicActionClient : public rclcpp::Node
{
public:
  using ScanUltra = pipebot_msgs::action::ScanUltra;
  using GoalHandleScanUltra = rclcpp_action::ClientGoalHandle<ScanUltra>;

  explicit UltrasonicActionClient(const rclcpp::NodeOptions & options);

  /**
   * @brief Start the action.
   * @note The result of the action is published on a different topic so we
   * don't need the result.
   */
  void Start();

private:
  void GoalResposeCallback(const GoalHandleScanUltra::SharedPtr & goal_handle);
  void FeedbackCallback(
    GoalHandleScanUltra::SharedPtr,
    const std::shared_ptr<const ScanUltra::Feedback> feedback);
  void ResultCallback(
    const rclcpp_action::ClientGoalHandle<ScanUltra>::WrappedResult & result);

  // The client.
  rclcpp_action::Client<ScanUltra>::SharedPtr client_;
  // Action running or not.
  bool action_running_;
};

#endif  // ULTRASONIC_ACTION_CLIENT_HPP_
