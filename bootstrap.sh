#!/bin/bash -ex

#This script should be run to setup git and your development
#environment properly.

#stolen from google - cloud - sdk
script_link="$( readlink "$BASH_SOURCE" )" || script_link="$BASH_SOURCE"
apparent_sdk_dir="${script_link%/*}"
if [ "$apparent_sdk_dir" == "$script_link" ]; then
  apparent_sdk_dir=.
fi
sdk_dir="$( command cd -P "$apparent_sdk_dir" > /dev/null && pwd -P )"

cp ${sdk_dir}/etc/git/hooks/pre-commit ${sdk_dir}/.git/hooks/
