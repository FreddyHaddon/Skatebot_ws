#!/bin/bash
# Creates a workspace using the specified .repos file.
# $1 the repos file to use.
# $2 the workspace directory.  The Current directory is used if not used.

# Stop on first error.
set -e

usage () {
  echo
  echo "Usage: $0 <.repos file> [workspace directory]"
  echo
  echo "<.repos file>          The .repos file to use."
  echo "[workspace directory]  The workspace directory to use."
  echo "                       Default is '<scripts dir>/../../..'"
  echo
}

if [[ $# < 1 ]]
then
  echo "Illegal number of parameters" >&2
  usage
  exit 2
else
  repos_file="$(cd "$(dirname "$1")"; pwd)/$(basename "$1")"
  ws_dir="$(cd ${2:-`pwd`/../../..}; pwd)"
  if [[ ! -f ${repos_file} ]]
  then
    echo "The file '${repos_file}' was not found." >&2
    usage
    exit 3
  else
    echo "About to create a workspace using the file '${repos_file}' in the directory"
    echo "'${ws_dir}'"
    echo "Enter 'yes' to create the workspace or anything else to abort."
    read response
    if [[ ${response} != "yes" ]]
    then
      echo "Aborted" >&2
      echo
      exit 1
    fi
  fi
fi

# Only ask for GitHub login for the first time for this shell.
git config --global credential.helper "cache"

# Use VCS to create the workspace.
mkdir -p ${ws_dir}
cd ${ws_dir}
# Use one worker to make it easier to debug.
vcs --workers=1 import < ${repos_file}

echo
echo "$0 took $SECONDS seconds."
echo "All repos created and checked out in the '${ws_dir}' directory using the hashes used for the release."
echo
