// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/trace/structure.hpp"
#include "glurg/trace/structureSignature.hpp"
#include "glurg/trace/values.hpp"

glurg::Structure::Structure(const StructureSignature* signature)
{
	this->signature = signature;
}

const glurg::StructureSignature* glurg::Structure::get_signature() const
{
	return this->signature;
}

const glurg::Value* glurg::Structure::get_field_by_name(
	const std::string& name) const
{
	auto value = this->fields.find(name);
	if (value != this->fields.end())
	{
		return value->second.get();
	}

	return nullptr;
}

const glurg::Value* glurg::Structure::get_field_by_index(
	std::size_t index) const
{
	auto name = this->signature->get_field_name(index);
	return get_field_by_name(name);
}

void glurg::Structure::set_field_by_name(
	const std::string& name, const glurg::Value* value)
{
	if (this->signature->has_field_name(name))
	{
		this->fields.insert(std::make_pair(name, ValuePointer(value->clone())));
	}
}

void glurg::Structure::set_field_by_index(
	std::size_t index, const glurg::Value* value)
{
	auto name = this->signature->get_field_name(index);
	this->fields.insert(std::make_pair(name, ValuePointer(value->clone())));
}
