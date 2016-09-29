cmake_minimum_required(VERSION 3.6)
project(glurg)

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	"${CMAKE_SOURCE_DIR}/cmake/")

add_subdirectory(libglurg)
