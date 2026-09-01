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

#include "dynamixel_comms.hpp"

#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include "dynamixel_sdk/dynamixel_sdk.h"
#include "rclcpp/rclcpp.hpp"

// Dynamixel SDK library.
// Exit codes.
#define EXIT_PORT_FAILURE (-2)
#define EXIT_THREAD_FAILED (-3)

// Dynamixel protocol version.
// The MX-xxAT dynamixels need to be updated to use this protocol version.
#define PROTOCOL_VERSION (2.0)

// Default Baudrate of DYNAMIXEL X series
// #define BAUD_RATE (57600)
// Use 1Mbs
#define BAUD_RATE (1000 * 1000)

// Defines for protocol 2.0
#define ADDR_PRO_GOAL_VELOCITY (104)
#define ADDR_PRO_GOAL_POSITION (116)
#define ADDR_PRO_PRESENT_POSITION (132)

#define LEN_PRO_GOAL_POSITION (4)
#define LEN_PRO_GOAL_VELOCITY (4)
#define LEN_PRO_PRESENT_POSITION (4)

// The timeout for the deadman's switch.  I would like it shorter than 500ms
// but every now and then, messages get delayed and there can be lag of up
// to 400ms, so 500ms works pretty well.
static const std::chrono::milliseconds kDeadmansSwitchTimeoutMs(500);

// Logging name.
static const char* kLogName = "dynamixel_driver.comms";

// Allocate Dynamixel servos to Ids.
static const int kTurretId = 0;
// Wheel servos are arranged as follows:
//        Left Right
//  Front  2    1
//  Rear   3    4
// Right servos CW = forwards, left servos CCW = forwards.
static const int kLeftFrontId = 2;
static const int kLeftRearId = 3;
static const int kRightFrontId = 1;
static const int kRightRearId = 4;

// Dynamixel servo model number for MX-28AT(2.0).
static const char* kModelMX28AT = "MX-28AT";
static const uint32_t kModelMX28ATNumber = 0x001E;
// Dynamixel servo model number for MX-64AT(2.0).
static const char* kModelMX64AT = "MX-64AT";
static const uint32_t kModelMX64ATNumber = 0x0137;

// Static variables for communicating with all Dynamixels.
dynamixel::PortHandler* DynamixelComms::port_handler_ = 0;
dynamixel::PacketHandler* DynamixelComms::packet_handler_ = 0;
dynamixel::GroupBulkRead* DynamixelComms::group_bulk_read_ = 0;
dynamixel::GroupBulkWrite* DynamixelComms::group_bulk_write_ = 0;

DynamixelComms::DynamixelComms() : connected_(false) {
  // Prevent more than one instance accessing the ports.
  if (port_handler_ != 0) {
    RCUTILS_LOG_FATAL_NAMED(kLogName, "port_handler_ already in use.");
    std::exit(EXIT_PORT_FAILURE);
  }
  if (packet_handler_ != 0) {
    RCUTILS_LOG_FATAL_NAMED(kLogName, "packet_handler_ already in use.");
    std::exit(EXIT_PORT_FAILURE);
  }
  // Start deadman's switch thread.
  try {
    deadmans_switch_thread_ =
        std::thread(&DynamixelComms::DeadmansSwitchLoop, this);
  } catch (const std::exception& e) {
    RCUTILS_LOG_FATAL_NAMED(kLogName, "Failed to start thread: %s", e.what());
    std::exit(EXIT_THREAD_FAILED);
  }
}

DynamixelComms::~DynamixelComms() {
  // Ensure the timer thread is joined before exiting the program
  deadmans_switch_thread_.join();
}

