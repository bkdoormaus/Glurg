// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_RESOURCE_BLOB_BUFFER_HPP
#define LIBGLURG_RESOURCES_RESOURCE_BLOB_BUFFER_HPP

namespace glurg
{
	class ResourceBlobWriteBuffer
	{
	public:
		ResourceBlobWriteBuffer() = default;
		~ResourceBlobWriteBuffer() = default;

		const std::uint8_t* get_buffer() const;
		std::size_t get_buffer_size() const;

		template <typename Type>
		void push_value(const Type& type);

		void push_data(const std::uint8_t* data, std::size_t size);
		void push_string(const std::string& string);

	private:
		std::vector<std::uint8_t> buffer;
		std::size_t current_size;
	};

	class ResourceBlobReadBuffer
	{
	public:
		ResourceBlobReadBuffer(const std::uint8_t* data, std::size_t size);
		~ResourceBlobReadBuffer() = default;

		const std::uint8_t* get_buffer() const;
		std::size_t get_buffer_size() const;

		template <typename Type>
		void pop_value(Type& type);

		void pop_data(std::uint8_t* data, std::size_t size);
		void pop_string(const std::string& string);

	private:
		std::vector<std::uint8_t> buffer;
		std::size_t current_size;
		std::size_t current_offset;
	};
}

#endif
