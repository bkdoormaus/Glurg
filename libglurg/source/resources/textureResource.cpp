// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include <boost/functional/hash.hpp>
#include "glurg/resources/textureResource.hpp"

glurg::TextureResource::TextureResource(
	int width, int height, int depth, int level,
	int texture_type, int pixel_components, int pixel_format,
	std::uint8_t* pixels)
{
	assert(width >= 1);
	assert(height >= 1);
	assert(depth >= 1);
	assert(level >= 0);
	assert(is_valid_texture_type(texture_type));
	assert(is_valid_pixel_components(pixel_components));
	assert(is_valid_pixel_format(pixel_format));

	std::size_t pixel_size = get_pixel_size(pixel_components, pixel_format);
	std::size_t pixels_length =
		pixel_size * (std::size_t)width * (std::size_t)height * (std::size_t)depth;

	this->size = pixels_length + sizeof(TextureHeader);
	this->data = new std::uint8_t[this->size];

	this->header = (TextureHeader*)this->data;
	this->header->width = width;
	this->header->height = height;
	this->header->depth = depth;
	this->header->level = level;
	this->header->texture_type = texture_type;
	this->header->pixel_components = pixel_components;
	this->header->pixel_format = pixel_format;

	this->pixels = this->data + sizeof(TextureHeader);
	std::memcpy(this->pixels, pixels, pixels_length);

	hash = Hash::hash(this->data, this->size);
}

int glurg::TextureResource::get_width() const
{
	return this->header->width;
}

int glurg::TextureResource::get_height() const
{
	return this->header->height;
}

int glurg::TextureResource::get_depth() const
{
	return this->header->depth;
}

int glurg::TextureResource::get_level() const
{
	return this->header->level;
}

int glurg::TextureResource::get_texture_type() const
{
	return this->header->texture_type;
}

int glurg::TextureResource::get_pixel_components() const
{
	return this->header->pixel_components;
}

int glurg::TextureResource::get_pixel_format() const
{
	return this->header->pixel_format;
}

const std::uint8_t* glurg::TextureResource::get_pixels() const
{
	return this->pixels;
}

int glurg::TextureResource::get_num_pixel_components(int pixel_components)
{
	switch (pixel_components)
	{
		case pixel_components_r:
			return 1;
		case pixel_components_rg:
			return 2;
		case pixel_components_rgb:
			return 3;
		case pixel_components_rgba:
			return 4;
		default:
			assert(false);
			return 0;
	}
}

int glurg::TextureResource::get_pixel_component_size(int pixel_format)
{
	switch (pixel_format)
	{
		case pixel_format_ubyte:
		case pixel_format_byte:
			return 1;
		case pixel_format_ushort:
		case pixel_format_short:
			return 2;
		case pixel_format_uint:
		case pixel_format_int:
		case pixel_format_float:
			return 4;
		default:
			assert(false);
			return 0;
	}
}

int glurg::TextureResource::get_pixel_size(
	int pixel_components, int pixel_format)
{
	int n = get_num_pixel_components(pixel_components);
	int s = get_pixel_component_size(pixel_format);

	return n * s;
}

bool glurg::TextureResource::is_valid_texture_type(int texture_type)
{
	switch (texture_type)
	{
		case type_1d:
		case type_2d:
		case type_3d:
		case type_cube_map_positive_x:
		case type_cube_map_negative_x:
		case type_cube_map_positive_y:
		case type_cube_map_negative_y:
		case type_cube_map_positive_z:
		case type_cube_map_negative_z:
		case type_1d_array:
		case type_2d_array:
			return true;
		default:
			return false;
	}
}

bool glurg::TextureResource::is_valid_pixel_components(int pixel_components)
{
	switch (pixel_components)
	{
		case pixel_components_r:
		case pixel_components_rg:
		case pixel_components_rgb:
		case pixel_components_rgba:
			return true;
		default:
			return false;
	}
}

bool glurg::TextureResource::is_valid_pixel_format(int pixel_format)
{
	switch (pixel_format)
	{
		case pixel_format_ubyte:
		case pixel_format_byte:
		case pixel_format_ushort:
		case pixel_format_short:
		case pixel_format_uint:
		case pixel_format_int:
		case pixel_format_float:
			return true;
		default:
			return false;
	}
}

const glurg::Hash& glurg::TextureResource::get_hash() const
{
	return this->hash;
}

const std::uint8_t* glurg::TextureResource::get_data() const
{
	return this->data;
}

const std::size_t glurg::TextureResource::get_size() const
{
	return this->size;
}
