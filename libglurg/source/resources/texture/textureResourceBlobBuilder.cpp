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
#include "glurg/resources/texture/pixelUnpacker.hpp"
#include "glurg/resources/texture/textureResourceBlobBuilder.hpp"

const int glurg::TextureResourceBlobBuilder::DEPTH_ATTACHMENT;

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
	const PixelData& pixel_data)
{
	if ((int)pixel_data.get_width() != width ||
		(int)pixel_data.get_height() != height)
	{
		throw std::runtime_error("pixel data size mismatch");
	}

	std::size_t size = pixel_data.get_buffer().size();
	std::uint8_t* p = new std::uint8_t[size];
	std::memcpy(p, &pixel_data.get_buffer()[0], size);

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

std::shared_ptr<glurg::TextureResourceBlob>
glurg::TextureResourceBlobBuilder::build()
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

	return std::make_shared<glurg::TextureResourceBlob>(std::move(final_buffer));
}

bool glurg::TextureResourceBlobBuilder::extract_from_state(
	const RenderState& state)
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
		texture_info->get_string("GL_TEXTURE_WRAP_S")));
	set_wrap_mode(1, gl::to_texture_blob_wrap_mode(
		texture_info->get_string("GL_TEXTURE_WRAP_T")));
	set_wrap_mode(2, gl::to_texture_blob_wrap_mode(
		texture_info->get_string("GL_TEXTURE_WRAP_R")));

	set_minification_filter(gl::to_texture_blob_zoom_filter(
		texture_info->get_string("GL_TEXTURE_MIN_FILTER")));
	set_magnification_filter(gl::to_texture_blob_zoom_filter(
		texture_info->get_string("GL_TEXTURE_MAG_FILTER")));

	set_width(texture_info->get_integer("GL_TEXTURE_WIDTH"));
	set_height(texture_info->get_integer("GL_TEXTURE_HEIGHT"));
	set_depth(texture_info->get_integer("GL_TEXTURE_DEPTH"));

	PixelDataBuffer pixel_data_buffer;
	decode_base64(texture_data->get_string("__data__"), pixel_data_buffer);

	PixelData pixel_data;
	PixelData::read(pixel_data_buffer, pixel_data);

	set_pixel_data(pixel_data);

	return true;
}

bool glurg::TextureResourceBlobBuilder::extract_framebuffer_from_state(
	const RenderState& state, int color_attachment)
{
	set_texture_type(glurg::TextureResourceBlob::type_2d);
	set_mipmap_level(0);

	auto framebuffer = state.get("framebuffer");
	std::shared_ptr<glurg::RenderValue> attachment;
	if (color_attachment == DEPTH_ATTACHMENT)
	{
		if (!framebuffer->has_field("GL_DEPTH_ATTACHMENT"))
		{
			return false;
		}
		attachment = framebuffer->get_field_by_name("GL_DEPTH_ATTACHMENT");
	}
	else
	{
		if (!framebuffer->has_field("GL_COLOR_ATTACHMENT", color_attachment))
		{
			return false;
		}
		attachment = framebuffer->get_field_by_name(
			"GL_COLOR_ATTACHMENT", color_attachment);
	}

	PixelDataBuffer pixel_data_buffer;
	decode_base64(attachment->get_string("__data__"), pixel_data_buffer);

	PixelData pixel_data;
	PixelData::read(pixel_data_buffer, pixel_data);

	this->red_component.swizzle = PixelComponentDescription::swizzle_red;
	this->red_component.storage = PixelComponentDescription::storage_disabled;
	this->green_component.swizzle = PixelComponentDescription::swizzle_green;
	this->green_component.storage = PixelComponentDescription::storage_disabled;
	this->blue_component.swizzle = PixelComponentDescription::swizzle_blue;
	this->blue_component.storage = PixelComponentDescription::storage_disabled;
	this->alpha_component.swizzle = PixelComponentDescription::swizzle_alpha;
	this->alpha_component.storage = PixelComponentDescription::storage_disabled;

	PixelComponentDescription description = {};
	description.storage =
		PixelComponentDescription::storage_unsigned_normalized;
	description.bit_size = 8;
	switch (pixel_data.get_num_components())
	{
		case 4:
			set_alpha_description(description);
		case 3:
			set_blue_description(description);
		case 2:
			set_green_description(description);
		case 1:
			set_red_description(description);
	}

	set_wrap_mode(0, TextureResourceBlob::wrap_edge_clamp);
	set_wrap_mode(1, TextureResourceBlob::wrap_edge_clamp);

	set_minification_filter(TextureResourceBlob::filter_nearest);
	set_magnification_filter(TextureResourceBlob::filter_nearest);

	set_width(pixel_data.get_width());
	set_height(pixel_data.get_height());
	set_depth(1);

	set_pixel_data(pixel_data);

	return true;
}

