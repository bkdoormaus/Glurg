// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstring>
#include "glurg/resources/mesh/meshResourceBlobBuilder.hpp"

glurg::MeshResourceBlobBuilder::MeshResourceBlobBuilder()
{
	this->vertex_description.format = VertexDescription::format_none;
	this->vertex_description.num_components = -1;
	this->vertex_description.normalized = VertexDescription::normalization_none;
	this->vertex_description.stride = -1;
	this->vertex_description.offset = -1;
}

void glurg::MeshResourceBlobBuilder::set_vertex_description_format(int value)
{
	this->vertex_description.format = value;
}

void glurg::MeshResourceBlobBuilder::set_vertex_description_num_components(
	int value)
{
	this->vertex_description.num_components = value;
}

void glurg::MeshResourceBlobBuilder::set_vertex_description_normalized(
	int value)
{
	this->vertex_description.normalized = value;
}

void glurg::MeshResourceBlobBuilder::set_vertex_description_stride(int value)
{
	this->vertex_description.stride = value;
}

void glurg::MeshResourceBlobBuilder::set_vertex_description_offset(int value)
{
	this->vertex_description.offset = value;
}

void glurg::MeshResourceBlobBuilder::set_vertex_data(
	const std::uint8_t* vertex_data, std::size_t size)
{
	this->vertex_data = std::make_unique<std::uint8_t[]>(size);
	this->vertex_data_size = size;

	std::memcpy(this->vertex_data.get(), vertex_data, size);
}

glurg::MeshResourceBlob* glurg::MeshResourceBlobBuilder::build()
{
	ResourceBlobWriteBuffer buffer;
	buffer.push_value(this->vertex_description.format);
	buffer.push_value(this->vertex_description.num_components);
	buffer.push_value(this->vertex_description.normalized);
	buffer.push_value(this->vertex_description.stride);
	buffer.push_value(this->vertex_description.offset);
	buffer.push_data(this->vertex_data.get(), this->vertex_data_size);

	ResourceBlobReadBuffer result;
	buffer.to_read_buffer(result);

	return new MeshResourceBlob(std::move(result));
}
