// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/trace/callSignature.hpp"

glurg::CallSignature::ID glurg::CallSignature::get_id() const
{
	return this->id;
}

void glurg::CallSignature::set_id(ID value)
{
	this->id = value;
}

const std::string& glurg::CallSignature::get_name() const
{
	return this->name;
}

void glurg::CallSignature::set_name(const std::string& value)
{
	this->name = value;
}

glurg::CallSignature::ParameterCount
glurg::CallSignature::get_num_parameters() const
{
	return (ParameterCount)this->parameters.size();
}

void glurg::CallSignature::set_num_parameters(ParameterCount value)
{
	this->parameters.resize(value);
}

std::string glurg::CallSignature::get_parameter_at(ParameterIndex index) const
{
	return this->parameters.at(index);
}

void glurg::CallSignature::set_parameter_at(
	ParameterIndex index, const std::string& value)
{
	this->parameters.at(index) = value;
}
