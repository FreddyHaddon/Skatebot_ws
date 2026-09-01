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

#include "robot_pose_publisher.hpp"

#include <cmath>

#include "tf2/LinearMath/Matrix3x3.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

RobotPosePublisher::RobotPosePublisher()
: Node("robot_pose_publisher")
{
  imu_subscriber_ = this->create_subscription<sensor_msgs::msg::Imu>(
    "/imu/imu", 10, std::bind(&RobotPosePublisher::imu_callback_, this, std::placeholders::_1));

  tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
}

void RobotPosePublisher::imu_callback_(const sensor_msgs::msg::Imu::SharedPtr msg)
{
  // RCLCPP_INFO(get_logger(), "%s: Called", __func__);
  geometry_msgs::msg::TransformStamped t;

  // Read message content and assign it to
  // corresponding tf variables
  t.header.stamp = this->get_clock()->now();
  t.header.stamp.sec += 1;
  t.header.frame_id = "map";
  t.child_frame_id = "base_footprint";

  t.transform.translation.x = 0;
  t.transform.translation.y = 0;
  t.transform.translation.z = 0.0;

  // For the same reason, turtle can only rotate around one axis
  // and this why we set rotation in x and y to 0 and obtain
  // rotation in z axis from the message

  auto q = msg->orientation;

  if (!isnan(q.x) && !isnan(q.y) && !isnan(q.z) && !isnan(q.w)) {
    t.transform.rotation.x = q.x;
    t.transform.rotation.y = q.y;
    t.transform.rotation.z = q.z;
    t.transform.rotation.w = q.w;

    // Send the transformation
    tf_broadcaster_->sendTransform(t);
  }
}