void DynamixelComms::Init(const std::string& device_name,
                          const std::string& model_name) {
  std::scoped_lock lock(comms_mutex_);
  // Setup port and packet handlers.
  // No need to delete these we don't own then.
  port_handler_ = dynamixel::PortHandler::getPortHandler(device_name.c_str());
  packet_handler_ =
      dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
  // Open Serial Port
  int dxl_comm_result = port_handler_->openPort();
  if (!dxl_comm_result) {
    RCUTILS_LOG_FATAL_NAMED(kLogName, "Failed to open port %s",
                            device_name.c_str());
    std::exit(EXIT_PORT_FAILURE);
  } else {
    RCUTILS_LOG_INFO_NAMED(kLogName, "Opened port %s.", device_name.c_str());
    // Set the Baud rate of the serial port (use DYNAMIXEL Baud rate)
    dxl_comm_result = port_handler_->setBaudRate(BAUD_RATE);
    if (!dxl_comm_result) {
      RCUTILS_LOG_ERROR_NAMED(kLogName, "Failed to set the Baud rate to %d!",
                              BAUD_RATE);
    } else {
      RCUTILS_LOG_INFO_NAMED(kLogName, "Baud rate set to %d.", BAUD_RATE);
      connected_ = true;
      // Initialise bulk mode.
      group_bulk_read_ =
          new dynamixel::GroupBulkRead(port_handler_, packet_handler_);
      group_bulk_write_ =
          new dynamixel::GroupBulkWrite(port_handler_, packet_handler_);
      // Set servo model number.
      uint32_t model_number = 0;
      if (model_name == kModelMX28AT) {
        model_number = kModelMX28ATNumber;
      } else if (model_name == kModelMX64AT) {
        model_number = kModelMX64ATNumber;
      } else {
        RCUTILS_LOG_ERROR_NAMED(kLogName, "Unknown Dynamixel model name %s",
                                model_name.c_str());
        std::exit(EXIT_FAILURE);
      }
      // Set up each servo.
      servo_turret_.Init(model_number, kTurretId, DynamixelServo::kConfigServo,
                         port_handler_, packet_handler_);
      // This is where the motor directions are set.
      // To move forward, the left motors rotate anti-clockwise and the right
      // motors rotate clockwise.
      motor_front_left_.Init(model_number, kLeftFrontId,
                             DynamixelServo::kConfigMotorAntiClockwise,
                             port_handler_, packet_handler_);
      motor_rear_left_.Init(model_number, kLeftRearId,
                            DynamixelServo::kConfigMotorAntiClockwise,
                            port_handler_, packet_handler_);
      motor_front_right_.Init(model_number, kRightFrontId,
                              DynamixelServo::kConfigMotorClockwise,
                              port_handler_, packet_handler_);
      motor_rear_right_.Init(model_number, kRightRearId,
                             DynamixelServo::kConfigMotorClockwise,
                             port_handler_, packet_handler_);
    }
  }
}

bool DynamixelComms::Connected() { return connected_; }

ServoStatusVector DynamixelComms::GetHardwareStatus() {
  std::scoped_lock lock(comms_mutex_);
  ServoStatusVector result;
  DynamixelServo::DynamixelServoStatus dynamixel_status;
  ServoStatus servo_status;
  // FIXME(AJB) Fragile code if number of servos change.
  for (int i = 0; i < 5; ++i) {
    switch (i) {
      case 0:
        dynamixel_status = servo_turret_.GetStatus();
        servo_status = DynamixelToServoStatus(dynamixel_status);
        result.push_back(servo_status);
        break;
      case 1:
        dynamixel_status = servo_turret_.GetStatus();
        servo_status = DynamixelToServoStatus(dynamixel_status);
        result.push_back(servo_status);
        break;
      case 2:
        dynamixel_status = servo_turret_.GetStatus();
        servo_status = DynamixelToServoStatus(dynamixel_status);
        result.push_back(servo_status);
        break;
      case 3:
        dynamixel_status = servo_turret_.GetStatus();
        servo_status = DynamixelToServoStatus(dynamixel_status);
        result.push_back(servo_status);
        break;
      case 4:
        dynamixel_status = servo_turret_.GetStatus();
        servo_status = DynamixelToServoStatus(dynamixel_status);
        result.push_back(servo_status);
        break;
    }
  }
  return result;
}

int64_t DynamixelComms::GetTotalEncoderCount(const MotorInstance motor) {
  int64_t result = 0;
  if (motor == kMotorLeft) {
    std::scoped_lock lock(comms_mutex_);
    result = motor_front_left_.GetTotalEncoderCount();
  }
  if (motor == kMotorRight) {
    std::scoped_lock lock(comms_mutex_);
    result = motor_front_right_.GetTotalEncoderCount();
  }
  return result;
}

