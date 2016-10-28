// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/mesh/model.hpp"

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
