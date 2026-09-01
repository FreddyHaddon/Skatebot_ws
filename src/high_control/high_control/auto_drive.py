#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from geometry_msgs.msg import Twist
from std_msgs.msg import Bool

class AutoDrive(Node):
    def __init__(self):
        super().__init__("auto_drive")

        self.declare_parameter("joy_topic", "/joy")
        self.declare_parameter("stop_topic", "/stop_obstacle")
        self.declare_parameter("cmd_vel_topic", "/cmd_vel")
        self.declare_parameter("turret_direction_topic", "/turret_joystick/is_forward")
        self.declare_parameter("toggle_button_index", 0)
        self.declare_parameter("forward_speed", 0.15)
        self.declare_parameter("publish_rate", 10.0)

        self.joy_topic = self.get_parameter("joy_topic").value
        self.stop_topic = self.get_parameter("stop_topic").value
        self.cmd_vel_topic = self.get_parameter("cmd_vel_topic").value
        self.turret_direction_topic = self.get_parameter("turret_direction_topic").value
        self.toggle_button_index = int(self.get_parameter("toggle_button_index").value)
        self.forward_speed = float(self.get_parameter("forward_speed").value)
        self.publish_rate = float(self.get_parameter("publish_rate").value)

        self.auto_enabled = False
        self.stop_required = True  # until obstacle_detection says clear
        self.is_forward = True     # assume turret starts facing forwards
        self.last_buttons = []

        self.cmd_pub = self.create_publisher(Twist, self.cmd_vel_topic, 10)

        self.create_subscription(Joy, self.joy_topic, self.joy_callback, 10)
        self.create_subscription(Bool, self.stop_topic, self.stop_callback, 10)
        self.create_subscription(Bool, self.turret_direction_topic, self.turret_direction_callback, 10)
        self.create_timer(1.0 / self.publish_rate, self.timer_callback)
        self.get_logger().info("Auto drive node started")
        self.get_logger().info(f"Subscribing to {self.joy_topic}")
        self.get_logger().info(f"Subscribing to {self.stop_topic}")
        self.get_logger().info(f"Subscribing to {self.turret_direction_topic}")
        self.get_logger().info(f"Publishing to {self.cmd_vel_topic}")

    def joy_callback(self, msg: Joy):
        if not self.last_buttons:
            self.last_buttons = list(msg.buttons)
            return
        if self.toggle_button_index >= len(msg.buttons):
            self.get_logger().warn("Toggle button index is outside Joy button list")
            self.last_buttons = list(msg.buttons)
            return
        was_pressed = self.last_buttons[self.toggle_button_index] == 1
        is_pressed = msg.buttons[self.toggle_button_index] == 1

# Rising edge toggle
        if is_pressed and not was_pressed:
            if self.auto_enabled:
                self.auto_enabled = False
                self.get_logger().info("Auto drive OFF")
                self.publish_stop()
            else:
                if self.stop_required:
                    self.get_logger().warn("Cannot enable auto drive: obstacle stop active")
                    self.publish_stop()
                else:
                    self.auto_enabled = True
                    direction = "forwards" if self.is_forward else "backwards"
                    self.get_logger().info(f"Auto drive ON, driving {direction}")
                    self.publish_drive_command()
        self.last_buttons = list(msg.buttons)

    def stop_callback(self, msg: Bool):
        self.stop_required = msg.data
        if self.stop_required and self.auto_enabled:
            self.get_logger().warn("Obstacle stop active. Auto drive OFF.")
            self.auto_enabled = False
            self.publish_stop()

    def turret_direction_callback(self, msg: Bool):
        previous_is_forward = self.is_forward
        self.is_forward = msg.data
        if self.is_forward != previous_is_forward:
            self.get_logger().info("Turret direction changed. Stopping auto drive.")
            self.auto_enabled = False
            self.publish_stop()

    def timer_callback(self):
        if not self.auto_enabled:
            return
        if self.stop_required:
            self.auto_enabled = False
            self.publish_stop()
            return
        self.publish_drive_command()

    def publish_drive_command(self):
        cmd = Twist()
        cmd.linear.x = self.forward_speed if self.is_forward else -self.forward_speed
        cmd.angular.z = 0.0
        self.cmd_pub.publish(cmd)

    def publish_stop(self):
        cmd = Twist()
        cmd.linear.x = 0.0
        cmd.angular.z = 0.0
        self.cmd_pub.publish(cmd)

def main(args=None):
    rclpy.init(args=args)
    node = AutoDrive()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.publish_stop()
        node.destroy_node()
        rclpy.shutdown()

if __name__ == "__main__":
    main()
