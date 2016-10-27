// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_TEXTURE_TEXTURE_RESOURCE_HPP
#define LIBGLURG_RESOURCES_TEXTURE_TEXTURE_RESOURCE_HPP

#include <cstddef>
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include "glurg/common/pixelData.hpp"
#include "glurg/resources/resource.hpp"

namespace glurg
{
	class PixelComponentDescription;
	class TextureResourceBlob;

	class TextureResource : public Resource
	{
	public:
		TextureResource(const TextureResourceBlob* blob);
		~TextureResource() = default;

		glm::ivec4 fetch_pixel(std::size_t x, std::size_t y);

		// Decodes to RGBA.
		void decode_image(PixelDataBuffer& output_buffer);

		const ResourceBlob* get_blob() const;

		static bool is_compatible_texture_blob(const TextureResourceBlob* blob);

	private:
		static std::size_t get_num_components(const TextureResourceBlob* blob);
		static bool is_compatible_pixel_description(
			const PixelComponentDescription& description);
		static bool is_pixel_description_enabled(
			const PixelComponentDescription& description);

		std::size_t pixel_components;
		const TextureResourceBlob* blob;
	};
}

#endif
