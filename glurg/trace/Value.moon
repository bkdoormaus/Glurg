-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"

local Value

class BitmaskSignature
	new: (signature) =>
		Promise.keep("signature", Promise.IsUserdata(signature))

		@_signature = signature
		@id = signature.id

	match: (value, mask) =>
		Promise.keep("value", Promise.IsNumber(value))
		Promise.keep("mask", Promise.IsString(value))

		return @_signature\match(value, mask)

class Bitmask
	new: (bitmask) =>
		Promise.keep("bitmask", Promise.IsUserdata(bitmask))

		@_bitmask = bitmask
		@bitmask = BitmaskSignature(bitmask.signature)
		@value = bitmask.value

class Blob
	new: (blob) =>
		Promise.keep("blob", Promise.IsUserdata(blob))

		@_blob = blob
		@data = blob.data
		@length = blob.length

class EnumerationSignature
	new: (signature) =>
		Promise.keep("signature", Promise.IsUserdata(signature))

		@_signature = signature
		@id = signature.id

	get_value_by_name: (name) =>
		Promise.keep("name", Promise.IsString(name))
		
		return Value(@_signature\get_value_by_name(name))

	get_name_by_value: (value) =>
		Promise.keep("value", Promise.IsClass(value, Value))

		return @_signature\get_name_by_value(value._value)

class Enumeration
	new: (enumeration) =>
		Promise.keep("enumeration", Promise.IsUserdata(enumeration))

		@_enumeration = enumeration
		@signature = EnumerationSignature(enumeration.signature)
		@value = Value(enumeration.value)
		@value_name = @signature\get_name_by_value(@value)

class Array
	new: (array) =>
		Promise.keep("array", Promise.IsUserdata(array))

		@_array = array
		for i = 1, array.size
			@[i] = Value(array)

class Value
	new: (value) =>
		Promise.keep("value", Promise.IsUserdata(value))

		@_value = value
		@type = switch value.type
			when glurg.trace.values.false_boolean then 'boolean'
			when glurg.trace.values.true_boolean then 'boolean'
			when glurg.trace.values.negative_integer then 'integer'
			when glurg.trace.values.positive_integer then 'integer'
			when glurg.trace.values.float then 'float'
			when glurg.trace.values.double then 'float'
			when glurg.trace.values.string then 'string'
			when glurg.trace.values.null_pointer then 'nil'
			when glurg.trace.values.blob then 'blob'
			when glurg.trace.values.enumeration then 'enumeration'
			when glurg.trace.values.bitmask then 'bitmask'
			when glurg.trace.values.array then 'array'
			when glurg.trace.values.structure then 'structure'
			when glurg.trace.values.handle then 'handle'
			else error "unknown value type"

	query: =>
		return switch @type
			when 'boolean' then @_value\to_boolean!
			when 'integer' then @_value\to_integer!
			when 'float' then @_value\to_float!
			when 'string' then @_value\to_string!
			when 'nil' then nil
			when 'blob' then Blob(@_value\to_blob!)
			when 'enumeration' then Enumeration(@_value\to_enumeration!)
			when 'bitmask' then Bitmask(@_value\to_bitmask!)
			when 'array' then Array(@_value\to_array!)
			when 'structure' then error "not yet implemented"
			when 'handle' then @_value\to_handle!
			else assert(false, "value type is invalid")

	clone: =>
		return Value(@_value\clone!)

return Value
