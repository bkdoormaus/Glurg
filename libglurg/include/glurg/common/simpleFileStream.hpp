// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_COMMON_SIMPLE_FILE_STREAM_HPP
#define LIBGLURG_COMMON_SIMPLE_FILE_STREAM_HPP

#include <fstream>
#include <string>
#include "glurg/common/fileStream.hpp"

namespace glurg
{
	class SimpleFileStream : public FileStream
	{
	public:
		SimpleFileStream() = default;
		SimpleFileStream(const std::string& filename, int mode);
		~SimpleFileStream() = default;

		void open(const std::string& filename, int mode);

		void write(const std::uint8_t* data, std::size_t length);
		void read(std::uint8_t* data, std::size_t length);

		std::size_t get_position();
		void set_position(std::size_t position);

		bool get_is_end_of_file() const;
		
		void close();

	private:
		std::fstream file;
	};
}

#endif
