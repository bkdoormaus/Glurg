// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <sol.hpp>
#include <glm/vec4.hpp>
#include "glurg/common/lua.hpp"
#include "glurg/common/fileStream.hpp"
#include "glurg/common/hash.hpp"
#include "glurg/common/pixelData.hpp"
#include "glurg/common/process.hpp"
#include "glurg/common/simpleFileStream.hpp"
#include "glurg/common/snappyAdapter.hpp"
#include "glurg/resources/renderState.hpp"
#include "glurg/resources/resourceBlob.hpp"
#include "glurg/resources/texture/textureResource.hpp"
#include "glurg/resources/texture/textureResourceBlob.hpp"
#include "glurg/resources/texture/textureResourceBlobBuilder.hpp"
#include "glurg/trace/bitmaskSignature.hpp"
#include "glurg/trace/call.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/enumerationSignature.hpp"
#include "glurg/trace/events.hpp"
#include "glurg/trace/structure.hpp"
#include "glurg/trace/structureSignature.hpp"
#include "glurg/trace/traceFile.hpp"
#include "glurg/trace/values.hpp"

const std::uint8_t* get_pixel_data_buffer_data(
	const glurg::PixelDataBuffer& buffer)
{
	return &buffer.at(0);
}

extern "C" int luaopen_glurg(lua_State* L)
{
	sol::state_view state(L);
	sol::table glurg = state.create_table();

	sol::table common = glurg.create_named("common");
	common["hash_data"] = &glurg::Hash::hash;
	common["file_mode_read"] = glurg::FileStream::mode_read;
	common["file_mode_write"] = glurg::FileStream::mode_write;
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
		"r", &glm::ivec4::x,
		"g", &glm::ivec4::y,
		"b", &glm::ivec4::z,
		"a", &glm::ivec4::a);
	common.new_usertype<glurg::PixelDataBuffer>(
		"PixelDataBuffer",
		sol::constructors<sol::types<>>(),
		"data", sol::readonly_property(&get_pixel_data_buffer_data),
		"size", sol::readonly_property(&glurg::PixelDataBuffer::size));
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
		"stream", &glurg::Process::get_stream);
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

	sol::table resources = glurg.create_named("resources");
	resources["is_compatible_texture_blob"] = &glurg::TextureResource::is_compatible_texture_blob;
	resources.new_usertype<glurg::PixelComponentDescription>(
		"PixelComponentDescription",
		sol::constructors<sol::types<>>(),
		"swizzle", &glurg::PixelComponentDescription::swizzle,
		"storage", &glurg::PixelComponentDescription::storage,
		"bit_size", &glurg::PixelComponentDescription::bit_size);
	resources.new_usertype<glurg::RenderState>(
		"RenderState",
		sol::constructors<sol::types<>>(),
		"get", &glurg::RenderState::get,
		"populate", &glurg::RenderState::populate<glurg::Process::Stream>);
	resources.new_usertype<glurg::Resource>(
		"Resource",
		"blob", sol::readonly_property(&glurg::Resource::get_blob));
	resources.new_usertype<glurg::TextureResource>(
		"Texture",
		sol::constructors<sol::types<const glurg::TextureResourceBlob*>>(),
		"fetch_pixel", &glurg::TextureResource::fetch_pixel,
		"decode_image", &glurg::TextureResource::decode_image,
		"blob", sol::readonly_property(&glurg::Resource::get_blob));
	resources.new_usertype<glurg::TextureResourceBlob>(
		"TextureBlob",
		"red_description", sol::readonly_property(&glurg::TextureResourceBlob::get_red_description),
		"green_description", sol::readonly_property(&glurg::TextureResourceBlob::get_green_description),
		"blue_description", sol::readonly_property(&glurg::TextureResourceBlob::get_blue_description),
		"alpha_description", sol::readonly_property(&glurg::TextureResourceBlob::get_alpha_description),
		"get_wrap_mode", &glurg::TextureResourceBlob::get_wrap_mode,
		"type", sol::readonly_property(&glurg::TextureResourceBlob::get_texture_type),
		"minification_filter", sol::readonly_property(&glurg::TextureResourceBlob::get_minification_filter),
		"magnification_filter", sol::readonly_property(&glurg::TextureResourceBlob::get_magnification_filter),
		"width", sol::readonly_property(&glurg::TextureResourceBlob::get_width),
		"height", sol::readonly_property(&glurg::TextureResourceBlob::get_height),
		"depth", sol::readonly_property(&glurg::TextureResourceBlob::get_depth),
		"mipmap_level", sol::readonly_property(&glurg::TextureResourceBlob::get_mipmap_level),
		"pixels", sol::readonly_property(&glurg::TextureResourceBlob::get_pixels),
		"pixels_size", sol::readonly_property(&glurg::TextureResourceBlob::get_pixels_size),
		"hash", sol::readonly_property(&glurg::TextureResourceBlob::get_hash),
		"data", sol::readonly_property(&glurg::TextureResourceBlob::get_data),
		"size", sol::readonly_property(&glurg::TextureResourceBlob::get_size));
	resources.new_usertype<glurg::TextureResourceBlobBuilder>(
		"TextureBlobBuilder",
		"set_red_description", &glurg::TextureResourceBlobBuilder::set_red_description,
		"set_green_description", &glurg::TextureResourceBlobBuilder::set_green_description,
		"set_blue_description", &glurg::TextureResourceBlobBuilder::set_blue_description,
		"set_alpha_description", &glurg::TextureResourceBlobBuilder::set_alpha_description,
		"set_wrap_mode", &glurg::TextureResourceBlobBuilder::set_wrap_mode,
		"set_texture_type", &glurg::TextureResourceBlobBuilder::set_texture_type,
		"set_minification_filter", &glurg::TextureResourceBlobBuilder::set_minification_filter,
		"set_magnification_filter", &glurg::TextureResourceBlobBuilder::set_magnification_filter,
		"set_width", &glurg::TextureResourceBlobBuilder::set_width,
		"set_height", &glurg::TextureResourceBlobBuilder::set_height,
		"set_depth", &glurg::TextureResourceBlobBuilder::set_depth,
		"set_mipmap_level", &glurg::TextureResourceBlobBuilder::set_mipmap_level,
		"set_pixel_data", &glurg::TextureResourceBlobBuilder::set_pixel_data,
		"set_binding_point", &glurg::TextureResourceBlobBuilder::set_binding_point,
		"build", &glurg::TextureResourceBlobBuilder::build,
		"extract_from_state", (bool (glurg::TextureResourceBlobBuilder::*)(const glurg::RenderState&))(&glurg::TextureResourceBlobBuilder::extract),
		"extract_from_call_args", (bool (glurg::TextureResourceBlobBuilder::*)(GLenum, GLenum, const std::uint8_t*))(&glurg::TextureResourceBlobBuilder::extract));
	sol::table texture = resources.create_named("texture");
	texture["swizzle_red"] = glurg::PixelComponentDescription::swizzle_red;
	texture["swizzle_green"] = glurg::PixelComponentDescription::swizzle_green;
	texture["swizzle_blue"] = glurg::PixelComponentDescription::swizzle_blue;
	texture["swizzle_alpha"] = glurg::PixelComponentDescription::swizzle_alpha;
	texture["swizzle_zero"] = glurg::PixelComponentDescription::swizzle_zero;
	texture["swizzle_one"] = glurg::PixelComponentDescription::swizzle_one;
	texture["storage_none"] = glurg::PixelComponentDescription::storage_none;
	texture["storage_disabled"] = glurg::PixelComponentDescription::storage_disabled;
	texture["storage_signed_normalized"] = glurg::PixelComponentDescription::storage_signed_normalized;
	texture["storage_unsigned_normalized"] = glurg::PixelComponentDescription::storage_unsigned_normalized;
	texture["storage_float"] = glurg::PixelComponentDescription::storage_float;
	texture["storage_signed_integral"] = glurg::PixelComponentDescription::storage_signed_integral;
	texture["storage_unsigned_integral"] = glurg::PixelComponentDescription::storage_unsigned_integral;
	texture["wrap_none"] = glurg::TextureResourceBlob::wrap_none;
	texture["wrap_border_clamp"] = glurg::TextureResourceBlob::wrap_border_clamp;
	texture["wrap_edge_clamp"] = glurg::TextureResourceBlob::wrap_edge_clamp;
	texture["wrap_repeat"] = glurg::TextureResourceBlob::wrap_repeat;
	texture["wrap_mirror_edge_clamp"] = glurg::TextureResourceBlob::wrap_mirror_edge_clamp;
	texture["wrap_mirror_repeat"] = glurg::TextureResourceBlob::wrap_mirror_repeat;
	texture["type_none"] = glurg::TextureResourceBlob::type_none;
	texture["type_1d"] = glurg::TextureResourceBlob::type_1d;
	texture["type_2d"] = glurg::TextureResourceBlob::type_2d;
	texture["type_3d"] = glurg::TextureResourceBlob::type_3d;
	texture["type_cube_map_positive_x"] = glurg::TextureResourceBlob::type_cube_map_positive_x;
	texture["type_cube_map_negative_x"] = glurg::TextureResourceBlob::type_cube_map_negative_x;
	texture["type_cube_map_positive_y"] = glurg::TextureResourceBlob::type_cube_map_positive_y;
	texture["type_cube_map_negative_y"] = glurg::TextureResourceBlob::type_cube_map_negative_y;
	texture["type_cube_map_positive_z"] = glurg::TextureResourceBlob::type_cube_map_positive_z;
	texture["type_cube_map_negative_z"] = glurg::TextureResourceBlob::type_cube_map_negative_z;
	texture["type_1d_array"] = glurg::TextureResourceBlob::type_1d_array;
	texture["type_2d_array"] = glurg::TextureResourceBlob::type_2d_array;
	texture["filter_none"] = glurg::TextureResourceBlob::filter_none;
	texture["filter_nearest"] = glurg::TextureResourceBlob::filter_nearest;
	texture["filter_linear"] = glurg::TextureResourceBlob::filter_linear;
	texture["filter_nearest_mipmap_nearest"] = glurg::TextureResourceBlob::filter_nearest_mipmap_nearest;
	texture["filter_linear_mipmap_nearest"] = glurg::TextureResourceBlob::filter_linear_mipmap_nearest;
	texture["filter_nearest_mipmap_linear"] = glurg::TextureResourceBlob::filter_nearest_mipmap_linear;
	texture["filter_linear_mipmap_linear"] = glurg::TextureResourceBlob::filter_linear_mipmap_linear;

	sol::table trace = glurg.create_named("trace");
	trace["read_event"] = &glurg::trace::Event::read;
	trace["event_enter"] = glurg::trace::Event::event_enter;
	trace["event_leave"] = glurg::trace::Event::event_leave;
	trace.new_usertype<glurg::trace::BitmaskSignature>(
		"BitmaskSignature",
		"id", sol::readonly_property(&glurg::trace::BitmaskSignature::get_id),
		"match", &glurg::trace::BitmaskSignature::match);
	trace.new_usertype<glurg::trace::Call>(
		"Call",
		"index", sol::readonly_property(&glurg::trace::Call::get_call_index),
		"signature", sol::readonly_property(&glurg::trace::Call::get_call_signature),
		"thread", sol::readonly_property(&glurg::trace::Call::get_thread),
		"result", sol::readonly_property(&glurg::trace::Call::get_return_value),
		"get_argument_at", &glurg::trace::Call::get_argument_at,
		"set_argument_at", &glurg::trace::Call::set_argument_at);
	trace.new_usertype<glurg::trace::CallSignature>(
		"CallSignature",
		"id", sol::readonly_property(&glurg::trace::CallSignature::get_id),
		"name", sol::readonly_property(&glurg::trace::CallSignature::get_name),
		"num_parameters", sol::readonly_property(&glurg::trace::CallSignature::get_num_parameters),
		"get_parameter_at", &glurg::trace::CallSignature::get_parameter_at);
	trace.new_usertype<glurg::trace::EnumerationSignature>(
		"EnumerationSignature",
		"id", sol::readonly_property(&glurg::trace::EnumerationSignature::get_id),
		"get_value_by_name", &glurg::trace::EnumerationSignature::get_value_by_name);
	trace.new_usertype<glurg::trace::Event>(
		"Event",
		"type", sol::readonly_property(&glurg::trace::Event::get_type),
		"call_index", sol::readonly_property(&glurg::trace::Event::get_call_index));
	trace.new_usertype<glurg::trace::Structure>(
		"Structure",
		"signature", sol::readonly_property(&glurg::trace::Structure::get_signature),
		"get_field", &glurg::trace::Structure::get_field_by_name);
	trace.new_usertype<glurg::trace::StructureSignature>(
		"StructureSignature",
		"id", sol::readonly_property(&glurg::trace::StructureSignature::get_id),
		"name", sol::readonly_property(&glurg::trace::StructureSignature::get_name),
		"num_fields", sol::readonly_property(&glurg::trace::StructureSignature::get_num_fields),
		"get_field_name", &glurg::trace::StructureSignature::get_field_name,
		"has_field_name", &glurg::trace::StructureSignature::has_field_name);
	trace.new_usertype<glurg::trace::TraceFile>(
		"TraceFile",
		sol::constructors<sol::types<>>(),
		"get_call", (glurg::trace::Call* (glurg::trace::TraceFile::*)(glurg::trace::Call::Index))(&glurg::trace::TraceFile::get_call),
		"delete_call", &glurg::trace::TraceFile::delete_call,
		"verify_is_compatible_version", &glurg::trace::TraceFile::verify_is_compatible_version);
	trace.new_usertype<glurg::trace::Enumeration>(
		"Enumeration",
		"signature", sol::readonly(&glurg::trace::Enumeration::signature),
		"value", sol::readonly(&glurg::trace::Enumeration::value));
	trace.new_usertype<glurg::trace::Bitmask>(
		"Bitmask",
		"signature", sol::readonly(&glurg::trace::Bitmask::signature),
		"value", sol::readonly(&glurg::trace::Bitmask::value));
	trace.new_usertype<glurg::trace::Blob>(
		"Blob",
		"data", sol::readonly(&glurg::trace::Blob::data),
		"length", sol::readonly(&glurg::trace::Blob::length));
	trace.new_usertype<glurg::trace::Value>(
		"Value",
		"type", sol::readonly_property(&glurg::trace::Value::get_type),
		"to_boolean", &glurg::trace::Value::to_boolean,
		"to_integer", &glurg::trace::Value::to_signed_integer,
		"to_number", &glurg::trace::Value::to_double,
		"to_string", &glurg::trace::Value::to_string,
		"to_blob", &glurg::trace::Value::to_blob,
		"to_handle", &glurg::trace::Value::to_handle,
		"to_enumeration", &glurg::trace::Value::to_enumeration,
		"to_bitmask", &glurg::trace::Value::to_bitmask,
		"to_array", &glurg::trace::Value::to_array,
		"to_structure", &glurg::trace::Value::to_structure,
		"to_handle", &glurg::trace::Value::to_structure,
		"clone", &glurg::trace::Value::clone);
	trace.new_usertype<glurg::trace::Array>(
		"Array",
		"size", sol::readonly_property(&glurg::trace::Array::get_size),
		"get_value_at", &glurg::trace::Array::get_value_at);

	sol::table values = trace.create_named("values");
	values["false_boolean"] = glurg::trace::BoolValue::FALSE_BOOLEAN;
	values["true_boolean"] = glurg::trace::BoolValue::TRUE_BOOLEAN;
	values["negative_integer"] = glurg::trace::IntegerValue::NEGATIVE_NUMBER;
	values["positive_integer"] = glurg::trace::IntegerValue::POSITIVE_NUMBER;
	values["float"] = glurg::trace::FloatingPointValue::FLOAT;
	values["double"] = glurg::trace::FloatingPointValue::DOUBLE;
	values["string"] = glurg::trace::StringValue::STRING;
	values["null_pointer"] = glurg::trace::BlobValue::NULL_POINTER;
	values["blob"] = glurg::trace::BlobValue::BLOB;
	values["enumeration"] = glurg::trace::EnumerationValue::ENUMERATION;
	values["bitmask"] = glurg::trace::BitmaskValue::BITMASK;
	values["array"] = glurg::trace::ArrayValue::ARRAY;
	values["structure"] = glurg::trace::StructureValue::STRUCTURE;
	values["handle"] = glurg::trace::HandleValue::HANDLE;

	sol::stack::push(L, glurg);

	return 1;
}
