// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstdint>
#include <utility>
#include "glurg/trace/enumerationSignature.hpp"
#include "glurg/trace/traceFile.hpp"

glurg::trace::EnumerationSignature::ID
glurg::trace::EnumerationSignature::get_id() const
{
	return this->id;
}

void glurg::trace::EnumerationSignature::set_id(ID value)
{
	this->id = value;
}

const glurg::trace::Value*
glurg::trace::EnumerationSignature::get_value_by_name(
	const std::string& name) const
{
	auto value = this->values.find(name);
	if (value != this->values.end())
	{
		return value->second.get();
	}

	return nullptr;
}

void glurg::trace::EnumerationSignature::set_value_by_name(
	const std::string& name, std::shared_ptr<Value> value)
{
	this->values.insert(std::make_pair(name, value->clone()));
}

glurg::trace::EnumerationSignature* glurg::trace::EnumerationSignature::read(
	ID id, TraceFile& trace, FileStream& stream)
{
	EnumerationSignature* signature = new EnumerationSignature();
	signature->set_id(id);

	std::uint32_t count = trace.read_unsigned_integer(stream);
	for (std::uint32_t i = 0; i < count; ++i)
	{
		signature->set_value_by_name(
			trace.read_string(stream), trace.read_value(stream)->clone());
	}

	return signature;
}
