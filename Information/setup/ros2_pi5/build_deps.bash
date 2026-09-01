set -e

# Dependencies
colcon build --packages-select \
	xacro \
	sdl2_vendor joy teleop_twist_joy \
	dynamixel_sdk \
	diagnostic_updater diagnostic_aggregator \
	cv_bridge
