// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_COMMON_FILE_STREAM_HPP
#define LIBGLURG_COMMON_FILE_STREAM_HPP

#include <cstddef>
#include <cstdint>

namespace glurg
{
	class FileStream
	{
	public:
		enum
		{
			mode_read = 0x1,
			mode_write = 0x2,
			mode_read_write = mode_read | mode_write
		};

		virtual ~FileStream() = default;

		virtual void write(const std::uint8_t* data, std::size_t length) = 0;
		virtual void read(std::uint8_t* data, std::size_t length) = 0;

		virtual std::size_t get_position() = 0;
		virtual void set_position(std::size_t position);

		virtual bool get_is_end_of_file() const = 0;

		virtual int get_mode() const = 0;

		virtual void close();
	};
}

#endif
