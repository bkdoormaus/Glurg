// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <png.h>
#include <glm/gtc/type_ptr.hpp>
#include "glurg/common/pixelData.hpp"

glurg::PixelData::PixelData()
{
	this->format = format_none;

	this->width = 0;
	this->height = 0;
	this->num_components = 0;
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
	return this->num_components;
}

void glurg::PixelData::read(
	const glurg::PixelDataBuffer& input_buffer, glurg::PixelData& data,
	bool normalize)
{
	std::uint8_t magic = input_buffer.at(0);

	switch (magic)
	{
		case 0x89:
			data.read_png(input_buffer);
			break;
		case 'P':
			data.read_pnm(input_buffer, normalize);
			break;
	}
}

bool glurg::PixelData::to_png(
	const PixelDataBuffer& input_buffer,
	std::size_t width, std::size_t height, std::size_t num_components,
	PixelDataBuffer& output_buffer)
{
	png_image image = { 0 };
	image.version = PNG_IMAGE_VERSION;
	image.width = width;
	image.height = height;
	switch (num_components)
	{
		case 4:
			image.format = PNG_FORMAT_RGBA;
			break;
		case 3:
			image.format = PNG_FORMAT_RGB;
			break;
		case 2:
			image.format = PNG_FORMAT_GA;
			break;
		case 1:
			image.format = PNG_FORMAT_GRAY;
			break;
		default:
			return false;
	}

	std::size_t bytes = 0;
	if (!png_image_write_to_memory(
		&image, nullptr, &bytes, 
		false, &input_buffer[0], num_components * width, nullptr))
	{
		return false;
	}

	output_buffer.resize(bytes);
	return png_image_write_to_memory(
		&image, &output_buffer[0], &bytes,
		false, &input_buffer[0], num_components * width, nullptr);
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
	this->num_components = components;
}

void glurg::PixelData::read_pnm_pixel(
	const std::uint8_t* input, std::size_t count, glm::vec4& pixel)
{
	assert(count >= 1 && count <= 4);

	pixel.r = 0.0f;
	pixel.g = 0.0f;
	pixel.b = 0.0f;
	pixel.a = 0.0f;

	auto p = glm::value_ptr(pixel);
	auto d = (float*)input;
	for (std::size_t i = 0; i < count; ++i)
	{
		p[i] = d[i];
	}
}

void glurg::PixelData::read_pnm_pixel(
	const std::uint8_t* input, std::size_t count, glm::ivec4& pixel)
{
	assert(count >= 1 && count <= 4);

	pixel.r = 0;
	pixel.g = 0;
	pixel.b = 0;
	pixel.a = 0;

	auto p = glm::value_ptr(pixel);
	for (std::size_t i = 0; i < count; ++i)
	{
		p[i] = input[i];
	}
}

void glurg::PixelData::pnm_next_line(const std::uint8_t*& input)
{
	while (*input++ != '\n')
		continue;
}

const std::uint8_t* glurg::PixelData::read_pnm_header(
	const glurg::PixelDataBuffer& input_buffer)
{
	switch (input_buffer.at(1))
	{
		case '5':
			this->num_components = 1;
			this->format = format_integer;
			break;
		case '6':
			this->num_components = 3;
			this->format = format_integer;
			break;
		case 'f':
			this->num_components = 1;
			this->format = format_float;
			break;
		case 'F':
			this->num_components = 3;
			this->format = format_float;
			break;
		case 'X':
			this->num_components = 4;
			this->format = format_float;
			break;
		default:
			throw std::runtime_error("invalid PNM header");
	}

	const std::uint8_t* pnm_data = &input_buffer[0];

	pnm_next_line(pnm_data);
	if (*pnm_data == '#')
	{
		pnm_next_line(pnm_data);
	}

	unsigned width, height;
	if (std::sscanf((const char*)pnm_data, "%u %u", &width, &height) != 2)
	{
		throw std::runtime_error("couldn't read width and height");
	}
	else
	{
		this->width = width;
		this->height = height;
	}
	pnm_next_line(pnm_data);

	// This line contains the 'max value', which is 255 of format_integer and
	// 1.0 for format_float.
	pnm_next_line(pnm_data);

	return pnm_data;
}

void glurg::PixelData::read_pnm(
	const glurg::PixelDataBuffer& input_buffer, bool normalize)
{
	const std::uint8_t* pixel_data = read_pnm_header(input_buffer);

	buffer.clear();
	for (std::size_t y = 0; y < this->width; ++y)
	{
		for (std::size_t x = 0; x < this->height; ++x)
		{
			if (this->format == format_integer)
			{
				glm::ivec4 pixel;
				read_pnm_pixel(pixel_data, this->num_components, pixel);

				auto p = glm::value_ptr(pixel);
				for (std::size_t i = 0; i < this->num_components; ++i)
				{
					this->buffer.push_back(p[i]);
				}

				pixel_data += this->num_components;
			}
			else if (this->format == format_float)
			{
				glm::vec4 pixel;
				read_pnm_pixel(pixel_data, this->num_components, pixel);

				auto p = glm::value_ptr(pixel);
				for (std::size_t i = 0; i < this->num_components; ++i)
				{
					if (normalize)
					{
						this->buffer.push_back((std::uint8_t)(p[i] * 255));
					}
					else
					{
						this->buffer.insert(
							this->buffer.end(), p, p + sizeof(float));
					}
				}

				pixel_data += this->num_components * sizeof(float);
			}
		}
	}
}
