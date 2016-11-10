// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <limits>
#include <glm/gtc/type_ptr.hpp>
#include "glurg/resources/mesh/meshResource.hpp"
#include "glurg/resources/mesh/meshResourceBlob.hpp"

glurg::MeshResource::MeshResource(const MeshResourceBlob* blob)
{
	this->blob = blob;
}

std::size_t glurg::MeshResource::get_num_values() const
{
	std::size_t size = this->blob->get_vertex_data_size();
	auto description = this->blob->get_vertex_description();

	return size / description.stride;
}

glm::vec4 glurg::MeshResource::get_value(std::size_t index) const
{
	glm::vec4 v;
	get_values(index, 1, &v);

	return v;
}

template<typename Integer>
float glurg_next_integer_value(
	const std::uint8_t*& input_data, bool normalized)
{
	Integer v = *((const Integer*)input_data);
	input_data += sizeof(Integer);

	if (normalized)
	{
		if (v < 0)
		{
			return v * (-1.0f / std::numeric_limits<Integer>::min());
		}
		else if (v > 0)
		{
			return v * (1.0f / std::numeric_limits<Integer>::max());
		}
	}

	return v;
}

void glurg::MeshResource::get_values(
	std::size_t index, std::size_t count, glm::vec4 output[]) const
{
	if (index + count > get_num_values())
	{
		throw std::runtime_error("requested values exceeds bounds");
	}

	auto description = this->blob->get_vertex_description();
	const std::uint8_t* data =
		this->blob->get_vertex_data() + description.offset;
	for (std::size_t i = 0; i < count; ++i)
	{
		const std::uint8_t* element = data + ((i + index) * description.stride);

		glm::vec4 result(0.0f, 0.0f, 0.0f, 0.0f);
		auto r = glm::value_ptr(result);
		for (std::size_t j = 0; j < description.num_components; ++j)
		{
			bool n = (description.normalized ==
				VertexDescription::normalization_enabled);
			switch (description.format)
			{
				case VertexDescription::format_float:
					r[j] = *((const float*)element);
					element += sizeof(float);
					break;
				case VertexDescription::format_byte:
					r[j] = glurg_next_integer_value<std::int8_t>(element, n);
					break;
				case VertexDescription::format_unsigned_byte:
					r[j] = glurg_next_integer_value<std::uint8_t>(element, n);
					break;
				case VertexDescription::format_short:
					r[j] = glurg_next_integer_value<std::int16_t>(element, n);
					break;
				case VertexDescription::format_unsigned_short:
					r[j] = glurg_next_integer_value<std::uint16_t>(element, n);
					break;
				case VertexDescription::format_integer:
					r[j] = glurg_next_integer_value<std::int32_t>(element, n);
					break;
				case VertexDescription::format_unsigned_integer:
					r[j] = glurg_next_integer_value<std::uint32_t>(element, n);
					break;
				default:
					assert(false);
			}

			// Sanitize.
			if (std::isnan(r[j]) || std::isinf(r[j]))
			{
				r[j] = 0.0f;
			}
		}

		output[i] = result;
	}
}

const glurg::ResourceBlob* glurg::MeshResource::get_blob() const
{
	return this->blob;
}
