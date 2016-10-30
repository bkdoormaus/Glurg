-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Promise = require "glurg.common.Promise"

class Log
	new: =>
		@message_threshold = 'high'

	_should_print: (importance) =>
		Promise.keep_any("importance",
			importance == 'high', importance == 'medium', importance == 'low')

		return switch @message_threshold
			when 'high'
				importance == 'high'
			when 'medium'
				importance == 'high' or importance == 'medium'
			when 'low'
				true
			else
				assert(false, "invalid verbosity threshold")

	_print: (importance, prefix, message, ...) =>
		Promise.keep("importance", Promise.IsString(importance))
		Promise.keep("prefix", Promise.IsString(prefix))
		Promise.keep("message", Promise.IsString(message))

		if @\_should_print(importance)
			if prefix != ""
				io.stdout\write("[", prefix, "]", " ")
			
			io.stdout\write(string.format(message, ...), "\n")
			io.stdout\flush!

	info: (importance, message, ...) =>
		@\_print(importance, "", message, ...)

	warn: (importance, message, ...) =>
		@\_print(importance, "warning", message, ...)

	error: (message, ...) =>
		@\_print('high', "error", message, ...)

return Log!
