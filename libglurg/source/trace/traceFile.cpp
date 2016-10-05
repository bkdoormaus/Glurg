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
	this->lifetime_num_calls = 0;

	register_all_value_read_functions();
}

glurg::BitmaskSignatureRegistry&
glurg::TraceFile::get_bitmask_signature_registry()
{
	return this->bitmaskSignatures;
}

glurg::CallSignatureRegistry&
glurg::TraceFile::get_call_signature_registry()
{
	return this->callSignatures;
}

glurg::EnumerationSignatureRegistry&
glurg::TraceFile::get_enumeration_signature_registry()
{
	return this->enumerationSignatures;
}

glurg::StructureSignatureRegistry&
glurg::TraceFile::get_structure_signature_registry()
{
	return this->structureSignatures;
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

glurg::Call* glurg::TraceFile::create_call(CallSignature::ID id)
{
	auto signature = get_call_signature_registry().get_signature(id);
	Call* call = new Call(this->lifetime_num_calls, signature);
	calls.emplace(this->lifetime_num_calls, CallPointer(call));

	++this->lifetime_num_calls;

	return call;
}

void glurg::TraceFile::delete_call(glurg::Call* call)
{
	this->calls.erase(call->get_call_index());
}

glurg::Call* glurg::TraceFile::get_call(Call::Index index)
{
	auto e = this->calls.find(index);
	if (e != this->calls.end())
	{
		return e->second.get();
	}

	return nullptr;
}

const glurg::Call* glurg::TraceFile::get_call(Call::Index index) const
{
	auto e = this->calls.find(index);
	if (e != this->calls.end())
	{
		return e->second.get();
	}

	return nullptr;
}

std::shared_ptr<glurg::Value> glurg::TraceFile::read_value(
	glurg::FileStream& stream)
{
	std::uint8_t type = 0;
	stream.read(&type, sizeof(std::uint8_t));

	auto e = this->read_value_functions.find(type);
	if (e == this->read_value_functions.end())
	{
		throw std::runtime_error("unknown value type");
	}

	auto read_func = e->second;
	return std::shared_ptr<Value>(read_func(type, *this, stream));
}

std::uint32_t glurg::TraceFile::read_unsigned_integer(glurg::FileStream& stream)
{
	std::uint32_t current_value = 0;
	std::uint8_t current_byte = 0;
	std::uint32_t shift = 0;
	do
	{
		stream.read(&current_byte, sizeof(std::uint8_t));
		current_value |= (std::uint32_t)(current_byte & 0x7F) << shift;
		shift += 7;
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
		StructureValue::STRUCTURE, StructureValue::read_structure);

	register_value_read_function(
		HandleValue::HANDLE, HandleValue::read_handle);
}

void glurg::TraceFile::register_value_read_function(
	Value::Type type, Value::ReadFunction func)
{
	this->read_value_functions.insert(std::make_pair(type, func));
}
