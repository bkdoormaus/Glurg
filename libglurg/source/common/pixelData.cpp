// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <stdexcept>
#include <png.h>
#include "glurg/common/fileStream.hpp"
#include "glurg/common/pixelData.hpp"
#include "glurg/resources/resourceBlobBuffer.hpp"

template <typename OutputBuffer>
void prepare_output_buffer(OutputBuffer& buffer, std::size_t size);

template <typename OutputBuffer>
void append_output_buffer(
	OutputBuffer& buffer,
	const std::uint8_t* values, std::size_t values_size);

template <>
void prepare_output_buffer<glurg::PixelDataBuffer>(
	glurg::PixelDataBuffer& buffer, std::size_t size)
{
	buffer.reserve(size);
}

template <>
void append_output_buffer<glurg::PixelDataBuffer>(
	glurg::PixelDataBuffer& buffer,
	const std::uint8_t* values, std::size_t values_size)
{
	buffer.insert(buffer.end(), values, values + values_size + 1);
}

template <>
void prepare_output_buffer<glurg::ResourceBlobWriteBuffer>(
	glurg::ResourceBlobWriteBuffer& buffer, std::size_t size)
{
	buffer.push_value(size);
}

template <>
void append_output_buffer<glurg::ResourceBlobWriteBuffer>(
	glurg::ResourceBlobWriteBuffer& buffer,
	const std::uint8_t* values, std::size_t values_size)
{
	for (std::size_t i = 0; i < values_size; ++i)
	{
		buffer.push_value(values[i]);
	}
}

template <typename OutputBuffer>
void read_png_pixel_data(
	OutputBuffer& output_buffer, const glurg::PixelDataBuffer& input_buffer)
{
	png_image image = { 0 };
	image.version = PNG_IMAGE_VERSION;

	if (!png_image_begin_read_from_memory(
		&image, &input_buffer[0], input_buffer.size()))
	{
		throw std::runtime_error("could not read PNG");
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
	prepare_output_buffer(output_buffer, image.height * size_pixels_row);

	const std::size_t row_size = PNG_IMAGE_ROW_STRIDE(image);
	for (std::size_t y = 0; y < image.height; ++y)
	{
		append_output_buffer(
			output_buffer, temp_buffer.get() + y * row_size, row_size);
	}

	png_image_free(&image);
}

template <typename OutputBuffer>
void read_pnm_pixel_data(
	OutputBuffer& output_buffer, const glurg::PixelDataBuffer& input_buffer)
{
	// TODO
	// apitrace uses an extended NetPBM format
	throw std::runtime_error("not yet implemented");
}

template <typename OutputBuffer>
void generic_read_pixel_data(OutputBuffer& output_buffer,
	const glurg::PixelDataBuffer& input_buffer)
{
	std::uint8_t magic = input_buffer.at(0);

	switch (magic)
	{
		case 0x89:
			read_png_pixel_data(output_buffer, input_buffer);
			break;
		case 'P':
			read_pnm_pixel_data(output_buffer, input_buffer);
			break;
	}
}

void glurg::read_pixel_data(PixelDataBuffer& output_buffer,
	const PixelDataBuffer& input_buffer)
{
	return generic_read_pixel_data(output_buffer, input_buffer);
}

void glurg::read_pixel_data(
	ResourceBlobWriteBuffer& output_buffer, const PixelDataBuffer& input_buffer)
{
	return generic_read_pixel_data(output_buffer, input_buffer);
}
