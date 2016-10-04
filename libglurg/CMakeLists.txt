cmake_minimum_required(VERSION 3.6)
project(glurg)

find_package(PkgConfig REQUIRED)

find_package(Boost REQUIRED)
find_package(Snappy REQUIRED)
pkg_search_module(Sodium REQUIRED libsodium)

set(LIBGLURG_INCLUDE_FILES
	"include/glurg/resources/resource.hpp"
	"include/glurg/resources/textureResource.hpp"
	"include/glurg/common/fileStream.hpp"
	"include/glurg/common/hash.hpp"
	"include/glurg/common/simpleFileStream.hpp"
	"include/glurg/common/snappyAdapter.hpp"
	"include/glurg/trace/bitmaskSignature.hpp"
	"include/glurg/trace/callSignature.hpp"
	"include/glurg/trace/enumerationSignature.hpp"
	"include/glurg/trace/traceFile.hpp")

set(LIBGLURG_SOURCE_FILES
	"source/resources/textureResource.cpp"
	"source/common/hash.cpp"
	"source/common/simpleFileStream.cpp"
	"source/common/snappyAdapter.cpp"
	"source/trace/bitmaskSignature.cpp"
	"source/trace/callSignature.cpp"
	"source/trace/enumerationSignature.cpp"
	"source/trace/events.cpp"
	"source/trace/traceFile.cpp"
	"source/trace/values.cpp")

set(LIBGLURG_INCLUDE_DIRS
	"include")

add_library(libglurg SHARED
	${LIBGLURG_INCLUDE_FILES}
	${LIBGLURG_SOURCE_FILES})

set_target_properties(libglurg PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(libglurg
	PUBLIC
	${LIBGLURG_INCLUDE_DIRS})

target_include_directories(libglurg
	PRIVATE
	${Boost_INCLUDE_DIRS}
	${Snappy_INCLUDE_DIRS}
	${Sodium_INCLUDE_DIRS})

target_link_libraries(libglurg PUBLIC
	${Boost_LIBRARIES} ${Boost_LDFLAGS}
	${Sodium_LIBRARIES} ${Sodium_LDFLAGS}
	${Snappy_LIBRARIES} ${Snappy_LDFLAGS})

target_compile_features(libglurg PUBLIC
	cxx_nullptr
	cxx_override
	cxx_defaulted_functions)
