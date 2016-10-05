// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_TEXTURE_RESOURCE_HPP
#define LIBGLURG_RESOURCES_TEXTURE_RESOURCE_HPP

#include <cstddef>
#include <cstdint>
#include "glurg/common/hash.hpp"
#include "glurg/resources/resource.hpp"

namespace glurg
{
	class TextureResource : public glurg::Resource
	{
	public:
		enum
		{
			type_1d,
			type_2d,
			type_3d,
			type_cube_map_positive_x,
			type_cube_map_negative_x,
			type_cube_map_positive_y,
			type_cube_map_negative_y,
			type_cube_map_positive_z,
			type_cube_map_negative_z,
			type_1d_array,
			type_2d_array
		};

		enum
		{
			pixel_components_r,
			pixel_components_rg,
			pixel_components_rgb,
			pixel_components_rgba
		};

		enum
		{
			pixel_format_ubyte,
			pixel_format_byte,
			pixel_format_ushort,
			pixel_format_short,
			pixel_format_uint,
			pixel_format_int,
			pixel_format_float
		};

		TextureResource(
			int width, int height, int depth, int level,
			int texture_type, int pixel_components, int pixel_format,
			std::uint8_t* pixels);
		~TextureResource();

		int get_width() const;
		int get_height() const;
		int get_depth() const;
		int get_level() const;
		int get_texture_type() const;
		int get_pixel_components() const;
		int get_pixel_format() const;
		const std::uint8_t* get_pixels() const;

		static int get_num_pixel_components(int pixel_components);
		static int get_pixel_component_size(int pixel_format);
		static int get_pixel_size(int pixel_components, int pixel_format);

		static bool is_valid_texture_type(int texture_type);
		static bool is_valid_pixel_components(int pixel_components);
		static bool is_valid_pixel_format(int pixel_format);

		const glurg::Hash& get_hash() const;

		const std::uint8_t* get_data() const;
		const std::size_t get_size() const;

	private:
		struct TextureHeader
		{
			int width, height, depth;
			int level;
			int texture_type;
			int pixel_components, pixel_format;
		};

		glurg::Hash hash;

		// A single allocation containing the texture header and pixel data,
		// for hashing and serialization purposes.
		std::uint8_t* data;
		std::size_t size;

		// Located at the start of 'data'.
		TextureHeader* header;

		// Located at 'data + sizeof(TextureHeader)'
		std::uint8_t* pixels;
	};
}

#endif