double DynamixelComms::GetVoltage(const ServoInstance /*servo*/) {
  std::scoped_lock lock(comms_mutex_);
  double voltage = 0.0;
  // For this robot, all servos have the same voltage so just read the
  // front left.
  voltage = motor_front_left_.GetVoltage();
  return voltage;
}

void DynamixelComms::SetAllRPM(double front_left_rpm, double front_right_rpm,
                               double rear_left_rpm, double rear_right_rpm) {
  RCUTILS_LOG_INFO_NAMED(kLogName, "%s: called", __func__);
  std::scoped_lock lock(comms_mutex_);
  // Set mode of all servos to velocity control.
  motor_front_left_.ConfigureControlMode(DynamixelServo::kVelocityControlMode);
  motor_front_right_.ConfigureControlMode(DynamixelServo::kVelocityControlMode);
  motor_rear_left_.ConfigureControlMode(DynamixelServo::kVelocityControlMode);
  motor_rear_right_.ConfigureControlMode(DynamixelServo::kVelocityControlMode);
  // Invert direction of right motors.
  front_right_rpm = -front_right_rpm;
  rear_right_rpm = -rear_right_rpm;
  // Use bulk mode to set all motors at once.
  uint8_t param_goal_velocity[4];
  ConvertRPMToGoalVelocity(front_left_rpm, param_goal_velocity);
  group_bulk_write_->addParam(motor_front_left_.GetId(), ADDR_PRO_GOAL_VELOCITY,
                              LEN_PRO_GOAL_VELOCITY, param_goal_velocity);
  ConvertRPMToGoalVelocity(front_right_rpm, param_goal_velocity);
  group_bulk_write_->addParam(motor_front_right_.GetId(), ADDR_PRO_GOAL_VELOCITY,
                              LEN_PRO_GOAL_VELOCITY, param_goal_velocity);
  ConvertRPMToGoalVelocity(rear_left_rpm, param_goal_velocity);
  group_bulk_write_->addParam(motor_rear_left_.GetId(), ADDR_PRO_GOAL_VELOCITY,
                              LEN_PRO_GOAL_VELOCITY, param_goal_velocity);
  ConvertRPMToGoalVelocity(rear_right_rpm, param_goal_velocity);
  group_bulk_write_->addParam(motor_rear_right_.GetId(), ADDR_PRO_GOAL_VELOCITY,
                              LEN_PRO_GOAL_VELOCITY, param_goal_velocity);
  group_bulk_write_->txPacket();
  group_bulk_write_->clearParam();
  DeadmansSwitchEnable(true);
  DeadmansSwitchReset();
}

void DynamixelComms::SetRPM(const MotorInstance motor, double rpm) {
  if (motor == kMotorLeft) {
    std::scoped_lock lock(comms_mutex_);
    motor_front_left_.SetRPM(rpm);
    motor_rear_left_.SetRPM(rpm);
  }
  if (motor == kMotorRight) {
    std::scoped_lock lock(comms_mutex_);
    motor_front_right_.SetRPM(-rpm);
    motor_rear_right_.SetRPM(-rpm);
  }
  DeadmansSwitchEnable(true);
  DeadmansSwitchReset();
}

void DynamixelComms::SetRelativePosition(const MotorInstance motor,
                                         double angle_radians) {
  DeadmansSwitchEnable(false);
  if (motor == kMotorLeft) {
    std::scoped_lock lock(comms_mutex_);
    motor_front_left_.SetRelativePosition(angle_radians);
    motor_rear_left_.SetRelativePosition(angle_radians);
  }
  if (motor == kMotorRight) {
    std::scoped_lock lock(comms_mutex_);
    motor_front_right_.SetRelativePosition(angle_radians);
    motor_rear_right_.SetRelativePosition(angle_radians);
  }
}

