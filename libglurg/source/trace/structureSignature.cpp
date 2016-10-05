// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <algorithm>
#include "glurg/trace/structureSignature.hpp"
#include "glurg/trace/traceFile.hpp"

glurg::StructureSignature::ID glurg::StructureSignature::get_id() const
{
	return this->id;
}

std::size_t glurg::StructureSignature::get_num_fields() const
{
	return fields.size();
}

std::string glurg::StructureSignature::get_field_name(std::size_t index) const
{
	return fields.at(index);
}

bool glurg::StructureSignature::has_field_name(const std::string& name) const
{
	auto begin = this->fields.begin();
	auto end = this->fields.end();
	
	return std::find(begin, end, name) != end;
}

glurg::StructureSignature* glurg::StructureSignature::read(
	ID id, TraceFile& trace, FileStream& stream)
{
	StructureSignature* signature = new StructureSignature();
	signature->id = id;

	std::size_t count = trace.read_unsigned_integer(stream);
	signature->set_num_fields(count);

	for (std::size_t i = 0; i < count; ++i)
	{
		signature->set_field_name(i, trace.read_string(stream));
	}

	return signature;
}

void glurg::StructureSignature::set_num_fields(std::size_t value)
{
	fields.resize(value);
}

void glurg::StructureSignature::set_field_name(
	std::size_t index, const std::string& value)
{
	fields.at(index) = value;
}
