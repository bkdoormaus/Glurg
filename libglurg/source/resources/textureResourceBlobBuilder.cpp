// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cassert>
#include <cstring>
#include "glurg/common/base64.hpp"
#include "glurg/common/pixelData.hpp"
#include "glurg/resources/openGL.hpp"
#include "glurg/resources/renderState.hpp"
#include "glurg/resources/renderValue.hpp"
#include "glurg/resources/textureResourceBlobBuilder.hpp"

glurg::TextureResourceBlobBuilder::TextureResourceBlobBuilder()
{
	this->texture_type = TextureResourceBlob::type_none;
	this->width = -1;
	this->height = -1;
	this->depth = -1;
	this->level = -1;

	red_component.storage = PixelComponentDescription::storage_none;
	green_component.storage = PixelComponentDescription::storage_none;
	blue_component.storage = PixelComponentDescription::storage_none;
	alpha_component.storage = PixelComponentDescription::storage_none;

	for (std::size_t i = 0; i < TextureResourceBlob::MAX_DIMENSIONS; ++i)
	{
		wrap_mode[i] = TextureResourceBlob::wrap_none;
	}

	minification_filter = TextureResourceBlob::filter_none;
	magnification_filter = TextureResourceBlob::filter_none;

	this->pixels = nullptr;
	this->pixels_size = 0;

	this->binding_point = -1;
}

glurg::TextureResourceBlobBuilder::~TextureResourceBlobBuilder()
{
	if (this->pixels)
	{
		delete[] this->pixels;
	}
}

void glurg::TextureResourceBlobBuilder::set_red_description(
	const PixelComponentDescription& value)
{
	this->red_component = value;
}

void glurg::TextureResourceBlobBuilder::set_green_description(
	const PixelComponentDescription& value)
{
	this->green_component = value;
}

void glurg::TextureResourceBlobBuilder::set_blue_description(
	const PixelComponentDescription& value)
{
	this->blue_component = value;
}

void glurg::TextureResourceBlobBuilder::set_alpha_description(
	const PixelComponentDescription& value)
{
	this->alpha_component = value;
}

void glurg::TextureResourceBlobBuilder::set_wrap_mode(
	std::size_t dimension, int value)
{
	if (dimension >= TextureResourceBlob::MAX_DIMENSIONS)
	{
		throw std::runtime_error("dimension index out-of-bounds");
	}

	this->wrap_mode[dimension] = value;
}

void glurg::TextureResourceBlobBuilder::set_texture_type(int value)
{
	this->texture_type = value;
}

void glurg::TextureResourceBlobBuilder::set_minification_filter(int filter)
{
	this->minification_filter = filter;
}

void glurg::TextureResourceBlobBuilder::set_magnification_filter(int filter)
{
	this->magnification_filter = filter;
}

void glurg::TextureResourceBlobBuilder::set_width(int value)
{
	if (value < 1)
	{
		throw std::runtime_error("width cannot be less than 1");
	}

	this->width = value;
}

void glurg::TextureResourceBlobBuilder::set_height(int value)
{
	if (value < 1)
	{
		throw std::runtime_error("height cannot be less than 1");
	}

	this->height = value;
}

void glurg::TextureResourceBlobBuilder::set_depth(int value)
{
	if (value < 1)
	{
		throw std::runtime_error("depth cannot be less than 1");
	}

	this->depth = value;
}

void glurg::TextureResourceBlobBuilder::set_mipmap_level(int value)
{
	if (value < 0)
	{
		throw std::runtime_error("mipmap level cannot be less than 0");
	}

	this->level = value;
}

void glurg::TextureResourceBlobBuilder::set_pixel_data(
	const std::uint8_t* pixels, std::size_t size)
{
	std::uint8_t* p = new std::uint8_t[size];
	std::memcpy(p, pixels, size);

	if (this->pixels != nullptr)
	{
		delete[] this->pixels;
	}

	this->pixels = p;
	this->pixels_size = size;
}

