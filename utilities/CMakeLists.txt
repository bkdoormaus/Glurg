cmake_minimum_required(VERSION 3.6)
project(glurg)

add_executable(print_trace_funcs
	"source/print_trace_funcs.cpp")
target_link_libraries(print_trace_funcs PRIVATE
	libglurg)
