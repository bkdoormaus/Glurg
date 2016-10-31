// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstdint>
#include <vector>
#include <sol.hpp>
#include <glm/vec4.hpp>
#include "glurg/common/lua.hpp"
#include "glurg/common/fileStream.hpp"
#include "glurg/common/hash.hpp"
#include "glurg/common/pixelData.hpp"
#include "glurg/common/process.hpp"
#include "glurg/common/simpleFileStream.hpp"
#include "glurg/common/snappyAdapter.hpp"

typedef std::vector<std::uint8_t> DataBuffer;
const std::uint8_t* get_data_buffer_data(
	const DataBuffer& buffer)
{
	return &buffer.at(0);
}

const std::uint8_t* slice_data(
	const std::uint8_t* buffer, std::size_t offset)
{
	return buffer + offset;
}

void glurg::lua::export_common(lua_State* L)
{
	sol::table glurg = sol::stack::get<sol::table>(L, -1);

	sol::table common = glurg.create_named("common");
	common["slice_data"] = &slice_data;
	common["hash_data"] = &glurg::Hash::hash;
	common["file_mode_read"] = glurg::FileStream::mode_read;
	common["file_mode_write"] = glurg::FileStream::mode_write;
	common["process_mode_read"] = glurg::Process::mode_read;
	common["process_mode_write"] = glurg::Process::mode_write;
	common["pixel_data_buffer_to_png"] = &glurg::PixelData::to_png;
	common.new_usertype<glurg::FileStream>(
		"FileStream",
		"write", &glurg::FileStream::write,
		"read", &glurg::FileStream::read,
		"position", sol::property(&glurg::FileStream::get_position, &glurg::FileStream::set_position),
		"is_end_of_file", sol::readonly_property(&glurg::FileStream::get_is_end_of_file),
		"get_mode", sol::readonly_property(&glurg::FileStream::get_mode),
		"close", &glurg::FileStream::close);
	common.new_usertype<glurg::Hash>(
		"Hash",
		"to_string", &glurg::Hash::to_string,
		"get_bytes", &glurg::Hash::get_bytes);
	common.new_usertype<glm::ivec4>(
		"Pixel",
		sol::constructors<sol::types<>>(),
		"r", &glm::ivec4::r,
		"g", &glm::ivec4::g,
		"b", &glm::ivec4::b,
		"a", &glm::ivec4::a);
	common.new_usertype<glm::vec4>(
		"Vector4",
		sol::constructors<sol::types<>>(),
		"x", &glm::vec4::x,
		"y", &glm::vec4::y,
		"z", &glm::vec4::z,
		"w", &glm::vec4::w);
	common.new_usertype<DataBuffer>(
		"DataBuffer",
		sol::constructors<sol::types<>>(),
		"data", sol::readonly_property(&get_data_buffer_data),
		"size", sol::readonly_property(&DataBuffer::size));
	common.new_usertype<glurg::Process>(
		"Process",
		sol::constructors<sol::types<>>(),
		"program_name", sol::property(&glurg::Process::get_program_name, &glurg::Process::set_program_name),
		"num_arguments", sol::readonly_property(&glurg::Process::get_num_program_arguments),
		"get_program_argument_at", &glurg::Process::get_program_argument_at,
		"set_program_argument_at", &glurg::Process::set_program_argument_at,
		"remove_program_argument_at", &glurg::Process::remove_program_argument_at,
		"push_program_argument", &glurg::Process::push_program_argument,
		"open", &glurg::Process::open,
		"close", &glurg::Process::close,
		"is_open", &glurg::Process::get_is_open,
		"stream", sol::readonly_property(&glurg::Process::get_stream));
	common.new_usertype<glurg::SimpleFileStream>(
		"SimpleFileStream",
		sol::constructors<sol::types<>>(),
		"open", &glurg::SimpleFileStream::open,
		sol::base_classes, sol::bases<glurg::FileStream>());
	typedef glurg::SnappyAdapter<glurg::SimpleFileStream> SnappyFileStream;
	common.new_usertype<SnappyFileStream>(
		"SnappyFileStream",
		sol::constructors<sol::types<>>(),
		sol::base_classes, sol::bases<glurg::SimpleFileStream, glurg::FileStream>());
}
