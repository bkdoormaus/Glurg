// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_TEXTURE_RESOURCE_BLOB_BUILDER_HPP
#define LIBGLURG_RESOURCES_TEXTURE_RESOURCE_BLOB_BUILDER_HPP

#include "glurg/resources/resourceBlobBuilder.hpp"
#include "glurg/resources/textureResourceBlob.hpp"

namespace glurg
{
	class ResourceBlobWriteBuffer;

	class TextureResourceBlobBuilder :
		public ResourceBlobBuilder<TextureResourceBlob>
	{
	public:
		TextureResourceBlobBuilder();
		~TextureResourceBlobBuilder();

		void set_red_description(const PixelComponentDescription& value);
		void set_green_description(const PixelComponentDescription& value);
		void set_blue_description(const PixelComponentDescription& value);
		void set_alpha_description(const PixelComponentDescription& value);

		void set_wrap_mode(std::size_t dimension, int value);

		void set_texture_type(int value);

		void set_minification_filter(int filter);
		void set_magnification_filter(int filter);

		void set_width(int value);
		void set_height(int value);
		void set_depth(int value);
		void set_mipmap_level(int value);

		void set_pixel_data(const std::uint8_t* pixels, std::size_t size);

		TextureResourceBlob* build();
		void extract(const RenderState& buffer);

	private:
		int texture_type;
		int width, height, depth, level;

		PixelComponentDescription red_component;
		PixelComponentDescription green_component;
		PixelComponentDescription blue_component;
		PixelComponentDescription alpha_component;
		static void write_pixel_component_description(
			const PixelComponentDescription& description,
			ResourceBlobWriteBuffer& buffer);

		int wrap_mode[TextureResourceBlob::MAX_DIMENSIONS];
		int minification_filter, magnification_filter;

		const std::uint8_t* pixels;
		std::size_t pixels_size;
	};
}

#endif
