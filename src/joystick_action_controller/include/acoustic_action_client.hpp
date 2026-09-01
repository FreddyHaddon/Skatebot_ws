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

#ifndef ACOUSTIC_ACTION_CLIENT_HPP_
#define ACOUSTIC_ACTION_CLIENT_HPP_

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "pipebot_msgs/action/acoustic_sensing.hpp"

/**
 * @brief Start ultrasonic action.
 */
class AcousticActionClient : public rclcpp::Node
{
public:
  using AcousticSensing = pipebot_msgs::action::AcousticSensing;
  using GoalHandleAcousticSensing = rclcpp_action::ClientGoalHandle<AcousticSensing>;

  explicit AcousticActionClient(const rclcpp::NodeOptions & options);

  /**
   * @brief Start the action.
   * @note The result of the action is published on a different topic so we
   * don't use the result.
   */
  void Start();

private:
  void GoalResposeCallback(const GoalHandleAcousticSensing::SharedPtr & goal_handle);
  void FeedbackCallback(
    GoalHandleAcousticSensing::SharedPtr,
    const std::shared_ptr<const AcousticSensing::Feedback> feedback);
  void ResultCallback(
    const rclcpp_action::ClientGoalHandle<AcousticSensing>::WrappedResult & result);

  // The client.
  rclcpp_action::Client<AcousticSensing>::SharedPtr client_;
  // Running or not.
  bool action_running_;
};

#endif  // ACOUSTIC_ACTION_CLIENT_HPP_
