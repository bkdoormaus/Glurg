// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <memory>
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

glm::vec2 glurg::RenderValue::get_vector2(const std::string& name) const
{
	auto v = get_field_by_name(name);
	return v->to_vector2();
}

glm::vec3 glurg::RenderValue::to_vector3() const
{
	return glm::vec3(
		this->data.get(0u, 0.0f).asFloat(),
		this->data.get(1, 0.0f).asFloat(),
		this->data.get(2, 0.0f).asFloat());
}

glm::vec3 glurg::RenderValue::get_vector3(const std::string& name) const
{
	auto v = get_field_by_name(name);
	return v->to_vector3();
}


glm::vec4 glurg::RenderValue::to_vector4() const
{
	return glm::vec4(
		this->data.get(0u, 0.0f).asFloat(),
		this->data.get(1, 0.0f).asFloat(),
		this->data.get(2, 0.0f).asFloat(),
		this->data.get(3, 0.0f).asFloat());
}

glm::vec4 glurg::RenderValue::get_vector4(const std::string& name) const
{
	auto v = get_field_by_name(name);
	return v->to_vector4();
}

int glurg::RenderValue::to_integer() const
{
	return this->data.asInt();
}

int glurg::RenderValue::get_integer(const std::string& name) const
{
	auto v = get_field_by_name(name);
	return v->to_integer();
}

float glurg::RenderValue::to_single() const
{
	return this->data.asFloat();
}

float glurg::RenderValue::get_single(const std::string& name) const
{
	auto v = get_field_by_name(name);
	return v->to_single();
}

std::string glurg::RenderValue::to_string() const
{
	return this->data.asString();
}

std::string glurg::RenderValue::get_string(const std::string& name) const
{
	auto v = get_field_by_name(name);
	return v->to_string();
}

void glurg::RenderValue::extract_blob(Blob& blob) const
{
	decode_base64(this->data.asString(), blob);
}

void glurg::RenderValue::extract_field_blob(
	const std::string &name, Blob& blob) const
{
	auto v = get_field_by_name(name);
	v->extract_blob(blob);
}

std::shared_ptr<glurg::RenderValue> glurg::RenderValue::get_field_by_name(
	const std::string& name) const
{
	return std::make_shared<RenderValue>(this->data[name]);
}

std::shared_ptr<glurg::RenderValue> glurg::RenderValue::get_field_by_name(
	const std::string& name, std::size_t index) const
{
	return std::make_shared<RenderValue>(this->data[name + std::to_string(index)]);
}

std::shared_ptr<glurg::RenderValue> glurg::RenderValue::get_field_by_index(
	std::size_t index) const
{
	return std::make_shared<RenderValue>(this->data[(Json::Value::ArrayIndex)index]);
}

bool glurg::RenderValue::has_field(const std::string& name) const
{
	return this->data.isMember(name);
}

bool glurg::RenderValue::has_field(
	const std::string& name, std::size_t index) const
{
	return this->data.isMember(name + std::to_string(index));
}

bool glurg::RenderValue::has_field(std::size_t index) const
{
	return this->data.isValidIndex((Json::Value::ArrayIndex)index);
}
