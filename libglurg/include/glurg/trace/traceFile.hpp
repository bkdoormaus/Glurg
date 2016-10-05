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
#include "glurg/trace/call.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/enumerationSignature.hpp"
#include "glurg/trace/structureSignature.hpp"
#include "glurg/trace/values.hpp"

namespace glurg
{
	class FileStream;

	class TraceFile
	{
	public:
		static const std::uint32_t VERSION = 5;
		
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

		const StructureSignature* get_structure_signature(
			StructureSignature::ID id) const;
		bool has_structure_signature(StructureSignature::ID id) const;
		void register_structure_signature(
			StructureSignature* signature);

		void register_backtrace(std::uint32_t id);
		bool has_backtrace(std::uint32_t id) const;

		Call* create_call(CallSignature::ID id);
		Call* get_call(Call::Index index);
		const Call* get_call(Call::Index index) const;

		Value* read_value(FileStream& stream);
		std::uint32_t read_unsigned_integer(FileStream& stream);
		std::string read_string(FileStream& stream);

		bool verify_is_compatible_version(FileStream& stream);

	private:
		void register_all_value_read_functions();
		void register_value_read_function(
			Value::Type type, Value::ReadFunction func);

		typedef std::unique_ptr<BitmaskSignature> BitmaskSignaturePointer;
		std::unordered_map<BitmaskSignature::ID, BitmaskSignaturePointer>
			bitmaskSignatures;

		typedef std::unique_ptr<CallSignature> CallSignaturePointer;
		std::unordered_map<CallSignature::ID, CallSignaturePointer>
			callSignatures;

		typedef std::unique_ptr<EnumerationSignature> EnumerationSignaturePointer;
		std::unordered_map<EnumerationSignature::ID, EnumerationSignaturePointer>
			enumerationSignatures;

		typedef std::unique_ptr<StructureSignature> StructureSignaturePointer;
		std::unordered_map<StructureSignature::ID, StructureSignaturePointer>
			structureSignatures;

		std::vector<std::uint32_t> backtraces;

		std::unordered_map<Value::Type, Value::ReadFunction> read_value_functions;

		typedef std::unique_ptr<Call> CallPointer;
		std::vector<CallPointer> calls;
	};
}

#endif
