cmake_minimum_required(VERSION 3.6)
project(glurg)

find_package(PkgConfig REQUIRED)

find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(Snappy REQUIRED)
find_package(PNG REQUIRED)
find_package(glm REQUIRED)
find_package(Lua51 REQUIRED)
find_package(assimp REQUIRED)
pkg_search_module(Sodium REQUIRED libsodium)
pkg_search_module(JsonCpp REQUIRED jsoncpp)

set(LIBGLURG_INCLUDE_FILES
	"include/glurg/common/base64.hpp"
	"include/glurg/common/fileStream.hpp"
	"include/glurg/common/hash.hpp"
	"include/glurg/common/lua.hpp"
	"include/glurg/common/pixelData.hpp"
	"include/glurg/common/process.hpp"
	"include/glurg/common/simpleFileStream.hpp"
	"include/glurg/common/snappyAdapter.hpp"
	"include/glurg/resources/openGL.hpp"
	"include/glurg/resources/renderState.hpp"
	"include/glurg/resources/renderValue.hpp"
	"include/glurg/resources/resource.hpp"
	"include/glurg/resources/resourceBlob.hpp"
	"include/glurg/resources/resourceBlobBuffer.hpp"
	"include/glurg/resources/resourceBlobBuilder.hpp"
	"include/glurg/resources/mesh/meshResource.hpp"
	"include/glurg/resources/mesh/meshResourceBlob.hpp"
	"include/glurg/resources/mesh/meshResourceBlobBuilder.hpp"
	"include/glurg/resources/mesh/model.hpp"
	"include/glurg/resources/texture/pixelUnpacker.hpp"
	"include/glurg/resources/texture/textureResource.hpp"
	"include/glurg/resources/texture/textureResourceBlob.hpp"
	"include/glurg/resources/texture/textureResourceBlobBuilder.hpp"
	"include/glurg/trace/bitmaskSignature.hpp"
	"include/glurg/trace/callSignature.hpp"
	"include/glurg/trace/enumerationSignature.hpp"
	"include/glurg/trace/structure.hpp"
	"include/glurg/trace/structureSignature.hpp"
	"include/glurg/trace/traceFile.hpp"
	"source/resources/texture/textureCompression.h")

set(LIBGLURG_SOURCE_FILES
	"source/common/base64.cpp"
	"source/common/hash.cpp"
	"source/common/lua.cpp"
	"source/common/lua/common.cpp"
	"source/common/lua/resources.cpp"
	"source/common/lua/trace.cpp"
	"source/common/pixelData.cpp"
	"source/common/process.cpp"
	"source/common/simpleFileStream.cpp"
	"source/common/snappyAdapter.cpp"
	"source/resources/openGL.cpp"
	"source/resources/renderState.cpp"
	"source/resources/renderValue.cpp"
	"source/resources/resourceBlobBuffer.cpp"
	"source/resources/mesh/meshResource.cpp"
	"source/resources/mesh/meshResourceBlob.cpp"
	"source/resources/mesh/meshResourceBlobBuilder.cpp"
	"source/resources/mesh/model.cpp"
	"source/resources/mesh/modelExporter.cpp"
	"source/resources/texture/pixelUnpacker.cpp"
	"source/resources/texture/textureCompression.c"
	"source/resources/texture/textureResource.cpp"
	"source/resources/texture/textureResourceBlob.cpp"
	"source/resources/texture/textureResourceBlobBuilder.cpp"
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

add_library(glurg SHARED
	${LIBGLURG_INCLUDE_FILES}
	${LIBGLURG_SOURCE_FILES})

set_target_properties(glurg PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(glurg
	PUBLIC
	${LIBGLURG_INCLUDE_DIRS})

target_include_directories(glurg
	PUBLIC
	${Boost_INCLUDE_DIRS}
	${Snappy_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${GLM_INCLUDES_DIRS}
	${LUA_INCLUDE_DIR}
	${ASSIMP_INCLUDE_DIRS}
	${Sodium_INCLUDE_DIRS}
	${JsonCpp_INCLUDE_DIRS})

target_link_libraries(glurg PUBLIC
	${Boost_LIBRARIES} ${Boost_LDFLAGS} ${Boost_iostreams_LIBRARY}
	${Snappy_LIBRARIES} ${Snappy_LDFLAGS}
	${PNG_LIBRARIES}
	${LUA_LIBRARIES}
	${ASSIMP_LIBRARIES}
	${Sodium_LIBRARIES} ${Sodium_LDFLAGS}
	${JsonCpp_LIBRARIES} ${JsonCpp_LDFLAGS})

add_definitions(${PNG_DEFINITIONS})

set_property(TARGET glurg PROPERTY CXX_STANDARD 14)