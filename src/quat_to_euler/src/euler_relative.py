#!/usr/bin/env python3
# Uses first IMU reading as zero.
import math
import rclpy
from rclpy.node import Node
from pipebot_msgs.msg import EulerAngles

class RelativeEuler(Node):

    def __init__(self):
        super().__init__("relative_euler")

        self.initial_set = False
        self.initial_roll = 0.0
        self.initial_pitch = 0.0
        self.initial_yaw = 0.0

        self.subscription = self.create_subscription(EulerAngles, "/imu/euler", self.euler_callback, 10,)
        self.publisher = self.create_publisher(EulerAngles, "/imu/euler_relative", 10,)
        self.get_logger().info("Relative Euler node started")

    def euler_callback(self, msg):
        if not self.initial_set:
            self.initial_roll = msg.roll
            self.initial_pitch = msg.pitch
            self.initial_yaw = msg.yaw
            self.initial_set = True
            self.get_logger().info(
                f"Reference orientation: "
                f"roll={msg.roll:.1f}, "
                f"pitch={msg.pitch:.1f}, "
                f"yaw={msg.yaw:.1f}"
            )

        relative = EulerAngles()

        relative.roll = math.remainder(msg.roll - self.initial_roll, 360.0)
        relative.pitch = math.remainder(msg.pitch - self.initial_pitch, 360.0)
        relative.yaw = math.remainder(msg.yaw - self.initial_yaw, 360.0)

        self.publisher.publish(relative)

def main(args=None):
    rclpy.init(args=args)

    node = RelativeEuler()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == "__main__":
    main()
