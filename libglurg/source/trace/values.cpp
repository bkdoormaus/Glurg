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

glurg::IntegerValue::IntegerValue(std::int64_t value)
{
	this->value = value;
}

glurg::Value::Type glurg::IntegerValue::get_type() const
{
	if (value < 0)
	{
		return NEGATIVE_NUMBER;
	}
	else
	{
		return POSITIVE_NUMBER;
	}
}

glurg::Value* glurg::IntegerValue::clone() const
{
	return new IntegerValue(this->value);
}

std::int32_t glurg::IntegerValue::to_signed_integer() const
{
	return (std::int32_t)this->value;
}

std::uint32_t glurg::IntegerValue::to_unsigned_integer() const
{
	return (std::uint32_t)this->value;
}

glurg::Value* glurg::IntegerValue::read_positive_integer(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	return new IntegerValue(trace.read_unsigned_integer(stream));
}

glurg::Value* glurg::IntegerValue::read_negative_integer(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	return new IntegerValue(-((std::int64_t)trace.read_unsigned_integer(stream)));
}

glurg::FloatingPointValue::FloatingPointValue(Type type, double value)
{
	this->type = type;
	this->value = value;
}

glurg::Value::Type glurg::FloatingPointValue::get_type() const
{
	return this->type;
}

glurg::Value* glurg::FloatingPointValue::clone() const
{
	return new FloatingPointValue(this->type, this->value);
}

float glurg::FloatingPointValue::to_single() const
{
	return this->value;
}

double glurg::FloatingPointValue::to_double() const
{
	return this->value;
}

glurg::Value* glurg::FloatingPointValue::read_single(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	float v;
	stream.read((std::uint8_t*)&v, sizeof(float));

	return new FloatingPointValue(FLOAT, v);
}

glurg::Value* glurg::FloatingPointValue::read_double(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	double v;
	stream.read((std::uint8_t*) &v, sizeof(double));

	return new FloatingPointValue(DOUBLE, v);
}

glurg::StringValue::StringValue(const std::string& value)
{
	this->value = value;
}

glurg::Value::Type glurg::StringValue::get_type() const
{
	return STRING;
}

glurg::Value* glurg::StringValue::clone() const
{
	return new StringValue(this->value);
}

std::string glurg::StringValue::to_string() const
{
	return this->value;
}

const std::uint8_t* glurg::StringValue::to_pointer() const
{
	return (std::uint8_t*)this->value.c_str();
}

glurg::Value* glurg::StringValue::read_string(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	return new StringValue(trace.read_string(stream));
}
