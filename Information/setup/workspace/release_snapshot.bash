#!/bin/bash
# Creates a .repos from the workspace.
# $1 is the file name for the .repos file.

# Stop on first error.
set -e

# Get access to the scripts directory.
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )"
WORKSPACE_DIR=${script_dir}/../../..

usage () {
  echo
  echo "Usage: $0 <release .repos file name>"
  echo
  echo "<release .repos file name>  A valid file name of the form 'my_release.repos'."
  echo
}

show_status () {
  cd ${ws_dir}
  echo
  echo "The status of the repos in the workspace follows:"
  vcs status
  echo "If any files have been modified, the release will fail."
  echo
}

if [[ $# -ne 1 ]]
then
  echo "Illegal number of parameters" >&2
  usage
  exit 2
else
  show_status
  release_filename=${1}
  echo "About to create a release snapshot using the file name '${release_filename}'."
  echo "Enter 'yes' to make the release or anything else to abort."
  read response
  if [[ ${response} != "yes" ]]
  then
    echo "Aborted" >&2
    echo
    exit 1
  fi
fi

# Create a .repos file with the exact versions of all repos.
cd ${WORKSPACE_DIR}
vcs export --workers 1 --exact > ${release_filename}.repos

echo
echo "$0 took $SECONDS seconds."
echo
