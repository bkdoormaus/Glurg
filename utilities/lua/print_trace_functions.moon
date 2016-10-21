-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Trace = require "glurg.trace.Trace"

Args = { ... }
if #Args < 2
	io.stderr\write("print_trace_functions.moon <file.trace> <function>\n")

	return 1

print_call = (call, comment) ->
	io.stdout\write(call.signature.name)
	io.stdout\write("(")
	for i = 1, call.num_arguments
		io.stdout\write(call.signature\get_parameter_name(i))
		io.stdout\write(" = ")

		value = call\get_argument_by_index(i)
		local result
		switch value.type
			when 'nil' then result = "NULL"
			when 'blob' then result = "char[#{value\query!.length}]"
			when 'enumeration'
				result = value\query!.value_name
			when 'bitmask'
				result = string.format("0x%08x", value\query!.value)
			when 'array'
				result = string.format('object[%d]', #value\query!)
			when 'structure' then result = 'struct'
			else result = value\query!
		io.stdout\write(tostring(result))

		if i != call.num_arguments
			io.stdout\write(", ")

	io.stdout\write(");")
	if comment != nil
		io.stdout\write(" // #{comment}")
	io.stdout\write("\n")

trace = Trace(Args[1])
while not trace.is_end_of_file
	event = trace\next_event!
	if event.type == 'leave' and event.call.signature.name == Args[2]
		print_call(event.call, event.call_index)
		trace\delete_call(event.call_index)
