// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstring>
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

	verify_pixels(this->pixels, this->pixels_size);
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
			case TextureResourceBlob::wrap_mirror_border_clamp:
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
