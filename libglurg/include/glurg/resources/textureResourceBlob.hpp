// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_TEXTURE_RESOURCE_BLOB_HPP
#define LIBGLURG_RESOURCES_TEXTURE_RESOURCE_BLOB_HPP

#include "glurg/common/hash.hpp"
#include "glurg/resources/resourceBlob.hpp"
#include "glurg/resources/resourceBlobBuffer.hpp"

namespace glurg
{
	class TextureResourceBlobBuilder;

	struct PixelComponentDescription
	{
		enum
		{
			swizzle_red,
			swizzle_green,
			swizzle_blue,
			swizzle_alpha,
			swizzle_zero,
			swizzle_one
		};
		int swizzle;

		enum
		{
			storage_none,
			storage_disabled,
			storage_signed_normalized,
			storage_unsigned_normalized,
			storage_float,
			storage_integral,
			storage_unsigned_integral
		};
		int storage;

		int bit_size;
	};

	class TextureResourceBlob : public ResourceBlob
	{
	public:
		TextureResourceBlob(ResourceBlobReadBuffer&& buffer);
		~TextureResourceBlob() = default;

		PixelComponentDescription get_red_description() const;
		PixelComponentDescription get_green_description() const;
		PixelComponentDescription get_blue_description() const;
		PixelComponentDescription get_alpha_description() const;

		enum
		{
			wrap_none,
			wrap_border_clamp,
			wrap_edge_clamp,
			wrap_repeat,
			wrap_mirror_border_clamp,
			wrap_mirror_edge_clamp,
			wrap_mirror_repeat
		};
		static const std::size_t MAX_DIMENSIONS = 3;
		int get_wrap_mode(std::size_t dimension) const;

		enum
		{
			type_none,
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
		int get_texture_type() const;

		enum
		{
			filter_none,
			filter_nearest,
			filter_linear,
			filter_nearest_mipmap_nearest,
			filter_linear_mipmap_nearest,
			filter_nearest_mipmap_linear,
			filter_linear_mipmap_linear
		};
		int get_minification_filter() const;
		int get_magnification_filter() const;

		int get_width() const;
		int get_height() const;
		int get_depth() const;
		int get_mipmap_level() const;

		const std::uint8_t* get_pixels() const;
		std::size_t get_pixels_size() const;

		const Hash& get_hash() const;
		const std::uint8_t* get_data() const;
		std::size_t get_size() const;

	private:
		void read_pixel_component_description(
			PixelComponentDescription& description);

		int texture_type;
		int width, height, depth, level;

		PixelComponentDescription red_component;
		PixelComponentDescription green_component;
		PixelComponentDescription blue_component;
		PixelComponentDescription alpha_component;

		int wrap_mode[MAX_DIMENSIONS];
		int minification_filter, magnification_filter;

		const std::uint8_t* pixels;
		std::size_t pixels_size;

		Hash hash;
		ResourceBlobReadBuffer buffer;
	};
}

#endif
