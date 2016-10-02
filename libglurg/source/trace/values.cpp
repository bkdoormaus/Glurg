// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <stdexcept>
#include "glurg/common/fileStream.hpp"
#include "glurg/trace/bitmaskSignature.hpp"
#include "glurg/trace/enumerationSignature.hpp"
#include "glurg/trace/traceFile.hpp"
#include "glurg/trace/values.hpp"

bool glurg::Value::to_boolean() const
{
	throw std::runtime_error("invalid conversion to bool");
}

std::int32_t glurg::Value::to_signed_integer() const
{
	throw std::runtime_error("invalid conversion to signed integer");
}

std::uint32_t glurg::Value::to_unsigned_integer() const
{
	throw std::runtime_error("invalid conversion to unsigned integer");
}

float glurg::Value::to_single() const
{
	throw std::runtime_error("invalid conversion to single-precision float");
}

double glurg::Value::to_double() const
{
	throw std::runtime_error("invalid conversion to double-precision float");
}

std::string glurg::Value::to_string() const
{
	throw std::runtime_error("invalid conversion to string");
}

const std::uint8_t* glurg::Value::to_pointer() const
{
	throw std::runtime_error("invalid conversion to raw pointer");
}

std::uint32_t glurg::Value::to_handle() const
{
	throw std::runtime_error("invalid conversion to handle");
}

const glurg::Enumeration* glurg::Value::to_enumeration() const
{
	throw std::runtime_error("invalid conversion to enumeration");
}

const glurg::Bitmask* glurg::Value::to_bitmask() const
{
	throw std::runtime_error("invalid conversion to bitmask");
}

const glurg::Array* glurg::Value::to_array() const
{
	throw std::runtime_error("invalid conversion to array");
}

std::size_t glurg::Array::get_size() const
{
	return this->values.size();
}

void glurg::Array::set_size(std::size_t value)
{
	this->values.resize(value);
}

const glurg::Value* glurg::Array::get_value_at(std::size_t index) const
{
	return this->values.at(index).get();
}

void glurg::Array::set_value_at(std::size_t index, const glurg::Value* value)
{
	this->values.at(index) = ValuePointer(value->clone());
}

glurg::BoolValue::BoolValue(bool value)
{
	this->value = value;
}

glurg::Value::Type glurg::BoolValue::get_type() const
{
	if (value == false)
	{
		return FALSE_BOOLEAN;
	}
	else
	{
		return TRUE_BOOLEAN;
	}
}

glurg::Value* glurg::BoolValue::clone() const
{
	return new BoolValue(this->value);
}

bool glurg::BoolValue::to_boolean() const
{
	return this->value;
}

glurg::Value* glurg::BoolValue::read_false_boolean(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	return new BoolValue(false);
}

glurg::Value* glurg::BoolValue::read_true_boolean(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	return new BoolValue(true);
}
