// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_EVENTS_HPP
#define LIBGLURG_TRACE_EVENTS_HPP

#include <cstdint>
#include <vector>

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

		virtual Type get_type() const = 0;

	protected:
		static void skip_backtrace(TraceFile& trace, FileStream& stream);

		enum
		{
			call_detail_terminator = 0,
			call_detail_argument = 1,
			call_detail_return = 2,
			call_detail_thread = 3,
			call_detail_backtrace = 4
		};
	};

	class EnterCallEvent : public Event
	{
	public:
		typedef std::uint32_t CallIndex;
		typedef std::uint32_t ArgumentCount;
		typedef std::uint32_t ArgumentIndex;
		typedef std::uint32_t Thread;

		~EnterCallEvent() = default;

		Thread get_thread() const;
		const CallSignature* get_call_signature() const;

		ArgumentCount get_num_arguments() const;
		const Value* get_argument_at(ArgumentIndex index);

		static EnterCallEvent* read(
			Type type, TraceFile& trace, FileStream& stream);

		Event::Type get_type() const;

	private:
		EnterCallEvent() = default;

		const CallSignature* signature;

		Thread thread;

		typedef std::unique_ptr<Value> ValuePointer;
		std::vector<ValuePointer> arguments;
	};

	class LeaveCallEvent : public Event
	{
	public:
		typedef std::uint32_t CallIndex;

		~LeaveCallEvent();

		CallIndex get_call_index() const;

		const Value* get_return_value() const;

		static LeaveCallEvent* read(
			Type type, TraceFile& trace, FileStream& stream);
		
		Event::Type get_type() const;

	private:
		Value* return_value;
		CallIndex call_index;
	};
}

#endif
