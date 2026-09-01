set -e

colcon build --packages-select \
    pipebot_4wd \
    pipebot_msgs \
    sensing_subsystem \
    tank_ultra_pkg \
    dynamixel_driver \
    imu_bno085 \
    ldlidar_stl_ros2 \
    rpi_monitor \
    low_control_with_imu \
    localization_subsystem \
    chassis_controller \
    control_system \
    # Fix later
    #camera_ros \
    
    
    
