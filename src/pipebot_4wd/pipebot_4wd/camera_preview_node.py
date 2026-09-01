#!/usr/bin/env python3

import time
from typing import Optional
import cv2
from cv_bridge import CvBridge
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image, CompressedImage

class CameraPreviewNode(Node):
    def __init__(self) -> None:
        super().__init__("camera_preview_node")

        self.declare_parameter("input_topic", "/camera/image_raw")
        self.declare_parameter("output_topic", "/camera/preview/compressed")
        self.declare_parameter("width", 320)
        self.declare_parameter("height", 240)
        self.declare_parameter("fps", 15.0)
        self.declare_parameter("jpeg_quality", 50)

        self.input_topic = str(self.get_parameter("input_topic").value)
        self.output_topic = str(self.get_parameter("output_topic").value)
        self.width = int(self.get_parameter("width").value)
        self.height = int(self.get_parameter("height").value)
        self.fps = float(self.get_parameter("fps").value)
        self.jpeg_quality = int(self.get_parameter("jpeg_quality").value)

        self.bridge = CvBridge()
        self.last_publish_time = 0.0
        self.min_period = 1.0 / self.fps if self.fps > 0 else 0.0

        self.publisher = self.create_publisher(
            CompressedImage,
            self.output_topic,
            10
        )

        self.subscription = self.create_subscription(
            Image,
            self.input_topic,
            self.image_callback,
            10
        )

        self.get_logger().info(
            f"Preview node running: {self.input_topic} -> {self.output_topic}, "
            f"{self.width}x{self.height} at {self.fps} FPS"
        )

    def image_callback(self, msg: Image) -> None:
        now = time.monotonic()

        if now - self.last_publish_time < self.min_period:
            return

        try:
            frame = self.bridge.imgmsg_to_cv2(msg, desired_encoding="bgr8")
        except Exception as e:
            self.get_logger().error(f"Failed to convert image: {e}")
            return

        preview = cv2.resize(
            frame,
            (self.width, self.height),
            interpolation=cv2.INTER_AREA
        )

        success, encoded = cv2.imencode(
            ".jpg",
            preview,
            [int(cv2.IMWRITE_JPEG_QUALITY), self.jpeg_quality]
        )

        if not success:
            self.get_logger().error("Failed to encode preview image")
            return

        out_msg = CompressedImage()
        out_msg.header = msg.header
        out_msg.format = "jpeg"
        out_msg.data = encoded.tobytes()

        self.publisher.publish(out_msg)
        self.last_publish_time = now

def main(args: Optional[list[str]] = None) -> None:
    rclpy.init(args=args)
    node = CameraPreviewNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()
