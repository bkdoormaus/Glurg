// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_EVENTS_HPP
#define LIBGLURG_TRACE_EVENTS_HPP

#include <cstdint>

namespace glurg
{
	class CallSignature;
	class Value;

	class Event
	{
	public:
		typedef std::uint8_t Type;

		virtual Type get_type() const = 0;
	};

	class EnterCallEvent
	{
	public:
		typedef std::uint32_t CallIndex;
		typedef std::uint32_t ArgumentCount;
		typedef std::uint32_t ArgumentIndex;
		typedef std::uint32_t Thread;

		Thread get_thread() const;
		const CallSignature* get_call_signature() const;

		ArgumentCount get_num_arguments() const;
		const Value* get_argument_at(ArgumentIndex index);

		Event::Type get_type() const;
	};

	class LeaveCallEvent
	{
	public:
		typedef std::uint32_t CallIndex;

		const Value* get_return_value() const;
		
		Event::Type get_type() const;
	};
}

#endif
