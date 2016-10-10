// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <stdexcept>
#include "glurg/common/fileStream.hpp"
#include "glurg/trace/bitmaskSignature.hpp"
#include "glurg/trace/enumerationSignature.hpp"
#include "glurg/trace/structure.hpp"
#include "glurg/trace/structureSignature.hpp"
#include "glurg/trace/traceFile.hpp"
#include "glurg/trace/values.hpp"

bool glurg::trace::Value::to_boolean() const
{
	throw std::runtime_error("invalid conversion to bool");
}

std::int32_t glurg::trace::Value::to_signed_integer() const
{
	throw std::runtime_error("invalid conversion to signed integer");
}

std::uint32_t glurg::trace::Value::to_unsigned_integer() const
{
	throw std::runtime_error("invalid conversion to unsigned integer");
}

float glurg::trace::Value::to_single() const
{
	throw std::runtime_error("invalid conversion to single-precision float");
}

double glurg::trace::Value::to_double() const
{
	throw std::runtime_error("invalid conversion to double-precision float");
}

std::string glurg::trace::Value::to_string() const
{
	throw std::runtime_error("invalid conversion to string");
}

glurg::trace::Blob glurg::trace::Value::to_blob() const
{
	throw std::runtime_error("invalid conversion to blob");
}

std::uint32_t glurg::trace::Value::to_handle() const
{
	throw std::runtime_error("invalid conversion to handle");
}

glurg::trace::Enumeration glurg::trace::Value::to_enumeration() const
{
	throw std::runtime_error("invalid conversion to enumeration");
}

glurg::trace::Bitmask glurg::trace::Value::to_bitmask() const
{
	throw std::runtime_error("invalid conversion to bitmask");
}

const glurg::trace::Array* glurg::trace::Value::to_array() const
{
	throw std::runtime_error("invalid conversion to array");
}

const glurg::trace::Structure* glurg::trace::Value::to_structure() const
{
	throw std::runtime_error("invalid conversion to structure");
}

std::size_t glurg::trace::Array::get_size() const
{
	return this->values.size();
}

void glurg::trace::Array::set_size(std::size_t value)
{
	this->values.resize(value);
}

const glurg::trace::Value* glurg::trace::Array::get_value_at(
	std::size_t index) const
{
	return this->values.at(index).get();
}

void glurg::trace::Array::set_value_at(std::size_t index, const Value* value)
{
	this->values.at(index) = value->clone();
}

glurg::trace::BoolValue::BoolValue(bool value)
{
	this->value = value;
}

glurg::trace::Value::Type glurg::trace::BoolValue::get_type() const
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

std::shared_ptr<glurg::trace::Value> glurg::trace::BoolValue::clone() const
{
	return std::shared_ptr<Value>(new BoolValue(this->value));
}

bool glurg::trace::BoolValue::to_boolean() const
{
	return this->value;
}

glurg::trace::Value* glurg::trace::BoolValue::read_false_boolean(
	Type type, TraceFile& trace, FileStream& stream)
{
	return new BoolValue(false);
}

glurg::trace::Value* glurg::trace::BoolValue::read_true_boolean(
	Type type, TraceFile& trace, FileStream& stream)
{
	return new BoolValue(true);
}

glurg::trace::IntegerValue::IntegerValue(std::int64_t value)
{
	this->value = value;
}

glurg::trace::Value::Type glurg::trace::IntegerValue::get_type() const
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

std::shared_ptr<glurg::trace::Value> glurg::trace::IntegerValue::clone() const
{
	return std::shared_ptr<glurg::trace::Value>(new IntegerValue(this->value));
}

std::int32_t glurg::trace::IntegerValue::to_signed_integer() const
{
	return (std::int32_t)this->value;
}

std::uint32_t glurg::trace::IntegerValue::to_unsigned_integer() const
{
	return (std::uint32_t)this->value;
}

glurg::trace::Value* glurg::trace::IntegerValue::read_positive_integer(
	Type type, TraceFile& trace, FileStream& stream)
{
	return new IntegerValue(trace.read_unsigned_integer(stream));
}

glurg::trace::Value* glurg::trace::IntegerValue::read_negative_integer(
	Type type, TraceFile& trace, FileStream& stream)
{
	return new IntegerValue(
		-((std::int64_t)trace.read_unsigned_integer(stream)));
}

glurg::trace::FloatingPointValue::FloatingPointValue(Type type, double value)
{
	this->type = type;
	this->value = value;
}