bool glurg::TextureResourceBlobBuilder::extract_from_call(
	GLenum data_format, GLenum data_type, const std::uint8_t* data)
{
	verify_texture_type(this->texture_type);
	verify_dimensions(this->width, this->height, this->depth);
	verify_mipmap_level(this->level);

	std::size_t num_components = 0;
	bool is_bgr = false;
	bool is_integer = false;
	switch (data_format)
	{
		case GL_RED_INTEGER:
			is_integer = true;
		case GL_RED:
		case GL_ALPHA:
			num_components = 1;
			break;
		case GL_RG_INTEGER:
			is_integer = true;
		case GL_RG:
		case GL_LUMINANCE_ALPHA:
			num_components = 2;
			break;
		case GL_BGR_INTEGER:
			is_integer = true;
		case GL_BGR:
			is_bgr = true;
			num_components = 3;
			break;
		case GL_RGB_INTEGER:
			is_integer = true;
		case GL_RGB:
			num_components = 3;
			break;
		case GL_BGRA_INTEGER:
			is_integer = true;
		case GL_BGRA:
			is_bgr = true;
			num_components = 4;
			break;
		case GL_RGBA_INTEGER:
			is_integer = true;
		case GL_RGBA:
			num_components = 4;
			break;
	}
	if (num_components == 0)
	{
		return false;
	}

	bool is_packed = false;
	std::size_t component_size = 0;
	bool is_signed = false;
	bool is_float = false;
	switch (data_type)
	{
		case GL_BYTE:
			is_signed = true;
		case GL_UNSIGNED_BYTE:
			component_size = 1;
			break;
		case GL_SHORT:
			is_signed = true;
		case GL_UNSIGNED_SHORT:
			component_size = 2;
			break;
		case GL_INT:
			is_signed = true;
		case GL_FLOAT:
			is_float = true;
		case GL_UNSIGNED_INT:
			component_size = 4;
			break;
		default:
			is_packed = true;
			component_size = 1;
			break;
	}

	const std::size_t pixel_size = num_components * component_size;
	const std::size_t plane_size = pixel_size * this->width * this->height;
	const std::size_t data_size = plane_size * this->depth;
	auto d = std::make_unique<std::uint8_t[]>(data_size);

	bool extract_pixels_result;
	if (is_packed)
	{
		extract_pixels_result = extract_packed_pixels(
			num_components, component_size,
			data_format, data,
			d.get(), is_bgr);
	}
	else
	{
		extract_pixels_result = extract_unpacked_pixels(
			num_components, component_size,
			data,
			d.get(), is_bgr);
	}

	if (!extract_pixels_result)
	{
		return false;
	}

	this->pixels = d.release();
	this->pixels_size = data_size;

	int storage_type = PixelComponentDescription::storage_none;
	if (is_float)
	{
		storage_type = PixelComponentDescription::storage_float;
	}
	else
	{
		if (is_signed)
		{
			if (is_integer)
			{
				storage_type =
					PixelComponentDescription::storage_signed_integral;
			}
			else
			{
				storage_type =
					PixelComponentDescription::storage_signed_normalized;
			}
		}
		else
		{
			if (is_integer)
			{
				storage_type =
					PixelComponentDescription::storage_unsigned_integral;
			}
			else
			{
				storage_type =
					PixelComponentDescription::storage_unsigned_normalized;
			}
		}
	}
	const std::size_t component_bits = component_size * 8;

	set_uniform_pixel_component_descriptions(
		num_components, storage_type, component_bits);

	return true;
}

