cmake_minimum_required(VERSION 3.6)
project(glurg)

add_executable(print_trace_funcs
	"source/print_trace_funcs.cpp")
target_link_libraries(print_trace_funcs PRIVATE
	glurg)
set_property(TARGET print_trace_funcs PROPERTY CXX_STANDARD 14)

find_package(PNG REQUIRED)

add_executable(save_image
	"source/save_image.cpp")
target_include_directories(save_image PRIVATE
	${PNG_INCLUDE_DIRS})
target_link_libraries(save_image PRIVATE
	${PNG_LIBRARIES}
	glurg)
set_property(TARGET save_image PROPERTY CXX_STANDARD 14)