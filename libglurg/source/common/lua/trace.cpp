// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <sol.hpp>
#include "glurg/common/lua.hpp"
#include "glurg/common/fileStream.hpp"
#include "glurg/trace/bitmaskSignature.hpp"
#include "glurg/trace/call.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/enumerationSignature.hpp"
#include "glurg/trace/events.hpp"
#include "glurg/trace/structure.hpp"
#include "glurg/trace/structureSignature.hpp"
#include "glurg/trace/traceFile.hpp"
#include "glurg/trace/values.hpp"

void glurg::lua::export_trace(lua_State* L)
{
	sol::table glurg = sol::stack::get<sol::table>(L, -1);

	sol::table trace = glurg.create_named("trace");
	trace["read_event"] = &glurg::trace::Event::read;
	trace["event_enter"] = glurg::trace::Event::event_enter;
	trace["event_leave"] = glurg::trace::Event::event_leave;
	trace.new_usertype<glurg::trace::BitmaskSignature>(
		"BitmaskSignature",
		"id", sol::readonly_property(&glurg::trace::BitmaskSignature::get_id),
		"match", &glurg::trace::BitmaskSignature::match);
	trace.new_usertype<glurg::trace::Call>(
		"Call",
		"index", sol::readonly_property(&glurg::trace::Call::get_call_index),
		"signature", sol::readonly_property(&glurg::trace::Call::get_call_signature),
		"thread", sol::readonly_property(&glurg::trace::Call::get_thread),
		"result", sol::readonly_property(&glurg::trace::Call::get_return_value),
		"get_argument_at", &glurg::trace::Call::get_argument_at,
		"set_argument_at", &glurg::trace::Call::set_argument_at);
	trace.new_usertype<glurg::trace::CallSignature>(
		"CallSignature",
		"id", sol::readonly_property(&glurg::trace::CallSignature::get_id),
		"name", sol::readonly_property(&glurg::trace::CallSignature::get_name),
		"num_parameters", sol::readonly_property(&glurg::trace::CallSignature::get_num_parameters),
		"get_parameter_at", &glurg::trace::CallSignature::get_parameter_at);
	trace.new_usertype<glurg::trace::EnumerationSignature>(
		"EnumerationSignature",
		"id", sol::readonly_property(&glurg::trace::EnumerationSignature::get_id),
		"get_value_by_name", &glurg::trace::EnumerationSignature::get_value_by_name,
		"has_value", &glurg::trace::EnumerationSignature::has_value,
		"get_name_by_value", &glurg::trace::EnumerationSignature::get_name_by_value);
	trace.new_usertype<glurg::trace::Event>(
		"Event",
		"type", sol::readonly_property(&glurg::trace::Event::get_type),
		"call_index", sol::readonly_property(&glurg::trace::Event::get_call_index));
	trace.new_usertype<glurg::trace::Structure>(
		"Structure",
		"signature", sol::readonly_property(&glurg::trace::Structure::get_signature),
		"get_field", &glurg::trace::Structure::get_field_by_name);
	trace.new_usertype<glurg::trace::StructureSignature>(
		"StructureSignature",
		"id", sol::readonly_property(&glurg::trace::StructureSignature::get_id),
		"name", sol::readonly_property(&glurg::trace::StructureSignature::get_name),
		"num_fields", sol::readonly_property(&glurg::trace::StructureSignature::get_num_fields),
		"get_field_name", &glurg::trace::StructureSignature::get_field_name,
		"has_field_name", &glurg::trace::StructureSignature::has_field_name);
	trace.new_usertype<glurg::trace::TraceFile>(
		"TraceFile",
		sol::constructors<sol::types<>>(),
		"get_call", (glurg::trace::Call* (glurg::trace::TraceFile::*)(glurg::trace::Call::Index))(&glurg::trace::TraceFile::get_call),
		"delete_call", &glurg::trace::TraceFile::delete_call,
		"verify_is_compatible_version", &glurg::trace::TraceFile::verify_is_compatible_version);
	trace.new_usertype<glurg::trace::Enumeration>(
		"Enumeration",
		"signature", sol::readonly(&glurg::trace::Enumeration::signature),
		"value", sol::readonly(&glurg::trace::Enumeration::value));
	trace.new_usertype<glurg::trace::Bitmask>(
		"Bitmask",
		"signature", sol::readonly(&glurg::trace::Bitmask::signature),
		"value", sol::readonly(&glurg::trace::Bitmask::value));
	trace.new_usertype<glurg::trace::Blob>(
		"Blob",
		"data", sol::readonly(&glurg::trace::Blob::data),
		"length", sol::readonly(&glurg::trace::Blob::length));
	trace.new_usertype<glurg::trace::Value>(
		"Value",
		"type", sol::readonly_property(&glurg::trace::Value::get_type),
		"to_boolean", &glurg::trace::Value::to_boolean,
		"to_integer", &glurg::trace::Value::to_signed_integer,
		"to_number", &glurg::trace::Value::to_double,
		"to_string", &glurg::trace::Value::to_string,
		"to_blob", &glurg::trace::Value::to_blob,
		"to_handle", &glurg::trace::Value::to_handle,
		"to_enumeration", &glurg::trace::Value::to_enumeration,
		"to_bitmask", &glurg::trace::Value::to_bitmask,
		"to_array", &glurg::trace::Value::to_array,
		"to_structure", &glurg::trace::Value::to_structure,
		"to_handle", &glurg::trace::Value::to_structure,
		"clone", &glurg::trace::Value::clone);
	trace.new_usertype<glurg::trace::Array>(
		"Array",
		"size", sol::readonly_property(&glurg::trace::Array::get_size),
		"get_value_at", &glurg::trace::Array::get_value_at);

	sol::table values = trace.create_named("values");
	values["false_boolean"] = glurg::trace::BoolValue::FALSE_BOOLEAN;
	values["true_boolean"] = glurg::trace::BoolValue::TRUE_BOOLEAN;
	values["negative_integer"] = glurg::trace::IntegerValue::NEGATIVE_NUMBER;
	values["positive_integer"] = glurg::trace::IntegerValue::POSITIVE_NUMBER;
	values["float"] = glurg::trace::FloatingPointValue::FLOAT;
	values["double"] = glurg::trace::FloatingPointValue::DOUBLE;
	values["string"] = glurg::trace::StringValue::STRING;
	values["null_pointer"] = glurg::trace::BlobValue::NULL_POINTER;
	values["blob"] = glurg::trace::BlobValue::BLOB;
	values["enumeration"] = glurg::trace::EnumerationValue::ENUMERATION;
	values["bitmask"] = glurg::trace::BitmaskValue::BITMASK;
	values["array"] = glurg::trace::ArrayValue::ARRAY;
	values["structure"] = glurg::trace::StructureValue::STRUCTURE;
	values["handle"] = glurg::trace::HandleValue::HANDLE;
}
