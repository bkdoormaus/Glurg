// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstdint>
#include "glurg/trace/bitmaskSignature.hpp"
#include "glurg/trace/traceFile.hpp"

glurg::BitmaskSignature::ID glurg::BitmaskSignature::get_id() const
{
	return this->id;
}

void glurg::BitmaskSignature::set_id(ID value)
{
	this->id = value;
}

bool glurg::BitmaskSignature::match(
	std::uint32_t value, const std::string& mask) const
{
	auto m = this->values.find(mask);
	if (m != this->values.end())
	{
		return (value & m->second) == m->second;
	}

	return false;
}

void glurg::BitmaskSignature::set_mask_by_name(
	const std::string& name, std::uint32_t mask)
{
	this->values.insert(std::make_pair(name, mask));
}

glurg::BitmaskSignature* glurg::BitmaskSignature::read(
	ID id, TraceFile& trace, FileStream& stream)
{
	BitmaskSignature* signature = new BitmaskSignature();
	signature->set_id(id);

	std::uint32_t count = trace.read_unsigned_integer(stream);
	for (std::uint32_t i = 0; i < count; ++i)
	{
		std::string name = trace.read_string(stream);
		Value* value = trace.read_value(stream);

		signature->set_mask_by_name(name, value->to_unsigned_integer());
	}

	return signature;
}
