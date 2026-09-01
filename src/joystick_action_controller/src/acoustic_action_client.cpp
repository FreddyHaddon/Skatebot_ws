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

#include "acoustic_action_client.hpp"

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "pipebot_msgs/action/acoustic_sensing.hpp"


static const char * kNodeName = "acoustic_client_node";
static const char * kActionName = "action_acoustic_sensing";
static const std::chrono::milliseconds kWaitDelayMs(250);

AcousticActionClient::AcousticActionClient(const rclcpp::NodeOptions & options)
: Node(kNodeName, options), action_running_(false)
{
  client_ = rclcpp_action::create_client<AcousticSensing>(this, kActionName);
  RCLCPP_INFO(get_logger(), "%s: Started", __func__);
}

void AcousticActionClient::Start()
{
  using namespace std::placeholders;
  if (!action_running_) {
    action_running_ = true;
    if (!client_->wait_for_action_server(kWaitDelayMs)) {
      RCLCPP_ERROR(get_logger(), "Action server not available after waiting");
      action_running_ = false;
    } else {
      RCLCPP_INFO(get_logger(), "Sending goal");
      // Setup callbacks.
      auto send_goal_options = rclcpp_action::Client<AcousticSensing>::SendGoalOptions();
      send_goal_options.goal_response_callback =
        std::bind(&AcousticActionClient::GoalResposeCallback, this, _1);
      send_goal_options.feedback_callback =
        std::bind(&AcousticActionClient::FeedbackCallback, this, _1, _2);
      send_goal_options.result_callback =
        std::bind(&AcousticActionClient::ResultCallback, this, _1);
      // Send the goal request.
      auto goal_msg = AcousticSensing::Goal();
      goal_msg.enable = true;
      client_->async_send_goal(goal_msg, send_goal_options);
    }
  } else {
    RCLCPP_INFO(get_logger(), "%s: Acoustic action already running", __func__);
  }
}

void AcousticActionClient::GoalResposeCallback(
  const GoalHandleAcousticSensing::SharedPtr & goal_handle)
{
  if (goal_handle) {
    RCLCPP_INFO(get_logger(), "Goal accepted by server, waiting for result");
  } else {
    RCLCPP_ERROR(get_logger(), "Goal was rejected by server");
  }
}

void AcousticActionClient::FeedbackCallback(
  rclcpp_action::ClientGoalHandle<AcousticSensing>::SharedPtr,
  const std::shared_ptr<const AcousticSensing::Feedback> feedback)
{
  RCLCPP_INFO(get_logger(), "Feedback %f%%", feedback->state);
}

void AcousticActionClient::ResultCallback(
  const rclcpp_action::ClientGoalHandle<AcousticSensing>::WrappedResult & result)
{
  switch (result.code) {
    case rclcpp_action::ResultCode::SUCCEEDED:
      RCLCPP_INFO(get_logger(), "Success!");
      break;
    case rclcpp_action::ResultCode::ABORTED:
      RCLCPP_ERROR(get_logger(), "Goal was aborted");
      return;
    case rclcpp_action::ResultCode::CANCELED:
      RCLCPP_ERROR(get_logger(), "Goal was cancelled");
      return;
    default:
      RCLCPP_ERROR(get_logger(), "Unknown result code");
      return;
  }
  action_running_ = false;
}

// Necessary boiler plate code.

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(AcousticActionClient)
