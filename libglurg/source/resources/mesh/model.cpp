// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstring>
#include <stdexcept>
#include "glurg/resources/mesh/model.hpp"

glurg::Model::Model()
{
	this->index_data_type = index_format_none;
	this->index_data_count = 0;
}

const glurg::MeshResource* glurg::Model::get_mesh_positions() const
{
	return this->positions.get();
}

void glurg::Model::set_mesh_positions(std::shared_ptr<MeshResource> value)
{
	this->positions = value;
}

const glurg::MeshResource* glurg::Model::get_mesh_normals() const
{
	return this->normals.get();
}

void glurg::Model::set_mesh_normals(std::shared_ptr<MeshResource> value)
{
	this->normals = value;
}

const glurg::MeshResource* glurg::Model::get_mesh_bones() const
{
	return this->bones.get();
}

void glurg::Model::set_mesh_bones(std::shared_ptr<MeshResource> value)
{
	this->bones = value;
}

const glurg::MeshResource* glurg::Model::get_mesh_weights() const
{
	return this->weights.get();
}

void glurg::Model::set_mesh_weights(std::shared_ptr<MeshResource> value)
{
	this->weights = value;
}

std::size_t glurg::Model::get_num_mesh_texture_coordinates() const
{
	return this->texture_coordinates.size();
}

void glurg::Model::set_num_mesh_texture_coordinates(std::size_t value)
{
	this->texture_coordinates.resize(value);
}

const glurg::MeshResource* glurg::Model::get_mesh_texture_coordinates(
	std::size_t index) const
{
	return this->texture_coordinates.at(index).get();
}

void glurg::Model::set_mesh_texture_coordinates(
	std::size_t index, std::shared_ptr<MeshResource> value)
{
	this->texture_coordinates.at(index) = value;
}

std::size_t glurg::Model::get_num_mesh_colors() const
{
	return this->colors.size();
}

void glurg::Model::set_num_mesh_colors(std::size_t value)
{
	this->colors.resize(value);
}

const glurg::MeshResource* glurg::Model::get_mesh_colors(std::size_t index)
{
	return this->colors.at(index).get();
}

void glurg::Model::set_mesh_colors(
	std::size_t index, std::shared_ptr<MeshResource> value)
{
	this->colors.at(index) = value;
}

int glurg::Model::get_index_data_type() const
{
	return this->index_data_type;
}

const std::uint8_t* glurg::Model::get_index_data() const
{
	return this->index_data.get();
}

std::size_t glurg::Model::get_index_data_count() const
{
	return this->index_data_count;
}

void glurg::Model::set_index_data(
	int type, const std::uint8_t* data, std::size_t count)
{
	std::size_t element_size;
	switch (type)
	{
		case index_format_none:
			element_size = 0;
			break;
		case index_format_unsigned_byte:
			element_size = 1;
			break;
		case index_format_unsigned_short:
			element_size = 2;
			break;
		case index_format_unsigned_integer:
			element_size = 4;
			break;
		default:
			throw std::runtime_error("invalid index format type");
	}

	this->index_data_type = type;
	if (element_size != 0)
	{
		const std::size_t num_bytes = count * element_size;

		this->index_data = std::make_unique<std::uint8_t[]>(num_bytes);
		this->index_data_count = count;

		std::memcpy(this->index_data.get(), data, num_bytes);
	}
}