void DynamixelComms::SetAllRelativePosition(double front_left_angle_radians,
                                            double front_right_angle_radians,
                                            double rear_left_angle_radians,
                                            double rear_right_angle_radians) {
  DeadmansSwitchEnable(false);
  std::scoped_lock lock(comms_mutex_);
  // Set servos into extended position control mode.
  motor_front_left_.ConfigureControlMode(
      DynamixelServo::kExtendedPositionControlMode);
  motor_front_right_.ConfigureControlMode(
      DynamixelServo::kExtendedPositionControlMode);
  motor_rear_left_.ConfigureControlMode(
      DynamixelServo::kExtendedPositionControlMode);
  motor_rear_right_.ConfigureControlMode(
      DynamixelServo::kExtendedPositionControlMode);
  // Get present position of all servos.
#if 0
  int32_t front_left_position = 0;
  int32_t front_right_position = 0;
  int32_t rear_left_position = 0;
  int32_t rear_right_position = 0;
  GetCurrentPositions(&front_left_position, &front_right_position,
                      &rear_left_position, &rear_right_position);
#else
  // Can't get the bulk mode read to work so just read each servo individually.
  int32_t front_left_position = motor_front_left_.GetPresentPosition();
  int32_t front_right_position = motor_front_right_.GetPresentPosition();
  int32_t rear_left_position = motor_rear_left_.GetPresentPosition();
  int32_t rear_right_position = motor_rear_right_.GetPresentPosition();
#endif
  // Use bulk mode to set all motors at once.
  uint8_t param_goal_position[4];
  CalculateNewGoalPosition(front_left_angle_radians, front_left_position,
                           param_goal_position);
  group_bulk_write_->addParam(motor_front_left_.GetId(), ADDR_PRO_GOAL_POSITION,
                              LEN_PRO_GOAL_POSITION, param_goal_position);
  CalculateNewGoalPosition(front_right_angle_radians, front_right_position,
                           param_goal_position);
  group_bulk_write_->addParam(motor_front_right_.GetId(),
                              ADDR_PRO_GOAL_POSITION, LEN_PRO_GOAL_POSITION,
                              param_goal_position);
  CalculateNewGoalPosition(rear_left_angle_radians, rear_left_position,
                           param_goal_position);
  group_bulk_write_->addParam(motor_rear_left_.GetId(), ADDR_PRO_GOAL_POSITION,
                              LEN_PRO_GOAL_POSITION, param_goal_position);
  CalculateNewGoalPosition(rear_right_angle_radians, rear_right_position,
                           param_goal_position);
  group_bulk_write_->addParam(motor_rear_right_.GetId(), ADDR_PRO_GOAL_POSITION,
                              LEN_PRO_GOAL_POSITION, param_goal_position);
  group_bulk_write_->txPacket();
  group_bulk_write_->clearParam();
}

void DynamixelComms::SetServoPosition(const ServoInstance servo,
                                      double angle_degrees) {
  if (servo == kServoTurret) {
    std::scoped_lock lock(comms_mutex_);
    servo_turret_.SetPosition(angle_degrees);
  } else {
    assert("Invalid ServoInstance");
  }
}

void DynamixelComms::Reboot(const ServoInstance servo) {
  uint8_t dxl_id = GetServoId(servo);
  std::scoped_lock lock(comms_mutex_);
  switch (dxl_id) {
    case kTurretId:
      servo_turret_.Reboot();
      break;
    case kLeftFrontId:
      motor_front_left_.Reboot();
      break;
    case kLeftRearId:
      motor_rear_left_.Reboot();
      break;
    case kRightFrontId:
      motor_front_right_.Reboot();
      break;
    case kRightRearId:
      motor_rear_right_.Reboot();
      break;
  }
}

/* Private functions */

void DynamixelComms::DeadmansSwitchLoop() {
  RCUTILS_LOG_INFO_NAMED(kLogName, "Deadman's thread started.");
  // If the deadman's switch is not reset within the timeout period
  // then the robot will stop.
  // This is a safety feature to prevent the robot from running away
  // if the joystick is disconnected.
  while (rclcpp::ok()) {
    std::this_thread::sleep_for(kDeadmansSwitchTimeoutMs);
    if (deadmans_switch_enabled_) {
      if (deadmans_switch_) {
        RCUTILS_LOG_DEBUG_NAMED(kLogName,
                                "Deadman's switch timeout. Stopping robot.");
        std::scoped_lock lock(comms_mutex_);
        motor_front_left_.SetRPM(0.0);
        motor_rear_left_.SetRPM(0.0);
        motor_front_right_.SetRPM(0.0);
        motor_rear_right_.SetRPM(0.0);
      }
      deadmans_switch_ = true;
    }
  }
}

