#!/usr/bin/env python3

import math
import rclpy
from rclpy.node import Node
from std_msgs.msg import Bool, Float32MultiArray
from pipebot_msgs.msg import EulerAngles

class EllipseModel(Node):

    def __init__(self):
        super().__init__("ellipse_model")

# Geometry
        self.pipe_radius = 0.15                      # 300 mm diameter pipe
        self.imu_height = 0.10                       # IMU height above pipe floor
        self.lidar_height = 0.18                     # lidar height above pipe floor
        self.lidar_forward = 0.11                    # lidar distance from IMU
        self.lidar_down_angle = math.radians(30.0)   # front lidar is angled 30 degrees downwards

# Topics
        self.create_subscription(EulerAngles, "/imu/euler_relative", self.imu_callback, 10)
        self.create_subscription( Bool, "/turret_joystick/is_forward", self.turret_callback, 10)
        self.model_pub = self.create_publisher(Float32MultiArray, "/ellipse_model/distances", 10)

# Relative orientation
        self.roll = 0.0
        self.pitch = 0.0

# Assume turret initially faces forwards.
        self.is_forward = True

        self.get_logger().info("Ellipse model started")

    def turret_callback(self, msg):
        self.is_forward = msg.data

    def imu_callback(self, msg):
        self.roll = math.radians(msg.roll)
        self.pitch = math.radians(msg.pitch)
        self.calculate_ellipse()

 #3D rotations
    def rotate_x(self, vector, angle):
        x, y, z = vector
        return (x, y * math.cos(angle) - z * math.sin(angle), y * math.sin(angle) + z * math.cos(angle))

    def rotate_y(self, vector, angle):
        x, y, z = vector
        return (x * math.cos(angle) + z * math.sin(angle), y, -x * math.sin(angle) + z * math.cos(angle))

    def rotate_z(self, vector, angle):
        x, y, z = vector
        return (x * math.cos(angle) - y * math.sin(angle), x * math.sin(angle) + y * math.cos(angle), z)

    def rotate_robot(self, vector):
        vector = self.rotate_x(vector, self.roll)
        vector = self.rotate_y(vector, self.pitch)
        return vector
        
# Pipe model
    def calculate_ellipse(self):
        if self.is_forward:
            turret_angle = 0.0
        else:
            turret_angle = math.pi

# Position of LiDAR relative to IMU.
        lidar_offset = (self.lidar_forward, 0.0, self.lidar_height - self.imu_height)

# Rotate offset if turret is backwards.
        lidar_offset = self.rotate_z(lidar_offset, turret_angle)

# Rotate position according to robot attitude.
        lidar_offset = self.rotate_robot(lidar_offset)

# IMU position relative to centre of pipe.
        imu_z = self.imu_height - self.pipe_radius

        lidar_position = (lidar_offset[0], lidar_offset[1], imu_z + lidar_offset[2])

        predicted_distances = []

# Calculate predicted pipe-wall distance for every degree.
        for angle_deg in range(360):

            angle = math.radians(angle_deg)

# LiDAR ray in its own scan plane, 30 degrees downwards.
            ray = (math.cos(angle), math.sin(angle), 0.0)
            ray = self.rotate_y(ray, self.lidar_down_angle)
            ray = self.rotate_z(ray, turret_angle)

# Account for relative robot orientation.
            ray = self.rotate_robot(ray)
            distance = self.distance_to_pipe(lidar_position, ray)
            predicted_distances.append(distance)

        msg = Float32MultiArray()
        msg.data = predicted_distances

        self.model_pub.publish(msg)

    def distance_to_pipe(self, origin, direction):

        _, y, z = origin
        _, dy, dz = direction
        a = dy * dy + dz * dz
        b = 2.0 * (y * dy + z * dz)
        c = y * y + z * z - self.pipe_radius * self.pipe_radius

# Ray exactly parallel to pipe.
        if abs(a) < 1e-9:
            return float("inf")

        discriminant = b * b - 4.0 * a * c

        if discriminant < 0.0:
            return float("inf")
            
        sqrt_d = math.sqrt(discriminant)
        t1 = (-b - sqrt_d) / (2.0 * a)
        t2 = (-b + sqrt_d) / (2.0 * a)

# Only intersections in front of the LiDAR are valid.
        valid = [t for t in (t1, t2) if t > 0.0]
        if not valid:
            return float("inf")
        return min(valid)

def main(args=None):
    rclpy.init(args=args)
    node = EllipseModel()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == "__main__":
    main()
