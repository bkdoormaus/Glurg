// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cassert>
#include <memory>
#include <assimp/Exporter.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "glurg/resources/mesh/meshResource.hpp"
#include "glurg/resources/mesh/meshResourceBlob.hpp"
#include "glurg/resources/mesh/model.hpp"
#include "glurg/resources/mesh/modelExporter.hpp"

glurg::ModelExporter::ModelExporter()
{
	this->model = nullptr;
}

const glurg::Model* glurg::ModelExporter::get_model() const
{
	return this->model;
}

void glurg::ModelExporter::set_model(const Model* value)
{
	this->model = value;
}

void populate_positions(aiMesh* mesh, const glurg::MeshResource* positions)
{
	if (positions == nullptr)
	{
		throw std::runtime_error("model incomplete; has no vertex positions");
	}
	else
	{
		const std::size_t num_values = positions->get_num_values();
		mesh->mNumVertices = num_values;
		mesh->mVertices = new aiVector3D[num_values];

		for (std::size_t i = 0; i < num_values; ++i)
		{
			auto v = positions->get_value(i);
			mesh->mVertices[i].x = v.x;
			mesh->mVertices[i].y = v.y;
			mesh->mVertices[i].z = v.z;
		}
	}
}

void populate_index_data(aiMesh* mesh, const glurg::Model* model)
{
	if (model->get_index_data_type() != glurg::Model::index_format_none)
	{
		const std::size_t index_data_count =
			model->get_index_data_count();
		assert(index_data_count % 3 == 0);

		mesh->mNumFaces = index_data_count / 3;
		mesh->mFaces = new aiFace[mesh->mNumFaces];

		auto index_data = model->get_index_data();
		for (std::size_t i = 0; i < mesh->mNumFaces; ++i)
		{
			mesh->mFaces[i].mIndices = new unsigned int[3];
			mesh->mFaces[i].mNumIndices = 3;

			for (std::size_t j = 0; j < 3; ++j)
			{
				switch (model->get_index_data_type())
				{
					case glurg::Model::index_format_unsigned_byte:
						mesh->mFaces[i].mIndices[j] = *index_data;
						index_data += sizeof(std::uint8_t);
						break;
					case glurg::Model::index_format_unsigned_short:
						mesh->mFaces[i].mIndices[j] = 
							*((const std::uint16_t*)index_data);
						index_data += sizeof(std::uint16_t);
						break;
					case glurg::Model::index_format_unsigned_integer:
						mesh->mFaces[i].mIndices[j] = 
							*((const std::uint32_t*)index_data);
						index_data += sizeof(std::uint32_t);
						break;
					default:
						assert(false);
				}
			}
		}
	}
}

void populate_generic_data(
	std::size_t num_vertices,
	aiVector3D*& output_data, const glurg::MeshResource* input_data)
{
	if (input_data->get_num_values() != num_vertices)
	{
		throw std::runtime_error("vertex data size mismatch");
	}

	output_data = new aiVector3D[num_vertices];
	for (std::size_t i = 0; i < num_vertices; ++i)
	{
		auto v = input_data->get_value(i);
		output_data[i].x = v.x;
		output_data[i].y = v.y;
		output_data[i].z = v.z;
	}
}

void glurg::ModelExporter::to_dae(Buffer& output_buffer)
{
	if (this->model == nullptr)
	{
		throw std::runtime_error("no model to export");
	}

	std::unique_ptr<aiMesh> mesh(new aiMesh());
	const std::size_t num_vertices =
		this->model->get_mesh_positions()->get_num_values();

	populate_positions(mesh.get(), this->model->get_mesh_positions());
	populate_index_data(mesh.get(), this->model);

	if (this->model->get_mesh_normals() != nullptr)
	{
		populate_generic_data(
			num_vertices, mesh->mNormals, this->model->get_mesh_normals());
	}

	if (this->model->get_num_mesh_texture_coordinates() != 0)
	{
		const std::size_t num_texture_coordinates = 
			this->model->get_num_mesh_texture_coordinates();

		for (std::size_t i = 0; i < num_texture_coordinates; ++i)
		{
			auto texture_coordinates =
				this->model->get_mesh_texture_coordinates(i);
			if (texture_coordinates == nullptr)
			{
				throw std::runtime_error("expected valid texture coordinates");
			}

			mesh->mNumUVComponents[i] =
				((MeshResourceBlob*)texture_coordinates->get_blob())
					->get_vertex_description()
					.num_components;
			populate_generic_data(
				num_vertices, mesh->mTextureCoords[i], texture_coordinates);
		}
	}

	if (this->model->get_num_mesh_colors() != 0)
	{
		const std::size_t num_colors = this->model->get_num_mesh_colors();
		if (num_colors > AI_MAX_NUMBER_OF_COLOR_SETS)
		{
			throw std::runtime_error("number of color channels exceeds max");
		}

		for (std::size_t i = 0; i < num_colors; ++i)
		{
			auto colors = this->model->get_mesh_colors(i);
			if (colors == nullptr)
			{
				throw std::runtime_error("expect valid colors");
			}

			populate_generic_data(
				num_vertices, mesh->mTextureCoords[i], colors);
		}
	}

	std::unique_ptr<aiScene> scene(new aiScene());
	scene->mNumMeshes = 1;
	scene->mMeshes = new aiMesh*[1];
	scene->mMeshes[0] = mesh.release();

	std::unique_ptr<Assimp::Exporter> exporter(new Assimp::Exporter());
	auto blob = exporter->ExportToBlob(scene.get(), "dae", 0);

	output_buffer.insert(output_buffer.end(),
		(std::uint8_t*)blob->data, (std::uint8_t*)blob->data + blob->size);
}