bool glurg::TextureResourceBlobBuilder::extract_unpacked_pixels(
	std::size_t num_components, std::size_t component_size,
	const std::uint8_t* data,
	std::uint8_t* output_pixels, bool swap_red_blue)
{
	const std::size_t pixel_size = num_components * component_size;

	const std::size_t red_index = swap_red_blue ? 2 : 0;
	const std::size_t green_index = 1;
	const std::size_t blue_index = swap_red_blue ? 0 : 2;
	const std::size_t alpha_index = 3;

	auto pixel = data;
	for (std::size_t z = 0; z < this->depth; ++z)
	{
		for (std::size_t y = 0; y < this->height; ++y)
		{
			for (std::size_t x = 0; x < this->width; ++x)
			{
				for (std::size_t i = 0; i < component_size; ++i)
				{
					switch (num_components)
					{
						case 4:
							output_pixels[3 * component_size + i] =
								pixel[alpha_index * component_size + i];
						case 3:
							output_pixels[2 * component_size + i] =
								pixel[blue_index * component_size + i];
						case 2:
							output_pixels[1 * component_size + i] =
								pixel[green_index * component_size + i];
						case 1:
							output_pixels[0 * component_size + i] = 
								pixel[red_index * component_size + i];
							break;
						default:
							assert(false);
					}
				}
				pixel += pixel_size;
				output_pixels += pixel_size;
			}
		}
	}

	return true;
}

bool glurg::TextureResourceBlobBuilder::extract_packed_pixels(
	std::size_t num_components, std::size_t component_size,
	GLenum data_format, const std::uint8_t* data,
	std::uint8_t* output_pixels, bool swap_red_blue)
{
	const PixelUnpacker* unpacker =
		PixelUnpacker::get_pixel_fetcher(data_format);
	if (unpacker == nullptr)
	{
		return false;
	}

	for (std::size_t z = 0; z < this->depth; ++z)
	{
		for (std::size_t y = 0; y < this->height; ++y)
		{
			auto row = unpacker->fetch_row(data, this->width, y);
			for (std::size_t x = 0; x < this->width; ++x)
			{
				glurg::UnpackedPixel current_pixel;
				unpacker->fetch_pixel(row, x, current_pixel);

				switch (num_components)
				{
					case 4:
						output_pixels[3] = current_pixel.alpha;
					case 3:
						output_pixels[2] = swap_red_blue ?
							current_pixel.red : current_pixel.blue;
					case 2:
						output_pixels[1] = current_pixel.green;
					case 1:
						output_pixels[0] = swap_red_blue ?
							current_pixel.blue : current_pixel.red;
						break;
					default:
						assert(false);
				}

				output_pixels += num_components;
			}
		}
	}

	return true;
}

void
glurg::TextureResourceBlobBuilder::set_uniform_pixel_component_descriptions(
	std::size_t count, int storage, std::size_t bit_size)
{
	this->red_component.swizzle = PixelComponentDescription::swizzle_red;
	this->red_component.storage = PixelComponentDescription::storage_disabled;
	this->red_component.bit_size = 0;
	this->green_component.swizzle = PixelComponentDescription::swizzle_green;
	this->green_component.storage = PixelComponentDescription::storage_disabled;
	this->green_component.bit_size = 0;
	this->blue_component.swizzle = PixelComponentDescription::swizzle_blue;
	this->blue_component.storage = PixelComponentDescription::storage_disabled;
	this->blue_component.bit_size = 0;
	this->alpha_component.swizzle = PixelComponentDescription::swizzle_alpha;
	this->alpha_component.storage = PixelComponentDescription::storage_disabled;
	this->alpha_component.bit_size = 0;

	switch (count)
	{
		case 4:
			this->alpha_component.bit_size = bit_size;
			this->alpha_component.storage = storage;
		case 3:
			this->blue_component.bit_size = bit_size;
			this->blue_component.storage = storage;
		case 2:
			this->green_component.bit_size = bit_size;
			this->green_component.storage = storage;
		case 1:
			this->red_component.bit_size = bit_size;
			this->red_component.storage = storage;
			break;
		default:
			assert(false);
	}
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
		case PixelComponentDescription::storage_signed_integral:
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
