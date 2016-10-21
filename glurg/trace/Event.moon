-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Call = require "glurg.trace.Call"
Promise = require "glurg.common.Promise"

class Event
	new: (event, call) =>
		Promise.keep("event", Promise.IsUserdata(event))
		Promise.keep("call", Promise.IsUserdata(call))

		@_event = event
		@call = Call(call)
		@type = switch event.type
			when glurg.trace.event_enter then 'enter'
			when glurg.trace.event_leave then 'leave'
		@call_index = event.call_index + 1

return Event
