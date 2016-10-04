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
