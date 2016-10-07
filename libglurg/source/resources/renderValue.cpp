// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/common/base64.hpp"
#include "glurg/resources/renderValue.hpp"

glurg::RenderValue::RenderValue(const Json::Value& data)
{
	this->data = data;
}

glm::vec2 glurg::RenderValue::to_vector2() const
{
	return glm::vec2(
		this->data.get(0u, 0.0f).asFloat(), this->data.get(1, 0.0f).asFloat());
}

glm::vec3 glurg::RenderValue::to_vector3() const
{
	return glm::vec3(
		this->data.get(0u, 0.0f).asFloat(),
		this->data.get(1, 0.0f).asFloat(),
		this->data.get(2, 0.0f).asFloat());
}


glm::vec4 glurg::RenderValue::to_vector4() const
{
	return glm::vec4(
		this->data.get(0u, 0.0f).asFloat(),
		this->data.get(1, 0.0f).asFloat(),
		this->data.get(2, 0.0f).asFloat(),
		this->data.get(3, 0.0f).asFloat());
}

int glurg::RenderValue::to_integer() const
{
	return this->data.asInt();
}

float glurg::RenderValue::to_single() const
{
	return this->data.asFloat();
}

std::string glurg::RenderValue::to_string() const
{
	return this->data.asString();
}

void glurg::RenderValue::extract_blob(Blob& blob) const
{
	return decode_base64(this->data.asString(), blob);
}

glurg::RenderValue* glurg::RenderValue::get_field_by_name(
	const std::string& name) const
{
	return new RenderValue(this->data[name]);
}

glurg::RenderValue* glurg::RenderValue::get_field_by_name(
	const std::string& name, std::size_t index) const
{
	return new RenderValue(this->data[name + std::to_string(index)]);
}

glurg::RenderValue* glurg::RenderValue::get_field_by_index(
	std::size_t index) const
{
	return new RenderValue(this->data[(Json::Value::ArrayIndex)index]);
}
