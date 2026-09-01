#!/bin/bash
# Set up the workspace to build the whole Pipebot project.
# This is designed to be run from inside the docker.
set -e

# Include the vars.bash script.
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )"
setup_dir=${script_dir}/..
. ${setup_dir}/vars.bash

# Create the workspace.
mkdir -p ${COLCON_WS_DIR}
${script_dir}/create_ws.bash pipebot_4wd.repos ${COLCON_WS_DIR}

# Build it to make sure everything is present.
${script_dir}/build_ws.bash

# Copy the build.bash file
cp ${script_dir}/build.bash ${COLCON_WS_DIR}

echo
echo "$0 took $SECONDS seconds."
echo