void glurg::TextureResourceBlobBuilder::set_binding_point(int value)
{
	if (value < 0)
	{
		throw std::runtime_error("binding point less than zero");
	}

	this->binding_point = value;
}

glurg::TextureResourceBlob* glurg::TextureResourceBlobBuilder::build()
{
	verify_state();

	ResourceBlobWriteBuffer buffer;

	buffer.push_value(this->texture_type);
	buffer.push_value(this->width);
	buffer.push_value(this->height);
	buffer.push_value(this->depth);
	buffer.push_value(this->level);

	write_pixel_component_description(this->red_component, buffer);
	write_pixel_component_description(this->green_component, buffer);
	write_pixel_component_description(this->blue_component, buffer);
	write_pixel_component_description(this->alpha_component, buffer);

	for (std::size_t i = 0; i < TextureResourceBlob::MAX_DIMENSIONS; ++i)
	{
		buffer.push_value(this->wrap_mode[i]);
	}

	buffer.push_value(this->minification_filter);
	buffer.push_value(this->magnification_filter);

	buffer.push_data(this->pixels, this->pixels_size);

	ResourceBlobReadBuffer final_buffer;
	buffer.to_read_buffer(final_buffer);

	return new TextureResourceBlob(std::move(final_buffer));
}

bool glurg::TextureResourceBlobBuilder::extract(const RenderState& state)
{
	verify_texture_type(this->texture_type);
	verify_mipmap_level(this->level);
	verify_binding_point(this->binding_point);
	auto texture_name = get_texture_key(
		this->binding_point,
		this->texture_type,
		this->level);

	auto textures = state.get("textures");
	if (!textures->has_field(texture_name))
	{
		return false;
	}

	auto texture_info = state.get("parameters")
		->get_field_by_name("GL_TEXTURE", this->binding_point)
		->get_field_by_name(
			gl::string_from_texture_blob_texture_type(this->texture_type));
	auto texture_data = textures->get_field_by_name(texture_name);

	extract_pixel_component_description(
		red_component,
		texture_info->get_field_by_name("GL_TEXTURE_SWIZZLE_R"),
		texture_info->get_field_by_name("GL_TEXTURE_RED_TYPE"),
		texture_info->get_field_by_name("GL_TEXTURE_RED_SIZE"));
	extract_pixel_component_description(
		green_component,
		texture_info->get_field_by_name("GL_TEXTURE_SWIZZLE_G"),
		texture_info->get_field_by_name("GL_TEXTURE_GREEN_TYPE"),
		texture_info->get_field_by_name("GL_TEXTURE_GREEN_SIZE"));
	extract_pixel_component_description(
		blue_component,
		texture_info->get_field_by_name("GL_TEXTURE_SWIZZLE_B"),
		texture_info->get_field_by_name("GL_TEXTURE_BLUE_TYPE"),
		texture_info->get_field_by_name("GL_TEXTURE_BLUE_SIZE"));
	extract_pixel_component_description(
		alpha_component,
		texture_info->get_field_by_name("GL_TEXTURE_SWIZZLE_A"),
		texture_info->get_field_by_name("GL_TEXTURE_ALPHA_TYPE"),
		texture_info->get_field_by_name("GL_TEXTURE_ALPHA_SIZE"));

	set_wrap_mode(0, gl::to_texture_blob_wrap_mode(
		texture_info->get_integer("GL_TEXTURE_WRAP_S")));
	set_wrap_mode(1, gl::to_texture_blob_wrap_mode(
		texture_info->get_integer("GL_TEXTURE_WRAP_T")));
	set_wrap_mode(2, gl::to_texture_blob_wrap_mode(
		texture_info->get_integer("GL_TEXTURE_WRAP_R")));

	set_minification_filter(gl::to_texture_blob_zoom_filter(
		texture_info->get_string("GL_TEXTURE_MIN_FILTER")));
	set_magnification_filter(gl::to_texture_blob_zoom_filter(
		texture_info->get_string("GL_TEXTURE_MAG_FILTER")));

	set_width(texture_info->get_integer("GL_TEXTURE_WIDTH"));
	set_height(texture_info->get_integer("GL_TEXTURE_HEIGHT"));
	set_depth(texture_info->get_integer("GL_TEXTURE_DEPTH"));

	PixelDataBuffer encoded_pixel_data;
	decode_base64(texture_data->get_string("__data__"), encoded_pixel_data);

	PixelDataBuffer raw_pixel_data;
	read_pixel_data(raw_pixel_data, encoded_pixel_data);

	set_pixel_data(&raw_pixel_data[0], raw_pixel_data.size());

	return true;
}

