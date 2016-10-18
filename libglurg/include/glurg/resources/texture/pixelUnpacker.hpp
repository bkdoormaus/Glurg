// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_TEXTURE_PIXEL_HPP
#define LIBGLURG_RESOURCES_TEXTURE_PIXEL_HPP

#include <cstdint>
#include "glurg/resources/openGL.hpp"

namespace glurg
{
	struct UnpackedPixel
	{
		std::uint8_t red;
		std::uint8_t green;
		std::uint8_t blue;
		std::uint8_t alpha;
	};

	class PixelUnpacker
	{
	public:
		virtual ~PixelUnpacker() = default;

		virtual const std::uint8_t* fetch_row(
			const std::uint8_t* pixels, int row_length, int row) const = 0;

		virtual void fetch_pixel(
			const std::uint8_t* row, int column,
			UnpackedPixel& pixel) const = 0;
		void fetch_pixel(
			const std::uint8_t* pixels,
			int row, int row_length, int column,
			UnpackedPixel& pixel) const;

		static const PixelUnpacker* get_pixel_fetcher(GLenum format);
	private:
		static void verify_pixel_sizes(
			std::uint32_t red_size,
			std::uint32_t green_size,
			std::uint32_t blue_size,
			std::uint32_t alpha_size);
	};
}

#endif
