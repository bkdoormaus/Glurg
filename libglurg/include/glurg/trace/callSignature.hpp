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
	class CallSignature
	{
	public:
		typedef std::uint32_t ID;
		typedef std::uint32_t ParameterIndex;
		typedef std::uint32_t ParameterCount;

		ID get_id() const;
		void set_id(ID value);

		const std::string& get_name() const;
		void set_name(const std::string& value);

		ParameterCount get_num_parameters() const;
		void set_num_parameters(ParameterCount value);

		std::string get_parameter_at(ParameterIndex index) const;
		void set_parameter_at(ParameterIndex index, const std::string& value);

	private:
		ID id;
		std::string name;
		std::vector<std::string> parameters;
	};
}

#endif
