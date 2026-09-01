#!/bin/bash
# Build packages, for use after after cloning the repos.

# Include the vars.bash script.
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )"
setup_dir=${script_dir}/..
. ${setup_dir}/vars.bash

#install dependencies for packages
rosdep update
rosdep install --from-paths ${WORKSPACE_DIR}/src -y -r --ignore-src

# Build the packages.
echo "Building packages..."
cd ${WORKSPACE_DIR}
. /opt/ros/${ROS2_DISTRO}/setup.bash
colcon build
# Rebuild the "special builds".
colcon build --symlink-install --packages-select rtf_sensors

echo
echo "$0 took $SECONDS seconds."
echo
echo "Source the workspace once the packages have been built:"
echo ". ./install/setup.bash"
echo
