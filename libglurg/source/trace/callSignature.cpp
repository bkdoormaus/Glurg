// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstdint>
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/traceFile.hpp"

glurg::trace::CallSignature::ID glurg::trace::CallSignature::get_id() const
{
	return this->id;
}

void glurg::trace::CallSignature::set_id(ID value)
{
	this->id = value;
}

const std::string& glurg::trace::CallSignature::get_name() const
{
	return this->name;
}

void glurg::trace::CallSignature::set_name(const std::string& value)
{
	this->name = value;
}

glurg::trace::CallSignature::ParameterCount
glurg::trace::CallSignature::get_num_parameters() const
{
	return (ParameterCount)this->parameters.size();
}

void glurg::trace::CallSignature::set_num_parameters(ParameterCount value)
{
	this->parameters.resize(value);
}

std::string glurg::trace::CallSignature::get_parameter_at(
	ParameterIndex index) const
{
	return this->parameters.at(index);
}

void glurg::trace::CallSignature::set_parameter_at(
	ParameterIndex index, const std::string& value)
{
	this->parameters.at(index) = value;
}

glurg::trace::CallSignature* glurg::trace::CallSignature::read(
	ID id, TraceFile& trace, FileStream& stream)
{
	CallSignature* signature = new CallSignature();

	signature->set_id(id);
	signature->set_name(trace.read_string(stream));

	signature->set_num_parameters(trace.read_unsigned_integer(stream));
	for (ParameterCount i = 0; i < signature->get_num_parameters(); ++i)
	{
		signature->set_parameter_at(i, trace.read_string(stream));
	}

	return signature;
}
