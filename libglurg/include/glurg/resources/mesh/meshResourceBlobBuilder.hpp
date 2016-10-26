// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MESH_RESOURCE_BLOB_BUILDER_HPP
#define LIBGLURG_RESOURCES_MESH_MESH_RESOURCE_BLOB_BUILDER_HPP

#include <memory>
#include "glurg/resources/resourceBlobBuilder.hpp"
#include "glurg/resources/mesh/meshResourceBlob.hpp"

namespace glurg
{
	class MeshResourceBlobBuilder :
		public ResourceBlobBuilder<MeshResourceBlob>
	{
	public:
		MeshResourceBlobBuilder();
		~MeshResourceBlobBuilder() = default;

		void set_vertex_description_format(int value);
		void set_vertex_description_num_components(int value);
		void set_vertex_description_normalized(int value);
		void set_vertex_description_stride(int value);
		void set_vertex_description_offset(int value);

		void set_vertex_data(const std::uint8_t* vertex_data, std::size_t size);

		MeshResourceBlob* build();

	private:
		static void verify_vertex_description(
			const VertexDescription& description);

		VertexDescription vertex_description;

		std::unique_ptr<std::uint8_t[]> vertex_data;
		std::size_t vertex_data_size;
	};
}

#endif
