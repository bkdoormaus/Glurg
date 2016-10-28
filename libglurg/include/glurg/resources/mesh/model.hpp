// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MODEL_HPP
#define LIBGLURG_RESOURCES_MESH_MODEL_HPP

#include <memory>
#include <vector>

namespace glurg
{
	class MeshResource;

	class Model
	{
	public:
		Model();
		~Model() = default;

		const MeshResource* get_mesh_positions() const;
		void set_mesh_positions(std::shared_ptr<MeshResource> value);

		const MeshResource* get_mesh_normals() const;
		void set_mesh_normals(std::shared_ptr<MeshResource> value);

		const MeshResource* get_mesh_bones() const;
		void set_mesh_bones(std::shared_ptr<MeshResource> value);

		const MeshResource* get_mesh_weights() const;
		void set_mesh_weights(std::shared_ptr<MeshResource> value);

		std::size_t get_num_mesh_texture_coordinates() const;
		void set_num_mesh_texture_coordinates(std::size_t value);

		const MeshResource* get_mesh_texture_coordinates(
			std::size_t index) const;
		void set_mesh_texture_coordinates(
			std::size_t index, std::shared_ptr<MeshResource> value);

		std::size_t get_num_mesh_colors() const;
		void set_num_mesh_colors(std::size_t value);

		const MeshResource* get_mesh_colors(std::size_t index) const;
		void set_mesh_colors(std::size_t index, std::shared_ptr<MeshResource> value);

		enum
		{
			index_format_none,
			index_format_unsigned_byte,
			index_format_unsigned_short,
			index_format_unsigned_integer
		};
		int get_index_data_type() const;
		const std::uint8_t* get_index_data() const;
		std::size_t get_index_data_count() const;

		void set_index_data(
			int type, const std::uint8_t* data, std::size_t count);

	private:
		typedef std::shared_ptr<MeshResource> MeshResourcePointer;
		MeshResourcePointer positions;
		MeshResourcePointer normals;
		MeshResourcePointer bones;
		MeshResourcePointer weights;
		std::vector<MeshResourcePointer> texture_coordinates;
		std::vector<MeshResourcePointer> colors;

		int index_data_type;
		std::unique_ptr<std::uint8_t[]> index_data;
		std::size_t index_data_count;
	};
}

#endif
