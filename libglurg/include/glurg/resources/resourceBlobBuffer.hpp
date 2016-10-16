// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_RESOURCE_BLOB_BUFFER_HPP
#define LIBGLURG_RESOURCES_RESOURCE_BLOB_BUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <vector>

namespace glurg
{
	class ResourceBlobBuffer
	{
	public:
		virtual ~ResourceBlobBuffer() = default;

		const std::uint8_t* get_buffer() const;
		std::size_t get_buffer_size() const;

	protected:
		std::vector<std::uint8_t> buffer;
	};

	class ResourceBlobReadBuffer;
	class ResourceBlobWriteBuffer : public ResourceBlobBuffer
	{
	public:
		ResourceBlobWriteBuffer() = default;
		~ResourceBlobWriteBuffer() = default;

		template <typename Type>
		void push_value(const Type& type);

		void push_data(const std::uint8_t* data, std::size_t size);
		void push_string(const std::string& string);

		void to_read_buffer(ResourceBlobReadBuffer& other);
	};

	class ResourceBlobReadBuffer : public ResourceBlobBuffer
	{
		friend ResourceBlobWriteBuffer;

	public:
		ResourceBlobReadBuffer();
		ResourceBlobReadBuffer(const std::uint8_t* data, std::size_t size);
		ResourceBlobReadBuffer(ResourceBlobReadBuffer&& other);
		~ResourceBlobReadBuffer() = default;

		template <typename Type>
		Type pop_value();

		const std::uint8_t* pop_data(std::size_t& return_size);
		std::string pop_string();

	private:
		std::size_t current_offset;
	};
}

template <typename Type>
void glurg::ResourceBlobWriteBuffer::push_value(const Type& type)
{
	const std::uint8_t* v = (const std::uint8_t*)&type;
	this->buffer.insert(this->buffer.end(), v, v + sizeof(Type));
}

template <typename Type>
Type glurg::ResourceBlobReadBuffer::pop_value()
{
	std::size_t offset = current_offset;
	this->current_offset += sizeof(Type);

	if (this->current_offset >= this->buffer.size())
	{
		throw std::runtime_error("pop value result too large for buffer");
	}

	return *((Type*)(&this->buffer.at(offset)));
}

#endif
