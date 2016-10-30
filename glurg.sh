#!/bin/sh

# This is an example script for now. Change the locations to match your setup,
# if necessary.
GLURG_SCRIPT_LOCATION=$(readlink -f "$0")
GLURG_ROOT=$(dirname "$GLURG_SCRIPT_LOCATION")

# Set LUA_PATH to include the Glurg source.
LUA_PATH="${GLURG_ROOT}/?.lua;${LUA_PATH}"

# Set LUA_CPATH to include the path to libglurg.so.
LUA_CPATH="${GLURG_ROOT}/bin/lib?.so;${LUA_CPATH}"

moon "${GLURG_ROOT}/glurg/Main.moon" "$@"
