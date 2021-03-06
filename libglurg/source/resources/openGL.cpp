// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/openGL.hpp"
#include "glurg/resources/texture/textureResourceBlob.hpp"

struct ValueMap
{
	GLenum gl_enum_value;
	const char* gl_string_value;
	int result;

	static bool lookup(
		const ValueMap map[], std::size_t count,
		GLenum value, int& result);
	static bool lookup(
		const ValueMap map[], std::size_t count,
		const std::string& value, int& result);

	static bool reverse_lookup(
		const ValueMap map[], std::size_t count,
		int glurg_value, GLenum& result);
	static bool reverse_lookup(
		const ValueMap map[], std::size_t count,
		int glurg_value, const char*& result);
};

bool ValueMap::lookup(
	const ValueMap map[], std::size_t count,
	GLenum value, int& result)
{
	for (std::size_t i = 0; i < count; ++i)
	{
		if (value == map[i].gl_enum_value)
		{
			result = map[i].result;
			return true;
		}
	}

	return false;
}

bool ValueMap::lookup(
	const ValueMap map[], std::size_t count,
	const std::string& value, int& result)
{
	for (std::size_t i = 0; i < count; ++i)
	{
		if (value == map[i].gl_string_value)
		{
			result = map[i].result;
			return true;
		}
	}

	return false;
}

bool ValueMap::reverse_lookup(
	const ValueMap map[], std::size_t count,
	int glurg_value, GLenum& result)
{
	for (std::size_t i = 0; i < count; ++i)
	{
		if (glurg_value == map[i].result)
		{
			result = map[i].gl_enum_value;
			return true;
		}
	}

	return false;
}

bool ValueMap::reverse_lookup(
	const ValueMap map[], std::size_t count,
	int glurg_value, const char*& result)
{
	for (std::size_t i = 0; i < count; ++i)
	{
		if (glurg_value == map[i].result)
		{
			result = map[i].gl_string_value;
			return true;
		}
	}

	return false;
}

template <typename Value>
int glurg_perform_lookup(
	const ValueMap* map, std::size_t size,
	const Value& value,
	const char* error_message);

template <>
int glurg_perform_lookup<GLenum>(
	const ValueMap* map, std::size_t size,
	const GLenum& value,
	const char* error_message)
{
	int result;
	if (!ValueMap::lookup(map, size, value, result))
	{
		throw std::runtime_error(error_message);
	}

	return result;
}

template <>
int glurg_perform_lookup<std::string>(
	const ValueMap* map, std::size_t size,
	const std::string& value,
	const char* error_message)
{
	int result;
	if (!ValueMap::lookup(map, size, value, result))
	{
		throw std::runtime_error(error_message);
	}

	return result;
}

template <typename Value>
Value glurg_perform_reverse_lookup(
	const ValueMap* map, std::size_t size,
	int value,
	const char* error_message);

template <>
GLenum glurg_perform_reverse_lookup<GLenum>(
	const ValueMap* map, std::size_t size,
	int value,
	const char* error_message)
{
	GLenum result;
	if (!ValueMap::reverse_lookup(map, size, value, result))
	{
		throw std::runtime_error(error_message);
	}

	return result;
}

template <>
const char* glurg_perform_reverse_lookup<const char*>(
	const ValueMap* map, std::size_t size,
	int value,
	const char* error_message)
{
	const char* result;
	if (!ValueMap::reverse_lookup(map, size, value, result))
	{
		throw std::runtime_error(error_message);
	}

	return result;
}

#define GLURG_GET_SIZE(map) (sizeof(map) / sizeof(ValueMap))
#define GLURG_EMIT_ENTRY(gl_enum, glurg_value) { gl_enum, #gl_enum, glurg_value }

