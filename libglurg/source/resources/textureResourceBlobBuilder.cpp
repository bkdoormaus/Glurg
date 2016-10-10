// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstring>
#include "glurg/resources/textureResourceBlobBuilder.hpp"

glurg::TextureResourceBlobBuilder::TextureResourceBlobBuilder()
{
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