glurg::trace::Value::Type glurg::trace::FloatingPointValue::get_type() const
{
	return this->type;
}

std::shared_ptr<glurg::trace::Value>
glurg::trace::FloatingPointValue::clone() const
{
	return std::shared_ptr<Value>(
		new FloatingPointValue(this->type, this->value));
}

float glurg::trace::FloatingPointValue::to_single() const
{
	return this->value;
}

double glurg::trace::FloatingPointValue::to_double() const
{
	return this->value;
}

glurg::trace::Value* glurg::trace::FloatingPointValue::read_single(
	Type type, TraceFile& trace, FileStream& stream)
{
	float v;
	stream.read((std::uint8_t*)&v, sizeof(float));

	return new FloatingPointValue(FLOAT, v);
}

glurg::trace::Value* glurg::trace::FloatingPointValue::read_double(
	Type type, TraceFile& trace, FileStream& stream)
{
	double v;
	stream.read((std::uint8_t*) &v, sizeof(double));

	return new FloatingPointValue(DOUBLE, v);
}

glurg::trace::StringValue::StringValue(const std::string& value)
{
	this->value = value;
}

glurg::trace::Value::Type glurg::trace::StringValue::get_type() const
{
	return STRING;
}

std::shared_ptr<glurg::trace::Value> glurg::trace::StringValue::clone() const
{
	return std::shared_ptr<glurg::trace::Value>(new StringValue(this->value));
}

std::string glurg::trace::StringValue::to_string() const
{
	return this->value;
}

glurg::trace::Blob glurg::trace::StringValue::to_blob() const
{
	return { (std::uint8_t*)this->value.c_str(), this->value.size() };
}

glurg::trace::Value* glurg::trace::StringValue::read_string(
	Type type, TraceFile& trace, FileStream& stream)
{
	return new StringValue(trace.read_string(stream));
}

glurg::trace::BlobValue::BlobValue(std::size_t length)
{
	this->data = new std::uint8_t[length];
	this->length = length;
}

glurg::trace::BlobValue::BlobValue(std::size_t length, const std::uint8_t* data)
{
	this->data = new std::uint8_t[length];
	std::memcpy(this->data, data, length);

	this->length = length;
}

glurg::trace::BlobValue::~BlobValue()
{
	delete this->data;
}

glurg::trace::Value::Type glurg::trace::BlobValue::get_type() const
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

std::shared_ptr<glurg::trace::Value> glurg::trace::BlobValue::clone() const
{
	return std::shared_ptr<Value>(new BlobValue(this->length, this->data));
}

glurg::trace::Blob glurg::trace::BlobValue::to_blob() const
{
	return { this->data, this->length };
}

glurg::trace::Value* glurg::trace::BlobValue::read_blob(
	Type type, TraceFile& trace, FileStream& stream)
{
	std::size_t length = trace.read_unsigned_integer(stream);

	BlobValue* value = new BlobValue(length);
	stream.read(value->data, length);

	return value;
}

glurg::trace::Value* glurg::trace::BlobValue::read_null_pointer(
	Type type, TraceFile& trace, FileStream& stream)
{
	BlobValue* value = new BlobValue();
	value->data = nullptr;
	value->length = 0;

	return value;
}

glurg::trace::EnumerationValue::EnumerationValue(
	const EnumerationSignature* signature, const std::shared_ptr<Value>& value)
{
	this->value.signature = signature;
	this->value.value = value;
}

glurg::trace::Value::Type glurg::trace::EnumerationValue::get_type() const
{
	return ENUMERATION;
}

std::shared_ptr<glurg::trace::Value>
glurg::trace::EnumerationValue::clone() const
{
	return std::shared_ptr<Value>(new EnumerationValue(
		this->value.signature, this->value.value->clone()));
}

glurg::trace::Enumeration glurg::trace::EnumerationValue::to_enumeration() const
{
	return this->value;
}

glurg::trace::Value* glurg::trace::EnumerationValue::read_enumeration(
	Type type, TraceFile& trace, FileStream& stream)
{
	EnumerationSignature::ID id = trace.read_unsigned_integer(stream);
	auto& registry = trace.get_enumeration_signature_registry();
	if (!registry.has_signature(id))
	{
		EnumerationSignature* s = EnumerationSignature::read(id, trace, stream);
		registry.register_signature(id, s);
	}

	return new EnumerationValue(
		registry.get_signature(id), trace.read_value(stream)->clone());
}

