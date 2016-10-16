// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <stdexcept>
#include <png.h>
#include "glurg/common/pixelData.hpp"

glurg::PixelData::PixelData()
{
	this->format = format_none;

	this->width = 0;
	this->height = 0;
	this->components = 0;
}

int glurg::PixelData::get_component_format() const
{
	return this->format;
}

const glurg::PixelDataBuffer& glurg::PixelData::get_buffer() const
{
	return this->buffer;
}

std::size_t glurg::PixelData::get_width() const
{
	return this->width;
}

std::size_t glurg::PixelData::get_height() const
{
	return this->height;
}

std::size_t glurg::PixelData::get_num_components() const
{
	return this->components;
}

void glurg::PixelData::read(
	const glurg::PixelDataBuffer& input_buffer, glurg::PixelData& data)
{
	std::uint8_t magic = input_buffer.at(0);

	switch (magic)
	{
		case 0x89:
			data.read_png(input_buffer);
			break;
		case 'P':
			data.read_pnm(input_buffer);
			break;
	}
}

void glurg::PixelData::read_png(const glurg::PixelDataBuffer& input_buffer)
{
	png_image image = { 0 };
	image.version = PNG_IMAGE_VERSION;

	if (!png_image_begin_read_from_memory(
		&image, &input_buffer[0], input_buffer.size()))
	{
		throw std::runtime_error("could not read PNG");
	}

	std::size_t components = 0;
	switch (image.format)
	{
		case PNG_FORMAT_GRAY:
			components = 1;
			break;
		case PNG_FORMAT_GA:
		case PNG_FORMAT_AG:
			components = 2;
			image.format = PNG_FORMAT_GA;
			break;
		case PNG_FORMAT_RGB:
		case PNG_FORMAT_BGR:
			components = 3;
			image.format = PNG_FORMAT_RGB;
		case PNG_FORMAT_RGBA:
		case PNG_FORMAT_ARGB:
		case PNG_FORMAT_BGRA:
		case PNG_FORMAT_ABGR:
			components = 4;
			image.format = PNG_FORMAT_RGBA;
			break;
		default:
			throw std::runtime_error("unhandled PNG image format");
	}

	auto temp_buffer = std::make_unique<std::uint8_t[]>(PNG_IMAGE_SIZE(image));
	if (!png_image_finish_read(
		&image, nullptr,
		temp_buffer.get(), PNG_IMAGE_ROW_STRIDE(image),
		nullptr))
	{
		throw std::runtime_error("could not read PNG");
	}

	const std::size_t size_pixels_row =
		PNG_IMAGE_PIXEL_SIZE(image.format) * image.width;
	this->buffer.clear();
	this->buffer.reserve(size_pixels_row * image.height);

	const std::size_t row_size = PNG_IMAGE_ROW_STRIDE(image);
	for (std::size_t y = 0; y < image.height; ++y)
	{
		auto begin = temp_buffer.get() + y * row_size;
		auto end = begin + row_size;
		this->buffer.insert(this->buffer.end(), begin, end);
	}

	png_image_free(&image);

	this->format = format_integer;
	this->width = image.width;
	this->height = image.height;
	this->components = components;
}

void glurg::PixelData::read_pnm(const glurg::PixelDataBuffer& input_buffer)
{
	// TODO
	// apitrace uses an extended NetPBM format
	throw std::runtime_error("not yet implemented");
}
