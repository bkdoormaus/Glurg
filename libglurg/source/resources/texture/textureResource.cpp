// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <algorithm>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include "glurg/resources/texture/textureResource.hpp"
#include "glurg/resources/texture/textureResourceBlob.hpp"

glurg::TextureResource::TextureResource(const TextureResourceBlob* blob)
{
	if (!is_compatible_texture_blob(blob))
	{
		throw std::runtime_error("texture blob not compatible, contains data");
	}

	this->blob = blob;
	this->pixel_components = get_num_components(blob);
}

glm::ivec4 glurg::TextureResource::fetch_pixel(std::size_t x, std::size_t y)
{
	glm::ivec4 result(0, 0, 0, 255);
	auto p = glm::value_ptr(result);

	std::size_t row_length = this->blob->get_width() * this->pixel_components;
	std::size_t row_offset = x * this->pixel_components;
	auto pixel = this->blob->get_pixels() + (row_length * y + row_offset);
	for (std::size_t i = 0; i < this->pixel_components; ++i)
	{
		p[i] = *(pixel++);
	}

	return result;
}

void glurg::TextureResource::decode_image(PixelDataBuffer& output_buffer)
{
	const std::size_t output_components = 4;

	const std::size_t num_pixels =
		this->blob->get_width() * this->blob->get_height();
	output_buffer.reserve(num_pixels * output_components);

	auto p = this->blob->get_pixels();
	for (std::size_t i = 0; i < num_pixels; ++i)
	{
		switch (this->pixel_components)
		{
			case 1:
				output_buffer.push_back(p[0]);
				output_buffer.push_back(p[0]);
				output_buffer.push_back(p[0]);
				output_buffer.push_back(255);
				break;
			case 2:
				output_buffer.insert(output_buffer.end(), p, p + 2);
				output_buffer.push_back(0);
				output_buffer.push_back(255);
				break;
			case 3:
				output_buffer.insert(output_buffer.end(), p, p + 3);
				output_buffer.push_back(255);
				break;
			case 4:
				output_buffer.insert(output_buffer.end(), p, p + 4);
				break;
		}

		p += this->pixel_components;
	}
}

const glurg::ResourceBlob* glurg::TextureResource::get_blob() const
{
	return this->blob;
}

bool glurg::TextureResource::is_compatible_texture_blob(
	const TextureResourceBlob* blob)
{
	bool is_compatible =
		is_compatible_pixel_description(blob->get_red_description()) &&
		is_compatible_pixel_description(blob->get_green_description()) &&
		is_compatible_pixel_description(blob->get_blue_description()) &&
		is_compatible_pixel_description(blob->get_alpha_description());

	bool r = is_pixel_description_enabled(blob->get_red_description());
	bool g = is_pixel_description_enabled(blob->get_green_description());
	bool b = is_pixel_description_enabled(blob->get_blue_description());
	bool a = is_pixel_description_enabled(blob->get_alpha_description());
	bool is_linear_layout =
		(r && g && b && a) ||
		(r && g && b && !a) ||
		(r && g && !b && !a) ||
		(r && !g && !b && !a);

	return is_compatible && is_linear_layout;
}

std::size_t glurg::TextureResource::get_num_components(
	const glurg::TextureResourceBlob* blob)
{
	bool r = is_pixel_description_enabled(blob->get_red_description());
	bool g = is_pixel_description_enabled(blob->get_green_description());
	bool b = is_pixel_description_enabled(blob->get_blue_description());
	bool a = is_pixel_description_enabled(blob->get_alpha_description());

	// This could be simplified to 'return r + g + b + a;' but I find that an
	// abuse of clarity.
	std::size_t components = 0;
	if (r)
	{
		++components;
	}
	if (g)
	{
		++components;
	}
	if (b)
	{
		++components;
	}
	if (a)
	{
		++components;
	}

	return components;
}

bool glurg::TextureResource::is_compatible_pixel_description(
	const PixelComponentDescription& description)
{
	bool is_disabled =
		(description.storage == PixelComponentDescription::storage_disabled);
	bool is_integral = (description.storage ==
		PixelComponentDescription::storage_unsigned_normalized);
	bool is_byte = (description.bit_size == 8);

	return (is_integral && is_byte) || is_disabled;
}

bool glurg::TextureResource::is_pixel_description_enabled(
	const PixelComponentDescription& description)
{
	return (description.storage != PixelComponentDescription::storage_disabled);
}
