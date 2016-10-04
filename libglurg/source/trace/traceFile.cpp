// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <stdexcept>
#include "glurg/common/fileStream.hpp"
#include "glurg/trace/traceFile.hpp"

glurg::TraceFile::TraceFile()
{
	register_all_value_read_functions();
}

const glurg::BitmaskSignature* glurg::TraceFile::get_bitmask_signature(
	BitmaskSignature::ID id) const
{
	auto e = this->bitmasks.find(id);
	if (e == this->bitmasks.end())
	{
		return nullptr;
	}

	return e->second.get();
}

bool glurg::TraceFile::has_bitmask_signature(BitmaskSignature::ID id) const
{
	return this->bitmasks.find(id) != this->bitmasks.end();
}

void glurg::TraceFile::register_bitmask_signature(
	glurg::BitmaskSignature* signature)
{
	this->bitmasks.insert(
		std::make_pair(signature->get_id(), BitmaskPointer(signature)));
}

const glurg::CallSignature* glurg::TraceFile::get_call_signature(
	CallSignature::ID id) const
{
	auto e = this->calls.find(id);
	if (e == this->calls.end())
	{
		return nullptr;
	}

	return e->second.get();
}

bool glurg::TraceFile::has_call_signature(CallSignature::ID id) const
{
	return this->calls.find(id) != this->calls.end();
}

void glurg::TraceFile::register_call_signature(
	glurg::CallSignature* signature)
{
	this->calls.insert(
		std::make_pair(signature->get_id(), CallPointer(signature)));
}

const glurg::EnumerationSignature* glurg::TraceFile::get_enumeration_signature(
	EnumerationSignature::ID id) const
{
	auto e = this->enumerations.find(id);
	if (e == this->enumerations.end())
	{
		return nullptr;
	}

	return e->second.get();
}

bool
glurg::TraceFile::has_enumeration_signature(EnumerationSignature::ID id) const
{
	return this->enumerations.find(id) != this->enumerations.end();
}

void glurg::TraceFile::register_backtrace(std::uint32_t id)
{
	if (!has_backtrace(id))
	{
		this->backtraces.push_back(id);
	}
}

bool glurg::TraceFile::has_backtrace(std::uint32_t id) const
{
	auto begin = this->backtraces.begin();
	auto end = this->backtraces.end();

	return std::find(begin, end, id) != end;
}

void glurg::TraceFile::register_enumeration_signature(
	glurg::EnumerationSignature* signature)
{
	this->enumerations.insert(
		std::make_pair(signature->get_id(), EnumerationPointer(signature)));
}

glurg::Value* glurg::TraceFile::read_value(glurg::FileStream& stream)
{
	std::uint8_t type = 0;
	stream.read(&type, sizeof(std::uint8_t));

	auto e = this->read_value_functions.find(type);
	if (e == this->read_value_functions.end())
	{
		throw std::runtime_error("unknown value type");
	}

	auto read_func = e->second;
	return read_func(type, *this, stream);
}

std::uint32_t glurg::TraceFile::read_unsigned_integer(glurg::FileStream& stream)
{
	std::uint32_t current_value = 0;
	std::uint8_t current_byte = 0;
	do
	{
		current_value <<= 7;

		stream.read(&current_byte, sizeof(std::uint8_t));
		current_value |= current_byte & 0x7F;
	} while(current_byte & 0x80);

	return current_value;
}

std::string glurg::TraceFile::read_string(glurg::FileStream& stream)
{
	std::string result;

	std::uint32_t length = read_unsigned_integer(stream);
	result.reserve(length);

	for (std::uint32_t i = 0; i < length; ++i)
	{
		std::uint8_t c;
		stream.read(&c, sizeof(std::uint8_t));

		result.push_back(c);
	}

	return result;
}

bool glurg::TraceFile::verify_is_compatible_version(glurg::FileStream& stream)
{
	std::uint32_t version = read_unsigned_integer(stream);

	return version == VERSION;
}

void glurg::TraceFile::register_all_value_read_functions()
{
	register_value_read_function(
		BoolValue::FALSE_BOOLEAN, BoolValue::read_false_boolean);
	register_value_read_function(
		BoolValue::TRUE_BOOLEAN, BoolValue::read_true_boolean);

	register_value_read_function(
		IntegerValue::NEGATIVE_NUMBER, IntegerValue::read_negative_integer);
	register_value_read_function(
		IntegerValue::POSITIVE_NUMBER, IntegerValue::read_positive_integer);

	register_value_read_function(
		FloatingPointValue::FLOAT, FloatingPointValue::read_single);
	register_value_read_function(
		FloatingPointValue::DOUBLE, FloatingPointValue::read_double);

	register_value_read_function(
		StringValue::STRING, StringValue::read_string);

	register_value_read_function(
		BlobValue::NULL_POINTER, BlobValue::read_null_pointer);
	register_value_read_function(
		BlobValue::BLOB, BlobValue::read_blob);

	register_value_read_function(
		EnumerationValue::ENUMERATION, EnumerationValue::read_enumeration);

	register_value_read_function(
		BitmaskValue::BITMASK, BitmaskValue::read_bitmask);

	register_value_read_function(
		ArrayValue::ARRAY, ArrayValue::read_array);

	register_value_read_function(
		HandleValue::HANDLE, HandleValue::read_handle);
}

void glurg::TraceFile::register_value_read_function(
	Value::Type type, Value::ReadFunction func)
{
	this->read_value_functions.insert(std::make_pair(type, func));
}
