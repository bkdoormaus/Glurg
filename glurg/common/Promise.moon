-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

class Promise
	check: =>
		return false, "promise not implemented"

class TypePromise
	get_value_type: =>
		return type(@value)

	get_match: =>
		return @\get_value_type! == @name

	check: =>
		return get_match!, "expected '#{@name}', got #{@get_value_type!}"

Promise = {}
Promise.IsNumber = class extends TypePromise
	new: (value) =>
		@name = "number"
		@value = value

Promise.IsString = class extends TypePromise
	new: (value) =>
		@name = "string"
		@value = value

Promise.IsBoolean = class extends TypePromise
	new: (value) =>
		@name = "boolean"
		@value = value

Promise.IsTable = class extends TypePromise
	new: (value) =>
		@name = "table"
		@value = value

Promise.IsUserdata = class extends TypePromise
	new: (value) =>
		@name = "userdata"
		@value = value

Promise.IsObject = class extends TypePromise
	new: (value, object) =>
		@value = value
		@name = object.__name

	get_value_type: =>
		return @value.__class.__name

check_promise = (arg) ->
	if type(arg) == 'boolean' then
		return arg, "condition not met"
	else
		return arg\check!

Promise.keep = (message, ...) ->
	args = { n: select("#", ...), ... }
	for i = 1, args.n
		promise_kept, reason = check_promise(args[i])
		if not promise_kept
			error "#{message}: #{reason}"

Promise.keep_any = (message, ...) ->
	args = { n: select("#", ...), ... }
	for i = 1, args.n
		promise_kept, reason = check_promise(args[i])
		if promise_kept
			return
	error "#{message}: no promises kept"

return Promise
