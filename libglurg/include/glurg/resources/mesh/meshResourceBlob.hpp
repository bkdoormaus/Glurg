// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MESH_RESOURCE_BLOB_HPP
#define LIBGLURG_RESOURCES_MESH_MESH_RESOURCE_BLOB_HPP

#include "glurg/common/hash.hpp"
#include "glurg/resources/resourceBlob.hpp"
#include "glurg/resources/resourceBlobBuffer.hpp"

namespace glurg
{
	struct VertexDescription
	{
		enum
		{
			format_none,
			format_float,
			format_byte,
			format_unsigned_byte,
			format_short,
			format_unsigned_short,
			format_integer,
			format_unsigned_integer
		};
		int format;

		int num_components;

		bool normalized;

		int stride;

		int offset;
	};

	class MeshResourceBlob
	{
	public:
		MeshResourceBlob(ResourceBlobReadBuffer&& buffer);
		~MeshResourceBlob() = default;

		VertexDescription get_vertex_description() const;

		const std::uint8_t* get_vertex_data() const;
		std::size_t get_vertex_data_size() const;

		const Hash& get_hash() const;
		const std::uint8_t* get_data() const;
		std::size_t get_size() const;

	private:
		VertexDescription vertex_description;

		const std::uint8_t* vertex_data;
		std::size_t vertex_data_size;

		Hash hash;
		ResourceBlobReadBuffer buffer;
	};
}

#endif
