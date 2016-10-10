// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_BITMASK_SIGNATURE_HPP
#define LIBGLURG_TRACE_BITMASK_SIGNATURE_HPP

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

	class BitmaskSignature
	{
	public:
		typedef std::uint32_t ID;

		BitmaskSignature() = default;
		~BitmaskSignature() = default;

		ID get_id() const;

		bool match(std::uint32_t value, const std::string& mask) const;

		static BitmaskSignature* read(
			ID id, TraceFile& trace, FileStream& stream);

	private:
		void set_id(ID value);
		void set_mask_by_name(const std::string& name, std::uint32_t mask);

		ID id;
		std::unordered_map<std::string, std::uint32_t> values;
	};
} }

#endif
