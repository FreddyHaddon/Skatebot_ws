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

#ifndef RPI_MONITOR_NODE_HPP_
#define RPI_MONITOR_NODE_HPP_

#include <map>
#include "diagnostic_updater/diagnostic_updater.hpp"
#include "diagnostic_updater/publisher.hpp"
#include "rclcpp/rclcpp.hpp"

/**
 * @brief Publish diagnostic messages.
 */
class RPiMonitorNode : public rclcpp::Node
{
public:
  RPiMonitorNode();
  ~RPiMonitorNode();

private:
  // Using struct so more temperatures can be added later.
  struct CPUTemperatureData
  {
    // True if values read from system OK.
    bool values_read_;
    // Temperature of CPU in Celsius (_c).
    double cpu_temperature_c_;
    // Constructor to initialise values.
    CPUTemperatureData()
    : values_read_(false), cpu_temperature_c_(0.0) {}
  };

  struct UsageData
  {
    // True if values read from system OK.
    bool values_read_;
    // Usage values in percent.  Can add detail here if needed.
    double total_usage_;
    // Constructor to initialise values.
    UsageData()
    : values_read_(false), total_usage_(0.0) {}
  };

  void CheckCPUTemperature(diagnostic_updater::DiagnosticStatusWrapper & stat);
  CPUTemperatureData ReadCPUTemperatureData();
  void CheckCPUUsage(diagnostic_updater::DiagnosticStatusWrapper & stat);
  UsageData ReadCPUUsageData();
  uint64_t ReadIdleJiffies();
  void CheckMemoryUsage(diagnostic_updater::DiagnosticStatusWrapper & stat);
  UsageData ReadMemoryUsageData();

  diagnostic_updater::Updater * updater_;
  uint64_t last_idle_jiffies_;
  rclcpp::Time last_idle_called_;

  /// @brief CPU usage status messages
  const std::map<int, const char *> load_dict_ =
  {
    {diagnostic_msgs::msg::DiagnosticStatus::OK, "OK"},
    {diagnostic_msgs::msg::DiagnosticStatus::WARN, "high load"},
    {diagnostic_msgs::msg::DiagnosticStatus::ERROR, "very high load"}
  };
  /// @brief CPU temperature status messages
  const std::map<int, const char *> temp_dict_ =
  {
    {diagnostic_msgs::msg::DiagnosticStatus::OK, "OK"},
    {diagnostic_msgs::msg::DiagnosticStatus::WARN, "warm"},
    {diagnostic_msgs::msg::DiagnosticStatus::ERROR, "hot"}
  };
};

#endif  // RPI_MONITOR_NODE_HPP_
