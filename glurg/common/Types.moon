-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Types = {}

class BaseType
	get_value_type: =>
		return type(@value)

	get_match: =>
		return @\get_value_type! == @name

Types.Number = class extends BaseType
	new: (value) =>
		@name = "number"
		@value = value

Types.String = class extends BaseType
	new: (value) =>
		@name = "string"
		@value = value

Types.Boolean = class extends BaseType
	new: (value) =>
		@name = "boolean"
		@value = value

Types.Table = class extends BaseType
	new: (value) =>
		@name = "table"
		@value = value

Types.Object = class extends BaseType
	new: (value, object) =>
		@value = value
		@name = object.__name

	get_value_type: =>
		return @value.__class.__name

Types.match = (message, ...) ->
	args = { n: select("#", ...), ... }
	for i = 1, args.n
		if args[i].match == false then
			error "#{message}: expected '#{args[i].name}', got #{type(args[i].value)}"

return Types
