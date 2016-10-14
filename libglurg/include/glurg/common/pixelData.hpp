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
	
	void read_pixel_data(PixelDataBuffer& output_buffer,
		const PixelDataBuffer& input_buffer);
	void read_pixel_data(ResourceBlobWriteBuffer& output_buffer, 
		const PixelDataBuffer& input_buffer);
}

#endif
