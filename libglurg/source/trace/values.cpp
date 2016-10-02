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

glurg::Blob glurg::Value::to_blob() const
{
	throw std::runtime_error("invalid conversion to blob");
}

std::uint32_t glurg::Value::to_handle() const
{
	throw std::runtime_error("invalid conversion to handle");
}

glurg::Enumeration glurg::Value::to_enumeration() const
{
	throw std::runtime_error("invalid conversion to enumeration");
}

glurg::Bitmask glurg::Value::to_bitmask() const
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

glurg::Blob glurg::StringValue::to_blob() const
{
	return { (std::uint8_t*)this->value.c_str(), this->value.size() };
}

glurg::Value* glurg::StringValue::read_string(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	return new StringValue(trace.read_string(stream));
}

glurg::BlobValue::BlobValue(std::size_t length)
{
	this->data = new std::uint8_t[length];
	this->length = length;
}

glurg::BlobValue::BlobValue(std::size_t length, const std::uint8_t* data)
{
	this->data = new std::uint8_t[length];
	std::memcpy(this->data, data, length);

	this->length = length;
}

glurg::BlobValue::~BlobValue()
{
	delete this->data;
}

glurg::Value::Type glurg::BlobValue::get_type() const
{
	if (this->data == nullptr)
	{
		return NULL_POINTER;
	}
	else
	{
		return BLOB;
	}
}

glurg::Value* glurg::BlobValue::clone() const
{
	return new BlobValue(this->length, this->data);
}

glurg::Blob glurg::BlobValue::to_blob() const
{
	return { this->data, this->length };
}

glurg::Value* glurg::BlobValue::read_blob(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	std::size_t length = trace.read_unsigned_integer(stream);

	BlobValue* value = new BlobValue(length);
	stream.read(value->data, length);

	return value;
}

glurg::Value* glurg::BlobValue::read_null_pointer(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	BlobValue* value = new BlobValue();
	value->data = nullptr;
	value->length = 0;

	return value;
}

glurg::EnumerationValue::EnumerationValue(
	const EnumerationSignature* signature, Value* value)
{
	this->value.signature = signature;
	this->value.value = value;
}

glurg::EnumerationValue::~EnumerationValue()
{
	delete this->value.value;
}

glurg::Value::Type glurg::EnumerationValue::get_type() const
{
	return ENUMERATION;
}

glurg::Value* glurg::EnumerationValue::clone() const
{
	return new EnumerationValue(
		this->value.signature, this->value.value->clone());
}

glurg::Enumeration glurg::EnumerationValue::to_enumeration() const
{
	return this->value;
}

glurg::Value* glurg::EnumerationValue::read_enumeration(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	EnumerationSignature::ID id = trace.read_unsigned_integer(stream);
	if (!trace.has_enumeration_signature(id))
	{
		EnumerationSignature* s = EnumerationSignature::read(id, trace, stream);
		trace.register_enumeration_signature(s);
	}

	return new EnumerationValue(
		trace.get_enumeration_signature(id), trace.read_value(stream));
}

glurg::BitmaskValue::BitmaskValue(
	const BitmaskSignature* signature, std::uint32_t value)
{
	this->value.signature = signature;
	this->value.value = value;
}

glurg::Value::Type glurg::BitmaskValue::get_type() const
{
	return BITMASK;
}

glurg::Value* glurg::BitmaskValue::clone() const
{
	return new BitmaskValue(this->value.signature, this->value.value);
}

glurg::Bitmask glurg::BitmaskValue::to_bitmask() const
{
	return this->value;
}

glurg::Value* glurg::BitmaskValue::read_bitmask(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	BitmaskSignature::ID id = trace.read_unsigned_integer(stream);
	if (!trace.has_bitmask_signature(id))
	{
		BitmaskSignature* s = BitmaskSignature::read(id, trace, stream);
		trace.register_bitmask_signature(s);
	}

	Value* v = trace.read_value(stream);
	return new BitmaskValue(
		trace.get_bitmask_signature(id), v->to_unsigned_integer());
}

glurg::ArrayValue::ArrayValue(const Array* array)
{
	this->value.set_size(array->get_size());
	for (std::size_t i = 0; i < array->get_size(); ++i)
	{
		this->value.set_value_at(i, array->get_value_at(i)->clone());
	}
}

glurg::Value::Type glurg::ArrayValue::get_type() const
{
	return ARRAY;
}

glurg::Value* glurg::ArrayValue::clone() const
{
	return new ArrayValue(&this->value);
}

const glurg::Array* glurg::ArrayValue::to_array() const
{
	return &this->value;
}

glurg::Value* glurg::ArrayValue::read_array(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	ArrayValue* v = new ArrayValue();

	std::size_t count = trace.read_unsigned_integer(stream);
	v->value.set_size(count);

	for (std::size_t i = 0; i < count; ++i)
	{
		v->value.set_value_at(i, trace.read_value(stream));
	}

	return v;
}

glurg::HandleValue::HandleValue(std::uint32_t value)
{
	this->value = value;
}

glurg::Value::Type glurg::HandleValue::get_type() const
{
	return HANDLE;
}

glurg::Value* glurg::HandleValue::clone() const
{
	return new HandleValue(this->value);
}

std::uint32_t glurg::HandleValue::to_handle() const
{
	return this->value;
}

glurg::Value* glurg::HandleValue::read_handle(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	return new HandleValue(trace.read_unsigned_integer(stream));
}
