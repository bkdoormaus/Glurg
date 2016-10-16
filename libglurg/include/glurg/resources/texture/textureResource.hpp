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

	class TextureResource
	{
	public:
		static const std::uint8_t FINGERPRINT_ID = 0x01;
		static const std::size_t FINGERPRINT_REGIONS_X = 4;
		static const std::size_t FINGERPRINT_REGIONS_Y = 4;

		TextureResource(const TextureResourceBlob* blob);
		~TextureResource() = default;

		glm::ivec4 fetch_pixel(std::size_t x, std::size_t y);

		void decode_image(
			std::size_t output_components, PixelDataBuffer& output_buffer);

		const ResourceBlob* get_blob() const;
		const ResourceFingerprint& get_fingerprint() const;

		static bool is_compatible_texture_blob(const TextureResourceBlob* blob);

	private:
		static const std::size_t FINGERPRINT_COLOR_BITS = 16;
		static const std::size_t FINGERPRINT_COLOR_MAX_VALUE =
			(1 << FINGERPRINT_COLOR_BITS) - 1;
		void generate_fingerprint();
		glm::vec4 fingerprint_region(
			std::size_t x, std::size_t y,
			std::size_t width, std::size_t height);

		static std::size_t get_num_components(const TextureResourceBlob* blob);
		static bool is_compatible_pixel_description(
			const PixelComponentDescription& description);
		static bool is_pixel_description_enabled(
			const PixelComponentDescription& description);

		std::size_t pixel_components;
		const TextureResourceBlob* blob;
		ResourceFingerprint fingerprint;
	};
}

#endif
