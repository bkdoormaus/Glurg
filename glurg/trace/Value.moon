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
		if blob != nil
			Promise.keep("blob", Promise.IsUserdata(blob))

			@_blob = blob
			@data = blob.data
			@length = blob.length
			@is_mutable = false
		else
			@_blob = false
			@data = nil
			@length = 0
			@is_mutable = false

	get_mutable: =>
		blob = Blob(nil)
		blob.data = glurg.common.duplicate_data(@data, @length)
		blob.length = @length
		blob.is_mutable = true

		return blob

	slice: (offset) =>
		Promise.keep("offset <= @length", offset <= @length)

		return glurg.common.slice_data(@data, offset)

	dispose: =>
		Promise.keep("@is_mutable == true", @is_mutable)
		glurg.common.delete_data(@data)
		@data = nil
		@length = 0

	write_byte: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("value", Promise.IsNumber(value))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_byte(@data, offset, value)

	write_unsigned_byte: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_unsigned_byte(@data, offset, value)

	write_short: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_short(@data, offset, value)

	write_unsigned_short: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_unsigned_short(@data, offset, value)

	write_integer: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_integer(@data, offset, value)

	write_unsigned_integer: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_unsigned_integer(@data, offset, value)

	write_float: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_float(@data, offset, value)

	read_byte: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_byte(@data, offset)

	read_unsigned_byte: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_unsigned_byte(@data, offset)

	read_short: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_short(@data, offset)

	read_unsigned_short: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_unsigned_short(@data, offset)

	read_integer: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_integer(@data, offset)

	read_unsigned_integer: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_unsigned_integer(@data, offset)

	read_float: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset <= @length", offset <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_float(@data, offset)

class EnumerationSignature
	new: (signature) =>
		Promise.keep("signature", Promise.IsUserdata(signature))

		@_signature = signature
		@id = signature.id

	get_value_by_name: (name) =>
		Promise.keep("name", Promise.IsString(name))
		
		return Value(@_signature\get_value_by_name(name))

	has_value: (value) =>
		Promise.keep("value", Promise.IsClass(value, Value))

		return @_signature\has_value(value._value)

	get_name_by_value: (value) =>
		Promise.keep("value", Promise.IsClass(value, Value))

		return @_signature\get_name_by_value(value._value)

class Enumeration
	new: (enumeration) =>
		Promise.keep("enumeration", Promise.IsUserdata(enumeration))

		@_enumeration = enumeration
		@signature = EnumerationSignature(enumeration.signature)
		@value = Value(enumeration.value)
		if @signature\has_value(@value)
			@value_name = @signature\get_name_by_value(@value)

class Array
	new: (array) =>
		Promise.keep("array", Promise.IsUserdata(array))

		@_array = array
		for i = 1, array.size
			@[i] = Value(array\get_value_at(i - 1))

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
			when 'float' then @_value\to_number!
			when 'string' then @_value\to_string!
			when 'nil' then nil
			when 'blob' then Blob(@_value\to_blob!)
			when 'enumeration' then Enumeration(@_value\to_enumeration!)
			when 'bitmask' then Bitmask(@_value\to_bitmask!)
			when 'array' then Array(@_value\to_array!)
			when 'structure' then error "not yet implemented"
			when 'handle' then @_value\to_handle!
			else assert(false, "value type is invalid")

	to_string: =>
		query = @\query!
		result = switch @type
			when 'nil' then "NULL"
			when 'blob' then "ubyte[#{query.length}]"
			when 'enumeration' 
				if not query.value_name then
					tostring(query.value\query!)
				else
					query.value_name
			when 'bitmask' then string.format("0x%08x", query.value)
			when 'array'
				"[%s]"\format(table.concat([i\to_string! for i in *query], ", "))
			when 'structure' then 'struct {}'
			else tostring(query)
		return result

	clone: =>
		return Value(@_value\clone!)

return Value