void DynamixelComms::DeadmansSwitchReset() {
  // Reset the switch.
  deadmans_switch_ = false;
  RCUTILS_LOG_DEBUG_NAMED(kLogName, "Deadman's switch reset.");
}

void DynamixelComms::DeadmansSwitchEnable(bool enable) {
  // Enable or disable the switch.
  deadmans_switch_enabled_ = enable;
  if (enable) {
    RCUTILS_LOG_DEBUG_NAMED(kLogName, "Deadman's switch enabled.");
  } else {
    RCUTILS_LOG_DEBUG_NAMED(kLogName, "Deadman's switch disabled.");
  }
}

uint8_t DynamixelComms::GetServoId(const ServoInstance servo) {
  uint8_t result = 0;
  // FIXME(AJB) Fragile code if number of servos change.
  switch (servo) {
    case kServoTurret:
      result = kTurretId;
      break;
    case kServoLeftFront:
      result = kLeftFrontId;
      break;
    case kServoLeftRear:
      result = kLeftRearId;
      break;
    case kServoRightFront:
      result = kRightFrontId;
      break;
    case kServoRightRear:
      result = kRightRearId;
      break;
    default:
      assert("Invalid ServoInstance");
  }
  return result;
}

ServoInstance DynamixelComms::GetServoInstance(const uint8_t dxl_id) {
  ServoInstance result = kServoNone;
  // FIXME(AJB) Fragile code if number of servos change.
  switch (dxl_id) {
    case kTurretId:
      result = kServoTurret;
      break;
    case kLeftFrontId:
      result = kServoLeftFront;
      break;
    case kLeftRearId:
      result = kServoLeftRear;
      break;
    case kRightFrontId:
      result = kServoRightFront;
      break;
    case kRightRearId:
      result = kServoRightRear;
      break;
  }
  return result;
}

std::string DynamixelComms::GetServoIdString(const uint8_t dxl_id) {
  std::string result;
  // FIXME(AJB) Fragile code if number of servos change.
  switch (dxl_id) {
    case kTurretId:
      result = "Turret";
      break;
    case kLeftFrontId:
      result = "LeftFront";
      break;
    case kLeftRearId:
      result = "LeftRear";
      break;
    case kRightFrontId:
      result = "RightFront";
      break;
    case kRightRearId:
      result = "RightRear";
      break;
  }
  return result;
}

ServoStatus DynamixelComms::DynamixelToServoStatus(
    const DynamixelServo::DynamixelServoStatus dynamixel_status) {
  ServoStatus result;
  result.instance_ = GetServoInstance(dynamixel_status.dxl_id_);
  result.instance_name_ = GetServoIdString(dynamixel_status.dxl_id_);
  result.hardware_status_ = dynamixel_status.hardware_status_;
  result.temperature_c_ = dynamixel_status.temperature_c_;
  result.input_voltage_ = dynamixel_status.input_voltage_;
  return result;
}

void DynamixelComms::ConvertRPMToGoalVelocity(const double goal_rpm,
                                              uint8_t goal_velocity[4]) {
  uint64_t new_velocity_ticks = static_cast<int64_t>(goal_rpm * (1.0 / 0.229));
  goal_velocity[0] = DXL_LOBYTE(DXL_LOWORD(new_velocity_ticks));
  goal_velocity[1] = DXL_HIBYTE(DXL_LOWORD(new_velocity_ticks));
  goal_velocity[2] = DXL_LOBYTE(DXL_HIWORD(new_velocity_ticks));
  goal_velocity[3] = DXL_HIBYTE(DXL_HIWORD(new_velocity_ticks));
}

