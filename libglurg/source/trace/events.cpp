// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <stdexcept>
#include "glurg/common/fileStream.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/events.hpp"
#include "glurg/trace/traceFile.hpp"
#include "glurg/trace/values.hpp"

void glurg::Event::skip_backtrace(
	glurg::TraceFile& trace, glurg::FileStream& stream)
{
	std::uint32_t id = trace.read_unsigned_integer(stream);
	if (!trace.has_backtrace(id))
	{
		enum
		{
			backtrace_terminator = 0,
			backtrace_module_name = 1,
			backtrace_function_name = 2,
			backtrace_source_file_name = 3,
			backtrace_source_line_number = 4,
			backtrace_offset = 5
		};

		std::uint8_t detail = 0;
		do
		{
			stream.read(&detail, sizeof(std::uint8_t));
			switch (detail)
			{
				case backtrace_terminator:
					break;
				case backtrace_module_name:
				case backtrace_function_name:
				case backtrace_source_file_name:
					{
						// Single string.
						std::uint32_t length =
							trace.read_unsigned_integer(stream);
						stream.set_position(stream.get_position() + length);
					}
					break;
				case backtrace_source_line_number:
				case backtrace_offset:
					// Single unsigned integer.
					trace.read_unsigned_integer(stream);
					break;
				default:
					throw std::runtime_error("unknown backtrace detail op");
			}
		} while(detail != backtrace_terminator);
	}
}

glurg::EnterCallEvent::Thread glurg::EnterCallEvent::get_thread() const
{
	return this->thread;
}

const glurg::CallSignature* glurg::EnterCallEvent::get_call_signature() const
{
	return this->signature;
}

glurg::EnterCallEvent::ArgumentCount
glurg::EnterCallEvent::get_num_arguments() const
{
	return (ArgumentCount)this->arguments.size();
}

const glurg::Value* glurg::EnterCallEvent::get_argument_at(ArgumentIndex index)
{
	return this->arguments.at(index).get();
}

glurg::EnterCallEvent* glurg::EnterCallEvent::read(
	Type type, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	EnterCallEvent* event = new EnterCallEvent();

	event->thread = trace.read_unsigned_integer(stream);
	CallSignature::ID signature_id = trace.read_unsigned_integer(stream);
	if (!trace.has_call_signature(signature_id))
	{
		CallSignature* s = CallSignature::read(signature_id, trace, stream);
		trace.register_call_signature(s);
	}

	event->signature = trace.get_call_signature(signature_id);
	event->arguments.resize(event->signature->get_num_parameters());

	std::uint8_t detail_type = 0;
	do
	{
		stream.read(&detail_type, sizeof(std::uint8_t));

		switch (detail_type)
		{
			case call_detail_terminator:
				continue;
			case call_detail_argument:
				{
					ArgumentIndex index = trace.read_unsigned_integer(stream);
					Value* value = trace.read_value(stream);

					event->arguments.at(index) = ValuePointer(value);
				}
				break;
			case call_detail_return:
				// Not sure how apitrace does return values yet. Is it done in
				// leave events only?
				//
				// If enter events can have return values, then have enter
				// events store the return value. Have friend with leave event,
				// and then if leave event has return value, retroactively
				// assign it to enter event.
				throw std::runtime_error(
					"unexpected return value in enter event");
			case call_detail_thread:
				// Eat input argument.
				trace.read_unsigned_integer(stream);
				break;
			case call_detail_backtrace:
				skip_backtrace(trace, stream);
				break;
		}
	} while (detail_type != call_detail_terminator);

	return event;
}