const ValueMap glurg_swizzle_mode[] =
{
	GLURG_EMIT_ENTRY(GL_RED, glurg::PixelComponentDescription::swizzle_red),
	GLURG_EMIT_ENTRY(GL_GREEN, glurg::PixelComponentDescription::swizzle_green),
	GLURG_EMIT_ENTRY(GL_BLUE, glurg::PixelComponentDescription::swizzle_blue),
	GLURG_EMIT_ENTRY(GL_ALPHA, glurg::PixelComponentDescription::swizzle_alpha),
	GLURG_EMIT_ENTRY(GL_ZERO, glurg::PixelComponentDescription::swizzle_zero),
	GLURG_EMIT_ENTRY(GL_ONE, glurg::PixelComponentDescription::swizzle_one)
};

int glurg::gl::to_pixel_component_description_swizzle_mode(GLenum value)
{
	return glurg_perform_lookup(
		glurg_swizzle_mode, GLURG_GET_SIZE(glurg_swizzle_mode),
		value,
		"unrecognized OpenGL swizzle mode");
}

int glurg::gl::to_pixel_component_description_swizzle_mode(
	const std::string& value)
{
	return glurg_perform_lookup(
		glurg_swizzle_mode, GLURG_GET_SIZE(glurg_swizzle_mode),
		value,
		"unrecognized OpenGL swizzle mode");
}

const ValueMap glurg_storage_type[] =
{
	GLURG_EMIT_ENTRY(GL_NONE,
		glurg::PixelComponentDescription::storage_disabled),
	GLURG_EMIT_ENTRY(GL_SIGNED_NORMALIZED,
		glurg::PixelComponentDescription::storage_signed_normalized),
	GLURG_EMIT_ENTRY(GL_UNSIGNED_NORMALIZED,
		glurg::PixelComponentDescription::storage_unsigned_normalized),
	GLURG_EMIT_ENTRY(GL_FLOAT, glurg::PixelComponentDescription::storage_float),
	GLURG_EMIT_ENTRY(GL_INT,
		glurg::PixelComponentDescription::storage_signed_integral),
	GLURG_EMIT_ENTRY(GL_UNSIGNED_INT,
		glurg::PixelComponentDescription::storage_unsigned_integral)
};

int glurg::gl::to_pixel_component_description_storage_type(GLenum value)
{
	return glurg_perform_lookup(
		glurg_storage_type, GLURG_GET_SIZE(glurg_storage_type),
		value, "unrecognized OpenGL storage mode");
}

int glurg::gl::to_pixel_component_description_storage_type(
	const std::string& value)
{
	return glurg_perform_lookup(
		glurg_storage_type, GLURG_GET_SIZE(glurg_storage_type),
		value, "unrecognized OpenGL storage mode");
}

const ValueMap glurg_texture_type[] =
{
	GLURG_EMIT_ENTRY(GL_TEXTURE_1D, glurg::TextureResourceBlob::type_1d),
	GLURG_EMIT_ENTRY(GL_TEXTURE_2D, glurg::TextureResourceBlob::type_2d),
	GLURG_EMIT_ENTRY(GL_TEXTURE_3D, glurg::TextureResourceBlob::type_3d),
	GLURG_EMIT_ENTRY(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		glurg::TextureResourceBlob::type_cube_map_positive_x),
	GLURG_EMIT_ENTRY(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		glurg::TextureResourceBlob::type_cube_map_negative_x),
	GLURG_EMIT_ENTRY(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		glurg::TextureResourceBlob::type_cube_map_positive_y),
	GLURG_EMIT_ENTRY(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		glurg::TextureResourceBlob::type_cube_map_negative_y),
	GLURG_EMIT_ENTRY(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		glurg::TextureResourceBlob::type_cube_map_positive_z),
	GLURG_EMIT_ENTRY(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		glurg::TextureResourceBlob::type_cube_map_negative_z),
	GLURG_EMIT_ENTRY(GL_TEXTURE_1D_ARRAY,
		glurg::TextureResourceBlob::type_1d_array),
	GLURG_EMIT_ENTRY(GL_TEXTURE_2D_ARRAY,
		glurg::TextureResourceBlob::type_2d_array)
};

