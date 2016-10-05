// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_STRUCTURE_HPP
#define LIBGLURG_TRACE_STRUCTURE_HPP

#include <cstddef>
#include <string>
#include <memory>
#include <unordered_map>

namespace glurg
{
	class StructureSignature;
	class Value;

	class Structure
	{
	public:
		explicit Structure(const StructureSignature* signature);
		~Structure() = default;

		const StructureSignature* get_signature() const;

		const Value* get_field_by_name(const std::string& name) const;
		const Value* get_field_by_index(std::size_t index) const;
		void set_field_by_name(const std::string& name, const Value* value);
		void set_field_by_index(std::size_t index, const Value* value);

	private:
		const StructureSignature* signature;

		typedef std::unique_ptr<Value> ValuePointer;
		std::unordered_map<std::string, ValuePointer> fields;
	};
}

#endif
