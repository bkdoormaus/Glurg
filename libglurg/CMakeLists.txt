cmake_minimum_required(VERSION 3.6)
project(glurg)

find_package(PkgConfig REQUIRED)

find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(Snappy REQUIRED)
pkg_search_module(Sodium REQUIRED libsodium)
pkg_search_module(JsonCpp REQUIRED jsoncpp)

set(LIBGLURG_INCLUDE_FILES
	"include/glurg/common/base64.hpp"
	"include/glurg/common/fileStream.hpp"
	"include/glurg/common/hash.hpp"
	"include/glurg/common/process.hpp"
	"include/glurg/common/simpleFileStream.hpp"
	"include/glurg/common/snappyAdapter.hpp"
	"include/glurg/resources/resourceBlob.hpp"
	"include/glurg/resources/resourceBlobBuffer.hpp"
	"include/glurg/resources/resourceBlobBuilder.hpp"
	"include/glurg/resources/renderState.hpp"
	"include/glurg/resources/renderValue.hpp"
	"include/glurg/resources/textureResourceBlob.hpp"
	"include/glurg/trace/bitmaskSignature.hpp"
	"include/glurg/trace/callSignature.hpp"
	"include/glurg/trace/enumerationSignature.hpp"
	"include/glurg/trace/structure.hpp"
	"include/glurg/trace/structureSignature.hpp"
	"include/glurg/trace/traceFile.hpp")

set(LIBGLURG_SOURCE_FILES
	"source/common/base64.cpp"
	"source/common/hash.cpp"
	"source/common/process.cpp"
	"source/common/simpleFileStream.cpp"
	"source/common/snappyAdapter.cpp"
	"source/resources/renderState.cpp"
	"source/resources/renderValue.cpp"
	"source/resources/resourceBlobBuffer.cpp"
	"source/resources/textureResourceBlob.cpp"
	"source/trace/bitmaskSignature.cpp"
	"source/trace/call.cpp"
	"source/trace/callSignature.cpp"
	"source/trace/enumerationSignature.cpp"
	"source/trace/events.cpp"
	"source/trace/structure.cpp"
	"source/trace/structureSignature.cpp"
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
	PUBLIC
	${Boost_INCLUDE_DIRS}
	${Snappy_INCLUDE_DIRS}
	${Sodium_INCLUDE_DIRS}
	${JsonCpp_INCLUDE_DIRS})

target_link_libraries(libglurg PUBLIC
	${Boost_LIBRARIES} ${Boost_LDFLAGS} ${Boost_iostreams_LIBRARY}
	${Snappy_LIBRARIES} ${Snappy_LDFLAGS}
	${Sodium_LIBRARIES} ${Sodium_LDFLAGS}
	${JsonCpp_LIBRARIES} ${JsonCpp_LDFLAGS})

set_property(TARGET libglurg PROPERTY CXX_STANDARD 14)