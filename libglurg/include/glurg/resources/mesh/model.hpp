// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MODEL_HPP
#define LIBGLURG_RESOURCES_MESH_MODEL_HPP

namespace glurg
{
	class Model
	{
	public:
		Model() = default;
		~Model() = default;

		const MeshResource* get_mesh_positions() const;
		void set_mesh_positions(const MeshResource* value);

		const MeshResource* get_mesh_normals() const;
		void set_mesh_normals(const MeshResource* value);

		const MeshResource* get_mesh_bones() const;
		void set_mesh_bones(const MeshResource* value) const;

		const MeshResource* get_mesh_weights() const;
		void set_mesh_weights(const MeshResource* value) const;

		std::size_t get_num_mesh_texture_coordinates() const;
		void set_num_mesh_texture_coordinates(std::size_t value);

		const MeshResource* get_mesh_texture_coordinates(
			std::size_t index) const;
		void set_mesh_texture_coordinates(
			std::size_t index, const MeshResource* value);

		std::size_t get_num_colors() const;
		void set_num_colors(std::size_t value);

		const MeshResource* get_mesh_colors(std::size_t index);
		void set_mesh_colors(std::size_t index, const MeshResource* value);

		const MeshResource

	private:
		const MeshResource* positions;
		const MeshResource* normals;
		const MeshResource* bones;
		const MeshResource* weights;
		std::vector<const MeshResource*> texture_coordinates;
		std::vector<const MeshResource*> colors;
	};
}

#endif
