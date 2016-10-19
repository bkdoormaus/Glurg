// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_COMMON_PIXEL_DATA_HPP
#define LIBGLURG_COMMON_PIXEL_DATA_HPP

#include <cstdint>
#include <vector>

namespace glurg
{
	class FileStream;
	class ResourceBlobWriteBuffer;

	typedef std::vector<std::uint8_t> PixelDataBuffer;

	class PixelData
	{
	public:
		PixelData();
		~PixelData() = default;

		enum
		{
			format_none,
			format_integer,
			format_float
		};
		int get_component_format() const;

		const PixelDataBuffer& get_buffer() const;

		std::size_t get_width() const;
		std::size_t get_height() const;
		std::size_t get_num_components() const;

		static void read(const PixelDataBuffer& input_buffer, PixelData& data);

		// Expects input_buffer to be equivalent to format_integer.
		static bool to_png(
			const PixelDataBuffer& input_buffer,
			std::size_t width, std::size_t height, std::size_t num_components,
			PixelDataBuffer& output_buffer);

	private:
		void read_png(const glurg::PixelDataBuffer& input_buffer);
		void read_pnm(const glurg::PixelDataBuffer& input_buffer);

		int format;
		PixelDataBuffer buffer;

		std::size_t width, height;
		std::size_t components;
	};
}

#endif
