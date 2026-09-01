#!/usr/bin/env python3

import math
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
from std_msgs.msg import Bool, Float32MultiArray

class ObstacleDetection(Node):
    def __init__(self):
        super().__init__("obstacle_detection")
        
# Parameters
        self.declare_parameter("front_angle_deg", 90)
        self.declare_parameter("stop_distance_m", 0.05)
        self.declare_parameter("min_close_points", 20)
        self.declare_parameter("lidar_timeout", 1.0)

        self.front_angle_deg = float(self.get_parameter("front_angle_deg").value)
        self.stop_distance_m = float(self.get_parameter("stop_distance_m").value)
        self.min_close_points = int(self.get_parameter("min_close_points").value)
        self.lidar_timeout = float(self.get_parameter("lidar_timeout").value)

        self.ellipse_distances = None
        self.last_scan_time = None
        self.stop_required = True

        self.stop_pub = self.create_publisher(Bool, "/stop_obstacle", 10)
        self.create_subscription(LaserScan, "/scan/front", self.scan_callback, 10)
        self.create_subscription(Float32MultiArray, "/ellipse_model/distances", self.ellipse_callback, 10)
        self.create_timer(0.1, self.timer_callback)

        self.get_logger().info("Obstacle detection started")

    def ellipse_callback(self, msg):
        self.ellipse_distances = msg.data
    def scan_callback(self, msg):
        self.last_scan_time = self.get_clock().now()
        if self.ellipse_distances is None:
            self.stop_required = True
            return
        close_points = 0
        for i, measured_distance in enumerate(msg.ranges):
            if not math.isfinite(measured_distance):
                continue
            angle = msg.angle_min + i * msg.angle_increment
            angle_deg = math.degrees(angle)

# Only check selected front sector. 360 degrees means check the entire scan.
            if self.front_angle_deg < 360.0:
                normalised_angle = ((angle_deg + 180.0) % 360.0) - 180.0

                if abs(normalised_angle) > self.front_angle_deg:
                    continue

# Convert LiDAR angle to ellipse model index 0-359.
            model_index = int(round(angle_deg)) % 360
            predicted_distance = self.ellipse_distances[model_index]

            if not math.isfinite(predicted_distance):
                continue

# Count points closer than the predicted pipe wall.
            if measured_distance <= predicted_distance - self.stop_distance_m:
                close_points += 1

        self.stop_required = close_points >= self.min_close_points

    def timer_callback(self):
        msg = Bool()
        if self.last_scan_time is None:
            msg.data = True
        else:
            age = (self.get_clock().now() - self.last_scan_time).nanoseconds / 1e9
            if age > self.lidar_timeout:
                msg.data = True
            else:
                msg.data = self.stop_required
        self.stop_pub.publish(msg)


def main(args=None):
    rclpy.init(args=args)
    node = ObstacleDetection()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        msg = Bool()
        msg.data = True
        node.stop_pub.publish(msg)
        node.destroy_node()
        rclpy.shutdown()

if __name__ == "__main__":
    main()
