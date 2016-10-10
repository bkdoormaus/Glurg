// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/resourceBlobBuffer.hpp"

const std::uint8_t* glurg::ResourceBlobBuffer::get_buffer() const
{
	if (this->buffer.size() == 0)
		return nullptr;

	return &this->buffer[0];
}

std::size_t glurg::ResourceBlobBuffer::get_buffer_size() const
{
	return this->buffer.size();
}

void glurg::ResourceBlobWriteBuffer::push_data(
	const std::uint8_t* data, std::size_t size)
{
	push_value(size);
	this->buffer.insert(this->buffer.end(), data, data + size + 1);
}

void glurg::ResourceBlobWriteBuffer::push_string(const std::string& string)
{
	push_data((const std::uint8_t*)string.c_str(), string.size());
}

glurg::ResourceBlobReadBuffer::ResourceBlobReadBuffer(
	const std::uint8_t* data, std::size_t size)
{
	this->buffer.insert(this->buffer.end(), data, data + size + 1);
}

glurg::ResourceBlobReadBuffer::ResourceBlobReadBuffer(
	ResourceBlobReadBuffer&& other)
{
	this->buffer = std::move(other.buffer);
	this->current_offset = other.current_offset;
}

const std::uint8_t* glurg::ResourceBlobReadBuffer::pop_data(
	std::size_t& return_size)
{
	return_size = pop_value<std::size_t>();

	std::size_t offset = this->current_offset;
	this->current_offset += return_size;

	return &this->buffer.at(offset);
}

std::string glurg::ResourceBlobReadBuffer::pop_string()
{
	std::size_t length = 0;
	auto data = pop_data(length);

	std::string result;
	result.append((const char*)data, (const char*)data + length);

	return result;
}
