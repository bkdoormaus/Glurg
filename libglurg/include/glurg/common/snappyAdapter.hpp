// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_COMMON_SNAPPY_ADAPTER_HPP
#define LIBGLURG_COMMON_SNAPPY_ADAPTER_HPP

#include <stdexcept>
#include <type_traits>
#include <list>
#include "glurg/common/fileStream.hpp"

namespace glurg
{
	class SnappyAdapterBase
	{
	public:
		~SnappyAdapterBase();

	protected:
		explicit SnappyAdapterBase(std::size_t block_size);

		void snappy_write(
			FileStream* stream, const std::uint8_t* data, std::size_t length);
		void snappy_read(
			FileStream* stream, std::uint8_t* data, std::size_t length);

		void snappy_seek(FileStream* stream, std::size_t virtual_offset);
		std::size_t snappy_tell();

		void snappy_flush_write_buffer(FileStream* stream);
		void snappy_reset_state();

	private:
		bool snappy_fetch_read_buffer(FileStream* stream);
		void snappy_add_block(
			std::size_t physical_position, std::size_t physical_size,
			std::size_t virtual_size);
		void snappy_get_last_block_info(
			std::size_t& max_physical_position,
			std::size_t& max_virtual_position);

		struct snappy_block
		{
			std::size_t physical_position;
			std::size_t physical_size;
			std::size_t virtual_position;
			std::size_t virtual_size;
		};
		std::list<snappy_block> snappy_blocks;

		std::size_t snappy_current_block_virtual_position;

		std::uint8_t* snappy_output_buffer;
		std::size_t snappy_output_buffer_size;

		std::uint8_t* snappy_write_buffer;
		std::size_t max_snappy_write_buffer_size;
		std::size_t cur_snappy_write_buffer_size;

		std::uint8_t* snappy_read_buffer;
		std::size_t snappy_read_buffer_size;
		std::size_t snappy_read_buffer_offset;
	};

	template <typename FileStreamImpl>
	class SnappyAdapter :
		public SnappyAdapterBase,
		public std::enable_if<std::is_base_of<FileStream, FileStreamImpl>::value, FileStreamImpl>::type
	{
	public:
		// Default compressed block size, in bytes.
		static const std::size_t DEFAULT_BLOCK_SIZE = 1 * 1024 * 1024;
		explicit SnappyAdapter(std::size_t block_size = DEFAULT_BLOCK_SIZE);
		~SnappyAdapter() = default;

		void write(const std::uint8_t* data, std::size_t length) override;
		void read(std::uint8_t* data, std::size_t length) override;

		std::size_t get_position() override;
		void set_position(std::size_t position) override;
		
		void close() override;

	private:
		// At least as of 7.1, apitrace's trace files insert these characters at
		// the beginning of a trace file using Snappy compression.
		static const char MAGIC_BYTE_1 = 'a';
		static const char MAGIC_BYTE_2 = 't';

		struct snappy_scope_lock
		{
			snappy_scope_lock(SnappyAdapter<FileStreamImpl>& snappy_adapter) :
				snappy_adapter(snappy_adapter)
			{
				this->snappy_adapter.use_filestream_impl = true;
			}

			~snappy_scope_lock()
			{
				this->snappy_adapter.use_filestream_impl = false;
			}

			SnappyAdapter<FileStreamImpl>& snappy_adapter;
		};

		bool is_new;
		bool use_filestream_impl;
	};
}

template <typename FileStreamImpl>
glurg::SnappyAdapter<FileStreamImpl>::SnappyAdapter(std::size_t block_size)
	: SnappyAdapterBase(block_size)
{
	this->is_new = true;
	this->use_filestream_impl = false;
}

template <typename FileStreamImpl>
void glurg::SnappyAdapter<FileStreamImpl>::write(
	const std::uint8_t* data, std::size_t length)
{
	if (this->is_new)
	{
		char magic[] = { MAGIC_BYTE_1, MAGIC_BYTE_2 };
		FileStreamImpl::write(magic, sizeof(magic));

		this->is_new = false;
	}

	if (this->use_filestream_impl)
	{
		FileStreamImpl::write(data, length);
	}
	else
	{
		snappy_scope_lock lock(*this);
		snappy_write(this, data, length);
	}
}

template <typename FileStreamImpl>
void glurg::SnappyAdapter<FileStreamImpl>::read(
	std::uint8_t *data, std::size_t length)
{
	if (this->is_new)
	{
		char magic[2];
		FileStreamImpl::read((char*)magic, 2);

		if (magic[0] != MAGIC_BYTE_1 || magic[1] != MAGIC_BYTE_2)
		{
			throw std::runtime_error("magic bytes are incorrect");
		}

		this->is_new = false;
	}

	if (this->use_filestream_impl)
	{
		FileStreamImpl::read(data, length);
	}
	else
	{
		snappy_scope_lock lock(*this);
		snappy_read(this, data, length);
	}
}

template <typename FileStreamImpl>
std::size_t glurg::SnappyAdapter<FileStreamImpl>::get_position()
{
	if (this->use_filestream_impl)
	{
		return FileStreamImpl::get_position();
	}
	else
	{
		snappy_scope_lock lock(*this);
		return snappy_tell();
	}
}

template <typename FileStreamImpl>
void glurg::SnappyAdapter<FileStreamImpl>::set_position(std::size_t position)
{
	if (FileStreamImpl::get_mode() & FileStream::mode_write)
	{
		// It's simply too complex to handle seeking and reading/writing
		// (inclusive), so only allow seeking when the file stream mode is read.
		throw std::runtime_error("cannot seek when writing Snappy file");
	}

	if (this->use_filestream_impl)
	{
		return FileStreamImpl::get_position();
	}
	else
	{
		snappy_scope_lock lock(*this);
		snappy_seek(position);
	}
}

template <typename FileStreamImpl>
void glurg::SnappyAdapter<FileStreamImpl>::close()
{
	snappy_flush_write_buffer(this);
	snappy_reset_state();

	this->is_new = true;
	this->use_filestream_impl = false;

	FileStreamImpl::close();
}

#endif
