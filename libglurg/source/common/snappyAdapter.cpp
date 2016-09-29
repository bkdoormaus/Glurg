// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <snappy.h>
#include "glurg/common/snappyAdapter.hpp"

glurg::SnappyAdapterBase::SnappyAdapterBase(std::size_t block_size)
{
	this->snappy_output_buffer_size = snappy::MaxCompressedLength(block_size);
	this->snappy_output_buffer =
		new std::uint8_t[this->snappy_output_buffer_size];

	this->snappy_write_buffer = new std::uint8_t[block_size];
	this->cur_snappy_write_buffer_size = 0;
	this->max_snappy_write_buffer_size = block_size;

	this->snappy_read_buffer = nullptr;
	this->snappy_read_buffer_size = 0;
	this->snappy_read_buffer_offset = 0;
}

glurg::SnappyAdapterBase::~SnappyAdapterBase()
{
	delete[] this->snappy_output_buffer;
	delete[] this->snappy_write_buffer;

	if (this->snappy_read_buffer != nullptr)
		delete[] this->snappy_read_buffer;
}

void glurg::SnappyAdapterBase::snappy_write(
	glurg::FileStream* stream, const std::uint8_t* data, std::size_t length)
{
	std::size_t requested_size = this->cur_snappy_write_buffer_size + length;
	std::size_t current_offset = 0;
	while (requested_size > this->max_snappy_write_buffer_size)
	{
		std::size_t write_buffer_remainder =
			this->max_snappy_write_buffer_size - this->cur_snappy_write_buffer_size;
		std::memcpy(
			this->snappy_write_buffer + this->cur_snappy_write_buffer_size,
			data + current_offset,
			write_buffer_remainder);

		snappy_flush_write_buffer(stream);
		this->cur_snappy_write_buffer_size = 0;

		requested_size -= this->max_snappy_write_buffer_size;
	}

	std::size_t final_remainder = length - current_offset;
	std::memcpy(
		this->snappy_write_buffer + this->cur_snappy_write_buffer_size,
		data + current_offset,
		final_remainder);
	this->cur_snappy_write_buffer_size += final_remainder;
}

void glurg::SnappyAdapterBase::snappy_read(
	glurg::FileStream* stream, std::uint8_t* data, std::size_t length)
{
	std::size_t current_offset = 0;
	while (current_offset < length)
	{
		std::size_t read_buffer_remainder =
			this->snappy_read_buffer_size - this->snappy_read_buffer_offset;

		if (read_buffer_remainder == 0)
		{
			if (!snappy_fetch_read_buffer(stream))
			{
				return;
			}
		}
		else
		{
			std::size_t l = std::min(
				length - current_offset, read_buffer_remainder);

			std::memcpy(
				data + current_offset,
				this->snappy_read_buffer + this->snappy_read_buffer_offset,
				read_buffer_remainder);

			this->snappy_read_buffer_offset += l;
			current_offset += l;
		}
	}
}

void glurg::SnappyAdapterBase::snappy_flush_write_buffer(
	glurg::FileStream* stream)
{
	if (this->cur_snappy_write_buffer_size == 0)
	{
		return;
	}

	std::size_t output_size;
	snappy::RawCompress(
		(char*)this->snappy_write_buffer, this->cur_snappy_write_buffer_size,
		(char*)this->snappy_output_buffer, &output_size);

	std::uint32_t header = (std::uint32_t)output_size;
	stream->write((std::uint8_t*)&header, sizeof(std::uint32_t));
	stream->write(this->snappy_output_buffer, output_size);
}

bool glurg::SnappyAdapterBase::snappy_fetch_read_buffer(glurg::FileStream* stream)
{
	std::uint32_t header = 0;
	stream->read((std::uint8_t*)&header, sizeof(std::uint32_t));

	if (stream->get_is_end_of_file())
	{
		return false;
	}
	else if (header == 0)
	{
		throw std::runtime_error("invalid block size 0");
	}

	typedef std::unique_ptr<uint8_t> byte_pointer;
	byte_pointer compressed_buffer(new std::uint8_t[header]);
	stream->read(compressed_buffer.get(), header);

	std::size_t read_buffer_size = 0;
	if (!snappy::GetUncompressedLength(
		(const char*)compressed_buffer.get(), header,
		&read_buffer_size))
	{
		throw std::runtime_error("couldn't get read buffer size");
	}

	std::uint8_t* read_buffer = new std::uint8_t[header];
	if (!snappy::RawUncompress(
		(char*)compressed_buffer.get(), header,
		(char*)read_buffer))
	{
		delete[] read_buffer;
		throw std::runtime_error("couldn't decode read buffer");
	}

	this->snappy_read_buffer = read_buffer;
	this->snappy_read_buffer_offset = 0;
	this->snappy_read_buffer_size = read_buffer_size;

	return true;
}
