-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Event = require "glurg.trace.Event"
Types = require "glurg.common.Types"

class Trace
	new: (filename) =>
		@_fileStream = glurg.common.SnappyFileStream.new!
		@_fileStream\open(filename, glurg.common.file_mode_read)

		@_traceFile = glurg.trace.TraceFile.new!
		if not @_traceFile\verify_is_compatible_version(@_fileStream)
			error "incompatible stream"

		@is_end_of_file = false

	next_event: =>
		e = glurg.trace.read_event(@_traceFile, @_fileStream)
		@is_end_of_file = @_fileStream.is_end_of_file

		return Event(e)

	get_call: (call_index) =>
		Types.match("call_index", Types.Number(call_index))

		return @_traceFile\get_call(call_index)

return Trace
