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
#include "glurg/trace/signatureRegistry.hpp"
#include "glurg/trace/structureSignature.hpp"
#include "glurg/trace/values.hpp"

namespace glurg
{
	class FileStream;
}

namespace glurg { namespace trace
{
	typedef SignatureRegistry<BitmaskSignature::ID, BitmaskSignature>
		BitmaskSignatureRegistry;
	typedef SignatureRegistry<CallSignature::ID, CallSignature>
		CallSignatureRegistry;
	typedef SignatureRegistry<EnumerationSignature::ID, EnumerationSignature>
		EnumerationSignatureRegistry;
	typedef SignatureRegistry<StructureSignature::ID, StructureSignature>
		StructureSignatureRegistry;

	class TraceFile
	{
	public:
		static const std::uint32_t VERSION = 5;
		
		TraceFile();
		~TraceFile() = default;

		BitmaskSignatureRegistry& get_bitmask_signature_registry();
		CallSignatureRegistry& get_call_signature_registry();
		EnumerationSignatureRegistry& get_enumeration_signature_registry();
		StructureSignatureRegistry& get_structure_signature_registry();

		void register_backtrace(std::uint32_t id);
		bool has_backtrace(std::uint32_t id) const;

		Call* create_call(CallSignature::ID id);
		void delete_call(Call* call);

		Call* get_call(Call::Index index);
		const Call* get_call(Call::Index index) const;

		std::shared_ptr<Value> read_value(FileStream& stream);
		std::uint32_t read_unsigned_integer(FileStream& stream);
		std::string read_string(FileStream& stream);

		bool verify_is_compatible_version(FileStream& stream);

	private:
		void register_all_value_read_functions();
		void register_value_read_function(
			Value::Type type, Value::ReadFunction func);

		BitmaskSignatureRegistry bitmaskSignatures;
		CallSignatureRegistry callSignatures;
		EnumerationSignatureRegistry enumerationSignatures;
		StructureSignatureRegistry structureSignatures;

		std::vector<std::uint32_t> backtraces;

		std::unordered_map<Value::Type, Value::ReadFunction> read_value_functions;

		typedef std::unique_ptr<Call> CallPointer;
		std::unordered_map<Call::Index, CallPointer> calls;
		Call::Index lifetime_num_calls;
	};
} }

#endif
