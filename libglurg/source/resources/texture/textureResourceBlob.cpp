// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/common/hash.hpp"
#include "glurg/resources/texture/textureResourceBlob.hpp"

glurg::TextureResourceBlob::TextureResourceBlob(ResourceBlobReadBuffer&& buffer)
	: buffer(std::move(buffer))
{
	this->hash = Hash::hash(
		this->buffer.get_buffer(), this->buffer.get_buffer_size());

	this->texture_type = this->buffer.pop_value<int>();
	this->width = this->buffer.pop_value<int>();
	this->height = this->buffer.pop_value<int>();
	this->depth = this->buffer.pop_value<int>();
	this->level = this->buffer.pop_value<int>();

	read_pixel_component_description(this->red_component);
	read_pixel_component_description(this->green_component);
	read_pixel_component_description(this->blue_component);
	read_pixel_component_description(this->alpha_component);

	for (std::size_t i = 0; i < MAX_DIMENSIONS; ++i)
	{
		this->wrap_mode[i] = this->buffer.pop_value<int>();
	}

	this->minification_filter = this->buffer.pop_value<int>();
	this->magnification_filter = this->buffer.pop_value<int>();

	this->pixels = this->buffer.pop_data(pixels_size);
}

glurg::PixelComponentDescription
glurg::TextureResourceBlob::get_red_description() const
{
	return this->red_component;
}

glurg::PixelComponentDescription
glurg::TextureResourceBlob::get_green_description() const
{
	return this->green_component;
}

glurg::PixelComponentDescription
glurg::TextureResourceBlob::get_blue_description() const
{
	return this->blue_component;
}

glurg::PixelComponentDescription
glurg::TextureResourceBlob::get_alpha_description() const
{
	return this->alpha_component;
}

int glurg::TextureResourceBlob::get_wrap_mode(std::size_t dimension) const
{
	if (dimension >= MAX_DIMENSIONS)
	{
		throw std::runtime_error("dimension index out-of-bounds");
	}

	return this->wrap_mode[dimension];
}

int glurg::TextureResourceBlob::get_texture_type() const
{
	return this->texture_type;
}

int glurg::TextureResourceBlob::get_minification_filter() const
{
	return this->minification_filter;
}

int glurg::TextureResourceBlob::get_magnification_filter() const
{
	return this->magnification_filter;
}

int glurg::TextureResourceBlob::get_width() const
{
	return this->width;
}

int glurg::TextureResourceBlob::get_height() const
{
	return this->height;
}

int glurg::TextureResourceBlob::get_depth() const
{
	return this->depth;
}

int glurg::TextureResourceBlob::get_mipmap_level() const
{
	return this->level;
}

const std::uint8_t* glurg::TextureResourceBlob::get_pixels() const
{
	return this->pixels;
}

std::size_t glurg::TextureResourceBlob::get_pixels_size() const
{
	return this->pixels_size;
}

const glurg::Hash& glurg::TextureResourceBlob::get_hash() const
{
	return this->hash;
}

const std::uint8_t* glurg::TextureResourceBlob::get_data() const
{
	return this->buffer.get_buffer();
}

std::size_t glurg::TextureResourceBlob::get_size() const
{
	return this->buffer.get_buffer_size();
}

void glurg::TextureResourceBlob::read_pixel_component_description(
	PixelComponentDescription& description)
{
	description.swizzle = this->buffer.pop_value<int>();
	description.storage = this->buffer.pop_value<int>();
	description.bit_size = this->buffer.pop_value<int>();
}
