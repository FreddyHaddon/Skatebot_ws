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

#include "imu_euler.hpp"

#include "tf2/LinearMath/Matrix3x3.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

IMU_Euler::IMU_Euler()
: Node("imu_euler")
{
  imu_subscriber_ = this->create_subscription<sensor_msgs::msg::Imu>(
    "imu/data", 10, std::bind(&IMU_Euler::imu_callback_, this, std::placeholders::_1));
  angle_publisher_ = this->create_publisher<pipebot_msgs::msg::EulerAngles>("imu/euler", 10);
}

double IMU_Euler::rad2deg_(double angle) {return angle / M_PI * 180;}

void IMU_Euler::imu_callback_(const sensor_msgs::msg::Imu::SharedPtr msg)
{
  tf2Scalar roll;
  tf2Scalar pitch;
  tf2Scalar yaw;

  tf2::Quaternion tf_quaternion;
  tf2::fromMsg(msg->orientation, tf_quaternion);

  tf2::Matrix3x3 rotation_matrix = tf2::Matrix3x3(tf_quaternion);
  rotation_matrix.getRPY(roll, pitch, yaw, 1);

  double roll_in_deg = rad2deg_(roll);
  double pitch_in_deg = rad2deg_(pitch);
  double yaw_in_deg = rad2deg_(yaw);

  pipebot_msgs::msg::EulerAngles angles;
  angles.roll = roll_in_deg;
  angles.pitch = pitch_in_deg;
  angles.yaw = yaw_in_deg;
  angle_publisher_->publish(angles);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<IMU_Euler>());
  rclcpp::shutdown();
  return 0;
}
