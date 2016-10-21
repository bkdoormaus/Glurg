// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_ENUMERATION_SIGNATURE_HPP
#define LIBGLURG_TRACE_ENUMERATION_SIGNATURE_HPP

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>

namespace glurg
{
	class FileStream;
}

namespace glurg { namespace trace
{
	class Value;
	class TraceFile;

	class EnumerationSignature
	{
	public:
		typedef std::uint32_t ID;

		EnumerationSignature() = default;
		~EnumerationSignature() = default;

		ID get_id() const;

		const Value* get_value_by_name(const std::string& name) const;
		std::string get_name_by_value(const Value* value) const;

		static EnumerationSignature* read(
			ID id, TraceFile& trace, FileStream& stream);

	private:
		void set_id(ID value);
		void set_value_by_name(
			const std::string& name, std::shared_ptr<Value> value);

		ID id;

		typedef std::shared_ptr<Value> ValuePointer;
		std::unordered_map<std::string, ValuePointer> values;
	};
} }

#endif
