// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_EVENTS_HPP
#define LIBGLURG_TRACE_EVENTS_HPP

#include <cstdint>
#include <vector>

#include "glurg/trace/call.hpp"

namespace glurg
{
	class CallSignature;
	class FileStream;
	class TraceFile;
	class Value;

	class Event
	{
	public:
		typedef std::uint8_t Type;

		enum
		{
			event_enter = 0,
			event_leave = 1
		};

		~Event() = default;

		Type get_type() const;
		Call::Index get_call_index() const;

		static Event* read(TraceFile& trace, FileStream& stream);

	private:
		enum
		{
			call_detail_terminator = 0,
			call_detail_argument = 1,
			call_detail_return = 2,
			call_detail_thread = 3,
			call_detail_backtrace = 4
		};

		Event() = default;

		static void skip_backtrace(TraceFile& trace, FileStream& stream);
		static void parse_call_detail(Call* call, TraceFile& trace, FileStream& stream);

		Type type;

		Call::Index call_index;
	};
}

#endif