int glurg::gl::to_texture_blob_texture_type(GLenum value)
{
	return glurg_perform_lookup(
		glurg_texture_type, GLURG_GET_SIZE(glurg_texture_type),
		value, "unrecognized OpenGL texture type");
}

int glurg::gl::to_texture_blob_texture_type(const std::string& value)
{
	return glurg_perform_lookup(
		glurg_texture_type, GLURG_GET_SIZE(glurg_texture_type),
		value, "unrecognized OpenGL texture type");
}

GLenum glurg::gl::enum_from_texture_blob_texture_type(int value)
{
	return glurg_perform_reverse_lookup<GLenum>(
		glurg_texture_type, GLURG_GET_SIZE(glurg_texture_type),
		value, "invalid texture type");
}

const char* glurg::gl::string_from_texture_blob_texture_type(int value)
{
	return glurg_perform_reverse_lookup<const char*>(
		glurg_texture_type, GLURG_GET_SIZE(glurg_texture_type),
		value, "invalid texture type");
}

const ValueMap glurg_wrap_mode[] =
{
	GLURG_EMIT_ENTRY(GL_CLAMP_TO_EDGE,
		glurg::TextureResourceBlob::wrap_edge_clamp),
	GLURG_EMIT_ENTRY(GL_CLAMP_TO_BORDER,
		glurg::TextureResourceBlob::wrap_border_clamp),
	GLURG_EMIT_ENTRY(GL_REPEAT,
		glurg::TextureResourceBlob::wrap_repeat),
	GLURG_EMIT_ENTRY(GL_MIRROR_CLAMP_TO_EDGE,
		glurg::TextureResourceBlob::wrap_mirror_edge_clamp),
	GLURG_EMIT_ENTRY(GL_MIRRORED_REPEAT,
		glurg::TextureResourceBlob::wrap_mirror_repeat)
};

int glurg::gl::to_texture_blob_wrap_mode(GLenum value)
{
	return glurg_perform_lookup(
		glurg_wrap_mode, GLURG_GET_SIZE(glurg_wrap_mode),
		value, "unrecognized OpenGL wrap mode");
}

int glurg::gl::to_texture_blob_wrap_mode(const std::string& value)
{
	
	return glurg_perform_lookup(
		glurg_wrap_mode, GLURG_GET_SIZE(glurg_wrap_mode),
		value, "unrecognized OpenGL wrap mode");
}

const ValueMap glurg_zoom_filter[] =
{
	GLURG_EMIT_ENTRY(GL_NEAREST, glurg::TextureResourceBlob::filter_nearest),
	GLURG_EMIT_ENTRY(GL_LINEAR, glurg::TextureResourceBlob::filter_linear),
	GLURG_EMIT_ENTRY(GL_NEAREST_MIPMAP_NEAREST,
		glurg::TextureResourceBlob::filter_nearest_mipmap_nearest),
	GLURG_EMIT_ENTRY(GL_LINEAR_MIPMAP_NEAREST,
		glurg::TextureResourceBlob::filter_linear_mipmap_nearest),
	GLURG_EMIT_ENTRY(GL_NEAREST_MIPMAP_LINEAR,
		glurg::TextureResourceBlob::filter_nearest_mipmap_linear),
	GLURG_EMIT_ENTRY(GL_LINEAR_MIPMAP_LINEAR,
		glurg::TextureResourceBlob::filter_linear_mipmap_linear)
};

int glurg::gl::to_texture_blob_zoom_filter(GLenum value)
{
	return glurg_perform_lookup(
		glurg_zoom_filter, GLURG_GET_SIZE(glurg_zoom_filter),
		value, "unrecognized OpenGL zoom filter");
}

int glurg::gl::to_texture_blob_zoom_filter(const std::string& value)
{
	return glurg_perform_lookup(
		glurg_zoom_filter, GLURG_GET_SIZE(glurg_zoom_filter),
		value, "unrecognized OpenGL zoom filter");
}
