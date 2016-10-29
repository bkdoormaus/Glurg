// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/mesh/meshResourceBlob.hpp"

glurg::MeshResourceBlob::MeshResourceBlob(ResourceBlobReadBuffer&& buffer)
	: buffer(std::move(buffer))
{
	this->hash = Hash::hash(
		this->buffer.get_buffer(), this->buffer.get_buffer_size());

	this->vertex_description.format = this->buffer.pop_value<int>();
	this->vertex_description.num_components = this->buffer.pop_value<int>();
	this->vertex_description.normalized = this->buffer.pop_value<int>();
	this->vertex_description.stride = this->buffer.pop_value<int>();
	this->vertex_description.offset = this->buffer.pop_value<int>();
	this->vertex_data = this->buffer.pop_data(this->vertex_data_size);
}

glurg::VertexDescription glurg::MeshResourceBlob::get_vertex_description() const
{
	return this->vertex_description;
}

const std::uint8_t* glurg::MeshResourceBlob::get_vertex_data() const
{
	return this->vertex_data;
}

std::size_t glurg::MeshResourceBlob::get_vertex_data_size() const
{
	return this->vertex_data_size;
}

const glurg::Hash& glurg::MeshResourceBlob::get_hash() const
{
	return this->hash;
}

const std::uint8_t* glurg::MeshResourceBlob::get_data() const
{
	return this->buffer.get_buffer();
}

std::size_t glurg::MeshResourceBlob::get_size() const
{
	return this->buffer.get_buffer_size();
}
