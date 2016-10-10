// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_CALL_SIGNATURE_HPP
#define LIBGLURG_TRACE_CALL_SIGNATURE_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace glurg
{
	class FileStream;
}

namespace glurg { namespace trace
{
	class TraceFile;

	class CallSignature
	{
	public:
		typedef std::uint32_t ID;
		typedef std::uint32_t ParameterIndex;
		typedef std::uint32_t ParameterCount;

		CallSignature() = default;
		~CallSignature() = default;

		ID get_id() const;
		const std::string& get_name() const;

		ParameterCount get_num_parameters() const;
		std::string get_parameter_at(ParameterIndex index) const;

		static CallSignature* read(ID id, TraceFile& trace, FileStream& stream);

	private:
		void set_id(ID value);
		void set_name(const std::string& value);
		void set_num_parameters(ParameterCount value);
		void set_parameter_at(ParameterIndex index, const std::string& value);

		ID id;
		std::string name;
		std::vector<std::string> parameters;
	};
} }

#endif
