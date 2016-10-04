// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_TRACE_FILE_HPP
#define LIBGLURG_TRACE_TRACE_FILE_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "glurg/trace/bitmaskSignature.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/enumerationSignature.hpp"
#include "glurg/trace/values.hpp"

namespace glurg
{
	class FileStream;

	class TraceFile
	{
	public:
		TraceFile();
		~TraceFile() = default;

		const BitmaskSignature* get_bitmask_signature(
			BitmaskSignature::ID id) const;
		bool has_bitmask_signature(BitmaskSignature::ID id) const;
		void register_bitmask_signature(BitmaskSignature* signature);

		const CallSignature* get_call_signature(
			CallSignature::ID id) const;
		bool has_call_signature(CallSignature::ID id) const;
		void register_call_signature(CallSignature* signature);

		const EnumerationSignature* get_enumeration_signature(
			EnumerationSignature::ID id) const;
		bool has_enumeration_signature(EnumerationSignature::ID id) const;
		void register_enumeration_signature(
			EnumerationSignature* signature);

		void register_backtrace(std::uint32_t id);
		bool has_backtrace(std::uint32_t id) const;

		Value* read_value(FileStream& stream);
		std::uint32_t read_unsigned_integer(FileStream& stream);
		std::string read_string(FileStream& stream);

	private:
		void register_all_value_read_functions();
		void register_value_read_function(
			Value::Type type, Value::ReadFunction func);

		typedef std::unique_ptr<BitmaskSignature> BitmaskPointer;
		std::unordered_map<BitmaskSignature::ID, BitmaskPointer> bitmasks;

		typedef std::unique_ptr<CallSignature> CallPointer;
		std::unordered_map<CallSignature::ID, CallPointer> calls;

		typedef std::unique_ptr<EnumerationSignature> EnumerationPointer;
		std::unordered_map<EnumerationSignature::ID, EnumerationPointer> enumerations;

		std::vector<std::uint32_t> backtraces;

		std::unordered_map<Value::Type, Value::ReadFunction> read_value_functions;
	};
}

#endif
