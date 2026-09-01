# Setup Pipebot 4WD

The installation is in 3 stages.

1. Install ROS 2 desktop and the ROS2 packages needed for the Pipebot 4WD.  Run `bash ros2/install_rpi.bash` which does the installation of packages etc. 
2. Add the following lines to your ~/.bash_aliases (or ~/.bashrc if you prefer)
```
. /opt/ros/humble/setup.bash
. ~/pipebot_4wd_ws/install/setup.bash
```
3. Create the robot workspace.  Run `workspace/setup_ws.bash`.
4. Install and setup the ROS 2 control packages.  Run `ros2_control/install_ros2_control.bash`.

More details on each stage can be found in the markdown files in the sub-directories.

Optionally, you can get bash scripts optimising your work environment, provided by Andy Blight:
```
git clone git@github.com:andyblight/bash_scripts.git
cd bash_scripts
./install.sh <REPOSITORY>
```

where REPOSITORY is either ubuntu22.04lts or raspbian-lite.
