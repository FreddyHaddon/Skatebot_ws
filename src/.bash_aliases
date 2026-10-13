source /opt/ros/jazzy/setup.bash
export ROS_DOMAIN_ID=14

if [ -f ~/ws/install/setup.bash ]; then
    source ~/ws/install/setup.bash
fi

wsbuild() {
  cd ~/ws || return
  source /opt/ros/jazzy/setup.bash
  colcon build --symlink-install || return
  source ~/ws/install/setup.bash
}

alias srcjaz='source /opt/ros/jazzy/setup.bash'
alias srcins='source ~/ws/install/setup.bash'
alias build='colcon build --symlink-install'

alias sensors='ros2 launch pipebot_4wd sensors.launch.py'
alias record='ros2 launch pipebot_4wd record_data.launch.py'
alias robot='ros2 launch pipebot_4wd robot.launch.py'
alias semiauto='ros2 launch high_control semi_autonomous_control.launch.py'

alias fox='ros2 launch foxglove_bridge foxglove_bridge_launch.xml'
alias ip='hostname -I'

alias lidar='ros2 launch ldlidar_stl_ros2 both_ld06.launch.py'
alias camera='ros2 launch camera_ros camera.launch.py'
alias imu='ros2 launch bno08x_driver bno085_i2c.launch.py'
alias joy='ros2 launch pipebot_4wd joystick.launch.py'

alias non-euler='ros2 topic echo /imu'
alias euler='ros2 topic echo /imu/euler'
alias relative='ros2 topic echo /imu/euler_relative'
alias joytest='ros2 topic echo /joy'

alias topics='ros2 topic list'
alias nodes='ros2 node list'