# https://github.com/lemire/FastPFor/blob/master/cmake_modules/Findsnappy.cmake
#
# [bk]door.maus: Modified to use "Snappy" rather than "snappy"
#
# ...
# 
# This code is released under the
# Apache License Version 2.0 http://www.apache.org/licenses/.
#
# Copyright (c) 2012 Louis Dionne
#
# Find snappy compression library and includes. This module defines:
#   Snappy_INCLUDE_DIRS - The directories containing snappy's headers.
#   Snappy_LIBRARIES    - A list of snappy's libraries.
#   Snappy_FOUND        - Whether snappy was found.
#
# This module can be controlled by setting the following variables:
#   Snappy_ROOT - The root directory where to find snappy. If this is not
#                 set, the default paths are searched.

if(NOT Snappy_ROOT)
    find_path(Snappy_INCLUDE_DIRS snappy.h)
    find_library(Snappy_LIBRARIES NAMES snappy)
else()
    find_path(Snappy_INCLUDE_DIRS snappy.h NO_DEFAULT_PATH PATHS ${Snappy_ROOT})
    find_library(Snappy_LIBRARIES NAMES snappy NO_DEFAULT_PATH PATHS ${Snappy_ROOT})
endif()

if(Snappy_INCLUDE_DIRS AND Snappy_LIBRARIES)
    set(Snappy_FOUND TRUE)
else()
    set(Snappy_FOUND FALSE)
    set(Snappy_INCLUDE_DIR)
    set(Snappy_LIBRARIES)
endif()

mark_as_advanced(Snappy_LIBRARIES Snappy_INCLUDE_DIRS)