-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Trace = require "glurg.trace.Trace"

Args = { ... }
if #Args < 2
	io.stderr\write("print_trace_functions.moon <file.trace> <functions...>\n")

	return 1

print_call = (call, comment) ->
	io.stdout\write(call.signature.name)
	io.stdout\write("(")
	for i = 1, call.num_arguments
		io.stdout\write(call.signature\get_parameter_name(i))
		io.stdout\write(" = ")

		value = call\get_argument_by_index(i)
		io.stdout\write(value\to_string!)

		if i != call.num_arguments
			io.stdout\write(", ")

	io.stdout\write(");")
	if comment != nil
		io.stdout\write(" // #{comment}")
	io.stdout\write("\n")

Funcs = {}
for i = 2, #Args
	Funcs[Args[i]] = true

trace = Trace(Args[1])
while not trace.is_end_of_file
	event = trace\next_event!
	if event.type == 'leave' and Funcs[event.call.signature.name]
		print_call(event.call, event.call_index)
		trace\delete_call(event.call_index)