void glurg::TextureResourceBlobBuilder::write_pixel_component_description(
	const PixelComponentDescription& description,
	ResourceBlobWriteBuffer& buffer)
{
	buffer.push_value(description.swizzle);
	buffer.push_value(description.storage);
	buffer.push_value(description.bit_size);
}

void glurg::TextureResourceBlobBuilder::verify_state()
{
	verify_texture_type(this->texture_type);
	verify_dimensions(this->width, this->height, this->depth);
	verify_mipmap_level(this->level);

	verify_pixel_component_description(this->red_component);
	verify_pixel_component_description(this->green_component);
	verify_pixel_component_description(this->blue_component);
	verify_pixel_component_description(this->alpha_component);

	verify_wrap_modes(
		this->wrap_mode, TextureResourceBlob::MAX_DIMENSIONS,
		this->texture_type);
	verify_zoom_filters(this->minification_filter, this->magnification_filter);

	verify_pixel_data(this->pixels, this->pixels_size);
}

void glurg::TextureResourceBlobBuilder::verify_texture_type(int texture_type)
{
	switch (texture_type)
	{
		case TextureResourceBlob::type_1d:
		case TextureResourceBlob::type_2d:
		case TextureResourceBlob::type_3d:
		case TextureResourceBlob::type_cube_map_positive_x:
		case TextureResourceBlob::type_cube_map_negative_x:
		case TextureResourceBlob::type_cube_map_positive_y:
		case TextureResourceBlob::type_cube_map_negative_y:
		case TextureResourceBlob::type_cube_map_positive_z:
		case TextureResourceBlob::type_cube_map_negative_z:
		case TextureResourceBlob::type_1d_array:
		case TextureResourceBlob::type_2d_array:
			break;
		default:
			throw std::runtime_error("invalid texture type");
	}
}

void glurg::TextureResourceBlobBuilder::verify_dimensions(
	int width, int height, int depth)
{
	if (width <= 0)
	{
		throw std::runtime_error("width is unset or an invalid value");
	}

	if (height <= 0)
	{
		throw std::runtime_error("height is unset or an invalid value");
	}

	if (depth <= 0)
	{
		throw std::runtime_error("depth is unset or an invalid value");
	}
}

void glurg::TextureResourceBlobBuilder::verify_mipmap_level(int level)
{
	if (level < 0)
	{
		throw std::runtime_error("mipmap level is unset or an invalid value");
	}
}

void glurg::TextureResourceBlobBuilder::verify_pixel_component_description(
	const PixelComponentDescription& description)
{
	switch (description.swizzle)
	{
		case PixelComponentDescription::swizzle_red:
		case PixelComponentDescription::swizzle_green:
		case PixelComponentDescription::swizzle_blue:
		case PixelComponentDescription::swizzle_alpha:
		case PixelComponentDescription::swizzle_zero:
		case PixelComponentDescription::swizzle_one:
			break;
		default:
			throw std::runtime_error("invalid pixel component swizzle mode");
	}

	switch (description.storage)
	{
		case PixelComponentDescription::storage_disabled:
		case PixelComponentDescription::storage_signed_normalized:
		case PixelComponentDescription::storage_unsigned_normalized:
		case PixelComponentDescription::storage_float:
		case PixelComponentDescription::storage_integral:
		case PixelComponentDescription::storage_unsigned_integral:
			break;
		default:
			throw std::runtime_error("invalid pixel component storage type");
	}

	if (description.bit_size == 0 &&
		description.storage != PixelComponentDescription::storage_disabled)
	{
		throw std::runtime_error(
			"pixel component bit size set, but component is disabled");
	}
}