void DynamixelComms::CalculateNewGoalPosition(const double angle_radians,
                                              const int32_t position,
                                              uint8_t goal_position[4]) {
  // Calculate new goal positions.
  int32_t new_position =
      position +
      static_cast<int32_t>(angle_radians / (2.0 * 3.1415926) * 4096.0 * -1.0);
  goal_position[0] = DXL_LOBYTE(DXL_LOWORD(new_position));
  goal_position[1] = DXL_HIBYTE(DXL_LOWORD(new_position));
  goal_position[2] = DXL_LOBYTE(DXL_HIWORD(new_position));
  goal_position[3] = DXL_HIBYTE(DXL_HIWORD(new_position));
}

void DynamixelComms::GetCurrentPositions(int32_t* front_left,
                                         int32_t* front_right,
                                         int32_t* rear_left,
                                         int32_t* rear_right) {
  // Clear the params.
  group_bulk_read_->clearParam();
  // Get position of all motors.
  bool add_param_result = group_bulk_read_->addParam(motor_front_left_.GetId(),
                                                     ADDR_PRO_PRESENT_POSITION,
                                                     LEN_PRO_PRESENT_POSITION);
  if (!add_param_result) {
    RCUTILS_LOG_ERROR_NAMED(kLogName, "Comms error adding param 1.");
  } else {
    add_param_result = group_bulk_read_->addParam(motor_front_right_.GetId(),
                                                  ADDR_PRO_PRESENT_POSITION,
                                                  LEN_PRO_PRESENT_POSITION);
    if (!add_param_result) {
      RCUTILS_LOG_ERROR_NAMED(kLogName, "Comms error adding param 2.");
    } else {
      add_param_result = group_bulk_read_->addParam(motor_rear_left_.GetId(),
                                                    ADDR_PRO_PRESENT_POSITION,
                                                    LEN_PRO_PRESENT_POSITION);
      if (!add_param_result) {
        RCUTILS_LOG_ERROR_NAMED(kLogName, "Comms error adding param 3.");
      } else {
        add_param_result = group_bulk_read_->addParam(motor_rear_right_.GetId(),
                                                      ADDR_PRO_PRESENT_POSITION,
                                                      LEN_PRO_PRESENT_POSITION);
        if (!add_param_result) {
          RCUTILS_LOG_ERROR_NAMED(kLogName, "Comms error adding param 4.");
        } else {
          // Do the read.
          int dxl_comm_result = group_bulk_read_->rxPacket();
          if (dxl_comm_result != COMM_SUCCESS) {
            RCUTILS_LOG_ERROR_NAMED(
                kLogName, "Comms error: %s.",
                packet_handler_->getTxRxResult(dxl_comm_result));
          } else {
            // Get the data, one by one.
            *front_left = GetMotorPresentPosition(motor_front_left_.GetId());
            *front_right = GetMotorPresentPosition(motor_front_right_.GetId());
            *rear_left = GetMotorPresentPosition(motor_rear_left_.GetId());
            *rear_right = GetMotorPresentPosition(motor_rear_right_.GetId());
          }
        }
      }
    }
  }
}

int32_t DynamixelComms::GetMotorPresentPosition(const uint8_t dxl_id) {
  int32_t result = 0;
  uint8_t dxl_error = 0;
  bool dxl_getdata_result = false;
  if (group_bulk_read_->getError(dxl_id, &dxl_error)) {
    RCUTILS_LOG_ERROR_NAMED(kLogName, "Comms DXL %d error: %s.", dxl_id,
                            packet_handler_->getRxPacketError(dxl_error));
  } else {
    dxl_getdata_result = group_bulk_read_->isAvailable(
        motor_front_left_.GetId(), ADDR_PRO_PRESENT_POSITION,
        LEN_PRO_PRESENT_POSITION);
    if (dxl_getdata_result) {
      result = group_bulk_read_->getData(dxl_id, ADDR_PRO_PRESENT_POSITION,
                                         LEN_PRO_PRESENT_POSITION);
    } else {
      RCUTILS_LOG_ERROR_NAMED(kLogName, "Comms DXL %d getdata failed.", dxl_id);
    }
  }
  return result;
}
