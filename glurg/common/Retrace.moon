-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"

class Retrace
	new: (trace_filename, call) =>
		Promise.keep("trace_filename", Promise.IsString(trace_filename))
		Promise.keep_any("call", Promise.IsNumber(call), Promise.IsTable(call))

		@_process = glurg.common.Process.new!
		@_process.program_name = "glretrace"

		@_process\push_program_argument("-D")
		if type(call) == "number"
			@_process\push_program_argument(tostring(call))
		else
			@_process\push_program_argument(table.concat(call, ","))

		@_process\push_program_argument("--dump-format")
		@_process\push_program_argument("json")

		@_process\push_program_argument(trace_filename)

	start: =>
		@_process\open(glurg.common.process_mode_read)

	stop: =>
		@_process\close!

	next: =>
		render_state = glurg.resources.RenderState.new!
		render_state\populate(@_process.stream)

		return render_state

return Retrace