void glurg::TextureResourceBlobBuilder::verify_wrap_modes(
	const int wrap_modes[], std::size_t count, int texture_type)
{
	// Only verify wrap modes corresponding to dimensions actually used,
	// depending on the type of texture.
	std::size_t dimensions = 0;
	switch (texture_type)
	{
		case TextureResourceBlob::type_1d:
		case TextureResourceBlob::type_1d_array:
			dimensions = 1;
			break;
		case TextureResourceBlob::type_2d:
		case TextureResourceBlob::type_cube_map_positive_x:
		case TextureResourceBlob::type_cube_map_negative_x:
		case TextureResourceBlob::type_cube_map_positive_y:
		case TextureResourceBlob::type_cube_map_negative_y:
		case TextureResourceBlob::type_cube_map_positive_z:
		case TextureResourceBlob::type_cube_map_negative_z:
		case TextureResourceBlob::type_2d_array:
			dimensions = 2;
			break;
		case TextureResourceBlob::type_3d:
			dimensions = 3;
			break;
	}

	if (dimensions > count)
	{
		throw std::runtime_error("fewer wrap modes than texture dimensions");
	}

	for (std::size_t i = 0; i < dimensions; ++i)
	{
		switch(wrap_modes[i])
		{
			case TextureResourceBlob::wrap_border_clamp:
			case TextureResourceBlob::wrap_edge_clamp:
			case TextureResourceBlob::wrap_repeat:
			case TextureResourceBlob::wrap_mirror_edge_clamp:
			case TextureResourceBlob::wrap_mirror_repeat:
				break;
			default:
				throw std::runtime_error("invalid wrap mode");
		}
	}
}

void glurg::TextureResourceBlobBuilder::verify_zoom_filters(
	int minification_filter, int magnification_filter)
{
	switch (minification_filter)
	{
		case TextureResourceBlob::filter_nearest:
		case TextureResourceBlob::filter_linear:
		case TextureResourceBlob::filter_nearest_mipmap_nearest:
		case TextureResourceBlob::filter_linear_mipmap_nearest:
		case TextureResourceBlob::filter_nearest_mipmap_linear:
		case TextureResourceBlob::filter_linear_mipmap_linear:
			break;
		default:
			throw std::runtime_error("invalid minification filter");
	}

	switch (magnification_filter)
	{
		case TextureResourceBlob::filter_nearest:
		case TextureResourceBlob::filter_linear:
			break;
		default:
			throw std::runtime_error("invalid magnification filter");
	}
}

void glurg::TextureResourceBlobBuilder::verify_pixel_data(
	const std::uint8_t* pixels, std::size_t pixels_size)
{
	if (pixels == nullptr)
	{
		throw std::runtime_error("pixel data not set");
	}

	if (pixels_size == 0)
	{
		throw std::runtime_error("invalid pixel data size");
	}
}

void glurg::TextureResourceBlobBuilder::verify_binding_point(int binding_point)
{
	if (binding_point < 0)
	{
		throw std::runtime_error("binding point is unset or invalid value");
	}
}

std::string glurg::TextureResourceBlobBuilder::get_texture_key(
	int binding_point, int texture_type, int level)
{
	std::string result;

	result += "GL_TEXTURE";
	result += std::to_string(binding_point);
	result += ", ";

	result += gl::string_from_texture_blob_texture_type(texture_type);
	result += ", ";

	result += "level = ";
	result += std::to_string(level);

	return result;
}

void glurg::TextureResourceBlobBuilder::extract_pixel_component_description(
	PixelComponentDescription& description,
	const std::shared_ptr<RenderValue>& swizzle,
	const std::shared_ptr<RenderValue>& storage,
	const std::shared_ptr<RenderValue>& bit_size)
{
	description.swizzle = gl::to_pixel_component_description_swizzle_mode(
		swizzle->to_string());
	description.storage = gl::to_pixel_component_description_storage_type(
		storage->to_string());
	description.bit_size = bit_size->to_integer();
}
