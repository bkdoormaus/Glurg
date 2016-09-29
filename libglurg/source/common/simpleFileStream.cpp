// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/common/simpleFileStream.hpp"

glurg::SimpleFileStream::SimpleFileStream(const std::string& filename, int mode)
{
	open(filename, mode);
}

void glurg::SimpleFileStream::open(const std::string &filename, int mode)
{
	close();

	int ios_mode = std::ios::binary;
	if (mode & FileStream::mode_read)
	{
		ios_mode |= std::ios::in;
	}
	if (mode & FileStream::mode_write)
	{
		ios_mode |= std::ios_base::out;
	}

	this->file.open(filename, ios_mode);
}

void glurg::SimpleFileStream::close()
{
	this->file.close();
}

void glurg::SimpleFileStream::write(
	const std::uint8_t *data, std::size_t length)
{
	this->file.write((const char*)data, length);
}

void glurg::SimpleFileStream::read(
	std::uint8_t *data, std::size_t length)
{
	this->file.read((char*)data, length);
}

std::size_t glurg::SimpleFileStream::get_position()
{
	return this->file.tellg();
}

void glurg::SimpleFileStream::set_position(std::size_t position)
{
	this->file.seekg(position);
}

bool glurg::SimpleFileStream::get_is_end_of_file() const
{
	return this->file.eof();
}
