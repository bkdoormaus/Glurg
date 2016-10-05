// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_STRUCTURE_SIGNATURE_HPP
#define LIBGLURG_TRACE_STRUCTURE_SIGNATURE_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace glurg
{
	class FileStream;
	class TraceFile;

	class StructureSignature
	{
	public:
		typedef std::uint32_t ID;

		StructureSignature() = default;
		~StructureSignature() = default;

		ID get_id() const;

		std::size_t get_num_fields() const;
		std::string get_field_name(std::size_t index) const;
		bool has_field_name(const std::string& name) const;

		static StructureSignature* read(
			ID id, TraceFile& trace, FileStream& stream);

	private:
		void set_num_fields(std::size_t value);
		void set_field_name(std::size_t index, const std::string& value);

		ID id;

		std::vector<std::string> fields;
	};
}

#endif
