#!/bin/bash
# Build packages, for use after after cloning the repos.
# Must be run from the workspace directory.

colcon build

echo
echo "$0 took $SECONDS seconds."
echo
echo "Source the workspace once the packages have been built:"
echo ". ./install/setup.bash"
echo