glurg::trace::BitmaskValue::BitmaskValue(
	const BitmaskSignature* signature, std::uint32_t value)
{
	this->value.signature = signature;
	this->value.value = value;
}

glurg::trace::Value::Type glurg::trace::BitmaskValue::get_type() const
{
	return BITMASK;
}

std::shared_ptr<glurg::trace::Value> glurg::trace::BitmaskValue::clone() const
{
	return std::shared_ptr<Value>(
		new BitmaskValue(this->value.signature, this->value.value));
}

glurg::trace::Bitmask glurg::trace::BitmaskValue::to_bitmask() const
{
	return this->value;
}

glurg::trace::Value* glurg::trace::BitmaskValue::read_bitmask(
	Type type, TraceFile& trace, FileStream& stream)
{
	BitmaskSignature::ID id = trace.read_unsigned_integer(stream);
	auto& registry = trace.get_bitmask_signature_registry();
	if (!registry.get_signature(id))
	{
		BitmaskSignature* s = BitmaskSignature::read(id, trace, stream);
		registry.register_signature(id, s);
	}

	std::uint32_t v = trace.read_unsigned_integer(stream);
	return new BitmaskValue(registry.get_signature(id), v);
}

glurg::trace::ArrayValue::ArrayValue(const Array* array)
{
	this->value.set_size(array->get_size());
	for (std::size_t i = 0; i < array->get_size(); ++i)
	{
		this->value.set_value_at(i, array->get_value_at(i));
	}
}

glurg::trace::Value::Type glurg::trace::ArrayValue::get_type() const
{
	return ARRAY;
}

std::shared_ptr<glurg::trace::Value> glurg::trace::ArrayValue::clone() const
{
	return std::shared_ptr<Value>(new ArrayValue(&this->value));
}

const glurg::trace::Array* glurg::trace::ArrayValue::to_array() const
{
	return &this->value;
}

glurg::trace::Value* glurg::trace::ArrayValue::read_array(
	Type type, TraceFile& trace, FileStream& stream)
{
	ArrayValue* v = new ArrayValue();

	std::size_t count = trace.read_unsigned_integer(stream);
	v->value.set_size(count);

	for (std::size_t i = 0; i < count; ++i)
	{
		v->value.set_value_at(i, trace.read_value(stream).get());
	}

	return v;
}

glurg::trace::StructureValue::StructureValue(
	const StructureSignature* signature)
{
	this->value = StructurePointer(new Structure(signature));
}

glurg::trace::Value::Type glurg::trace::StructureValue::get_type() const
{
	return STRUCTURE;
}

std::shared_ptr<glurg::trace::Value> glurg::trace::StructureValue::clone() const
{
	auto signature = this->value->get_signature();
	StructureValue* other = new StructureValue(signature);

	for (std::size_t i = 0; i < signature->get_num_fields(); ++i)
	{
		auto field = this->value->get_field_by_index(i);
		if (field != nullptr)
		{
			other->value->set_field_by_index(i, field);
		}
	}

	return std::shared_ptr<Value>(other);
}

const glurg::trace::Structure*
glurg::trace::StructureValue::to_structure() const
{
	return this->value.get();
}

glurg::trace::Value* glurg::trace::StructureValue::read_structure(
	Type type, TraceFile& trace, FileStream& stream)
{
	StructureSignature::ID id = trace.read_unsigned_integer(stream);
	auto& registry = trace.get_structure_signature_registry();
	if (!registry.get_signature(id))
	{
		StructureSignature* s = StructureSignature::read(id, trace, stream);
		registry.register_signature(id, s);
	}

	auto signature = registry.get_signature(id);
	StructureValue* v = new StructureValue(signature);
	for (std::size_t i = 0; i < signature->get_num_fields(); ++i)
	{
		v->value->set_field_by_index(i, trace.read_value(stream).get());
	}

	return v;
}

glurg::trace::HandleValue::HandleValue(std::uint32_t value)
{
	this->value = value;
}

glurg::trace::Value::Type glurg::trace::HandleValue::get_type() const
{
	return HANDLE;
}

std::shared_ptr<glurg::trace::Value> glurg::trace::HandleValue::clone() const
{
	return std::shared_ptr<glurg::trace::Value>(new HandleValue(this->value));
}

std::uint32_t glurg::trace::HandleValue::to_handle() const
{
	return this->value;
}

glurg::trace::Value* glurg::trace::HandleValue::read_handle(
	Type type, TraceFile& trace, FileStream& stream)
{
	return new HandleValue(trace.read_unsigned_integer(stream));
}
