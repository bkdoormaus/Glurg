// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MESH_RESOURCE_HPP
#define LIBGLURG_RESOURCES_MESH_MESH_RESOURCE_HPP

#include <glm/vec4.hpp>
#include "glurg/resources/resource.hpp"

namespace glurg
{
	class MeshResourceBlob;

	class MeshResource : public Resource
	{
	public:
		MeshResource(const MeshResourceBlob* blob);
		~MeshResource() = default;

		std::size_t get_num_values() const;
		glm::vec4 get_value(std::size_t index) const;
		void get_values(
			std::size_t index, std::size_t count, glm::vec4 output[]) const;

		const ResourceBlob* get_blob() const;

	private:
		const MeshResourceBlob* blob;
	};
};

#endif
