// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cassert>
#include <stdexcept>
#include "glurg/common/fileStream.hpp"
#include "glurg/trace/call.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/events.hpp"
#include "glurg/trace/traceFile.hpp"
#include "glurg/trace/values.hpp"

glurg::Event::Type glurg::Event::get_type() const
{
	return this->type;
}

glurg::Call::Index glurg::Event::get_call_index() const
{
	return this->call_index;
}

glurg::Event* glurg::Event::read(glurg::TraceFile& trace, glurg::FileStream& stream)
{
	std::uint8_t type;
	stream.read(&type, sizeof(std::uint8_t));

	Call* call = nullptr;
	if (type == event_enter)
	{
		Call::Thread thread = trace.read_unsigned_integer(stream);

		CallSignature::ID signature_id = trace.read_unsigned_integer(stream);
		if (!trace.has_call_signature(signature_id))
		{
			CallSignature* s = CallSignature::read(signature_id, trace, stream);
			trace.register_call_signature(s);
		}

		call = trace.create_call(signature_id);
	}
	else if (type == event_leave)
	{
		call = trace.get_call(trace.read_unsigned_integer(stream));
	}
	else
	{
		throw std::runtime_error("unknown event type");
	}

	Event* event = new Event();
	event->type = type;
	event->call_index = call->get_call_index();

	parse_call_detail(call, trace, stream);

	return event;
}

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
						trace.read_string(stream);
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

void glurg::Event::parse_call_detail(
	glurg::Call* call, glurg::TraceFile& trace, glurg::FileStream& stream)
{
	std::uint8_t detail_type = 0;
	do
	{
		stream.read(&detail_type, sizeof(std::uint8_t));

		switch (detail_type)
		{
			case call_detail_terminator:
				break;
			case call_detail_argument:
				{
					Call::ArgumentIndex index = trace.read_unsigned_integer(stream);
					Value* value = trace.read_value(stream);

					call->set_argument_at(index, value);
					delete value;
				}
				break;
			case call_detail_return:
				call->set_return_value(trace.read_value(stream));
				break;
			case call_detail_thread:
				// Eat input argument.
				//
				// This type shouldn't even be in the supported trace files, but
				// play it safe.
				trace.read_unsigned_integer(stream);
				break;
			case call_detail_backtrace:
				{
					std::uint32_t count = trace.read_unsigned_integer(stream);

					for (std::uint32_t i = 0; i < count; ++i)
					{
						skip_backtrace(trace, stream);
					}
				}
				break;
			default:
				throw std::runtime_error("unknown call detail op");
		}
	} while (detail_type != call_detail_terminator);
}
