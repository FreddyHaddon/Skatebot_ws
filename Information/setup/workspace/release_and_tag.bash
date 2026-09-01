#!/bin/bash
# Creates a release and pushes all changes to the GitHub server.
# $1 is the branch name to use for the release.

# Stop on first error.
set -e

# Get access to the scripts directory.
scripts_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &>/dev/null && pwd )"
ws_dir=${scripts_dir}/../../..

usage () {
  echo
  echo "Usage: $0 <release name> <release note>"
  echo
  echo "<release name>  A vaild git tag name."
  echo "<release note>  A message for the annotation, e.g. \"Awesome release 1\"."
  echo "                The double quotes are impaortant!"
  echo
}

show_status () {
  cd ${ws_dir}
  echo
  echo "The status of the repos in the workspace follows:"
  vcs status
  echo "WARNING: If any files have been modified, the release will fail."
  echo
}

if [[ $# -ne 2 ]]
then
  echo "Illegal number of parameters" >&2
  usage
  exit 2
else
  show_status
  release_tag=${1}
  release_note=${2}
  echo "About to create a release using the name '${release_tag}' with the annotation"
  echo "'${release_note}.'"
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
cd ${ws_dir}
vcs export --workers 1 --exact > ${release_dir}/${release_tag}.repos

# Use VCS to add the release tag to all repos and push.
vcs custom --workers 1 --git --args tag -f -a ${release_tag} -m "${release_note}"
# Use VCS to push tags on all repos.
vcs custom --workers 1 --git --args push origin ${release_tag}

# Add and commit the new .repos file.
cd ${scripts_dir}
git add .
git commit -m "Added .repos file for release"
git push
# Tag this repo as well and push.
git tag -f -a ${release_tag} -m "${release_note}"
git push origin ${release_tag}

echo
echo "$0 took $SECONDS seconds."
echo
