#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import Bool
from pipebot_msgs.msg import Servo

class TurretJoystickNode(Node):
    def __init__(self):
        super().__init__("turret_joystick_node")

        self.declare_parameter("joy_topic", "/joy")
        self.declare_parameter("servo_topic", "/servo/turret")
        self.declare_parameter("button_index", 2)
        self.declare_parameter("forward_command", 180)
        self.declare_parameter("backward_command", 0)
        self.declare_parameter("is_forward", True)
        self.declare_parameter("turret_direction_topic", "/turret_joystick/is_forward")
        self.declare_parameter("state_publish_rate", 2.0)
        self.declare_parameter("require_servo_subscriber", True)
        self.declare_parameter("servo_command_repeats", 5)
        self.declare_parameter("servo_repeat_period", 0.1)

        self.joy_topic = self.get_parameter("joy_topic").value
        self.servo_topic = self.get_parameter("servo_topic").value
        self.button_index = int(self.get_parameter("button_index").value)
        self.forward_command = int(self.get_parameter("forward_command").value)
        self.backward_command = int(self.get_parameter("backward_command").value)
        self.turret_direction_topic = self.get_parameter("turret_direction_topic").value
        self.state_publish_rate = float(self.get_parameter("state_publish_rate").value)
        self.require_servo_subscriber = bool(self.get_parameter("require_servo_subscriber").value)
        self.servo_command_repeats = int(self.get_parameter("servo_command_repeats").value)
        self.servo_repeat_period = float(self.get_parameter("servo_repeat_period").value)

# Assume turret facing forwards
        self.is_forward = bool(self.get_parameter("is_forward").value)
        self.last_button_state = 0
        
        self.pending_servo_command = None
        self.pending_servo_repeats = 0
        self.joy_sub = self.create_subscription(Joy, self.joy_topic, self.joy_callback, 10)
        self.servo_pub = self.create_publisher(Servo, self.servo_topic, 10)

        self.alignment_timer = self.create_timer(2.0, self.align_turret_on_startup)
        self.direction_pub = self.create_publisher(Bool, self.turret_direction_topic, 10)
        self.create_timer(1.0 / self.state_publish_rate, self.publish_turret_direction)
        self.create_timer(self.servo_repeat_period, self.repeat_servo_command)
        
        self.publish_turret_direction()
        self.get_logger().info("Turret joystick node started")
        self.get_logger().info(f"Subscribing to {self.joy_topic}")
        self.get_logger().info(f"Publishing servo commands to {self.servo_topic}")
        self.get_logger().info(f"Publishing turret direction to {self.turret_direction_topic}")
    
    def align_turret_on_startup(self):
        self.alignment_timer.cancel()
        self.publish_servo_command(self.forward_command)
        self.pending_servo_command = self.forward_command
        self.pending_servo_repeats = max(0, self.servo_command_repeats - 1)

        self.is_forward = True
        self.publish_turret_direction()

        self.get_logger().info(f"Turret aligned on startup, angle_degrees={self.forward_command}")
    
    def joy_callback(self, msg: Joy):
        if self.button_index >= len(msg.buttons):
            return

        button_state = msg.buttons[self.button_index]

# Rising edge only.
        if button_state == 1 and self.last_button_state == 0:
            self.toggle_turret()
        self.last_button_state = button_state

    def toggle_turret(self):
        if self.require_servo_subscriber and self.servo_pub.get_subscription_count() == 0:
            self.get_logger().warn(
                f"Ignored turret button: no subscriber on {self.servo_topic}. "
                "Turret direction state was not changed."
            )
            self.publish_turret_direction()
            return

        new_is_forward = not self.is_forward
        command = self.forward_command if new_is_forward else self.backward_command

# Send the servo command first
        self.publish_servo_command(command)
        self.pending_servo_command = command
        self.pending_servo_repeats = max(0, self.servo_command_repeats - 1)

        self.is_forward = new_is_forward
        self.publish_turret_direction()

        direction = "forward" if self.is_forward else "backward"
        self.get_logger().info(f"Turret command accepted: {direction}, angle_degrees={command}")

    def publish_servo_command(self, command):
        servo_msg = Servo()
        servo_msg.angle_degrees = int(command)
        self.servo_pub.publish(servo_msg)

    def repeat_servo_command(self):
        if self.pending_servo_command is None:
            return

        if self.pending_servo_repeats <= 0:
            self.pending_servo_command = None
            return

        self.publish_servo_command(self.pending_servo_command)
        self.pending_servo_repeats -= 1

    def publish_turret_direction(self):
        msg = Bool()
        msg.data = self.is_forward
        self.direction_pub.publish(msg)

def main(args=None):
    rclpy.init(args=args)
    node = TurretJoystickNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
