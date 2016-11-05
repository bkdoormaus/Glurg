-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"

class Blob
	new: (blob) =>
		if type(blob) == "userdata"
			Promise.keep("blob", Promise.IsUserdata(blob))

			@_blob = blob
			@data = blob.data
			@length = blob.length
			@is_mutable = false
		elseif type(blob) == "number"
			Promise.keep("blob >= 1", blob >= 1)

			@_blob = false
			@data = glurg.common.create_data(blob)
			@length = blob
			@is_mutable = true
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

	copy: (other, num_bytes, offset) =>
		offset = offset or 0

		Promise.keep("other", Promise.IsClass(other, Blob))
		Promise.keep("num_bytes", Promise.IsNumber(num_bytes))
		Promise.keep("num_bytes >= 0", num_bytes >= 0)
		Promise.keep("num_bytes <= @length", num_bytes <= @length)
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("offset + num_bytes <= other.length",
			offset + num_bytes <= other.length)

		glurg.common.copy_data(@data, other\slice(offset), num_bytes)

	calculate_hash: (offset, size) =>
		offset = offset or 0
		size = size or @length

		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("size", Promise.IsNumber(size))
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("size >= 1", size >= 1)
		Promise.keep("offset + size <= @length", offset + size <= @length)

		return glurg.common.hash_data(@\slice(offset), size)\to_string!

	write_byte: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("value", Promise.IsNumber(value))
		Promise.keep("offset + 1 <= @length", offset + 1 <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_byte(@data, offset, value)

	write_unsigned_byte: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 1 <= @length", offset + 1 <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_unsigned_byte(@data, offset, value)

	write_short: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 2 <= @length", offset + 2 <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_short(@data, offset, value)

	write_unsigned_short: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 2 <= @length", offset + 2 <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_unsigned_short(@data, offset, value)

	write_integer: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 4 <= @length", offset + 4 <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_integer(@data, offset, value)

	write_unsigned_integer: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 4 <= @length", offset + 4 <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_unsigned_integer(@data, offset, value)

	write_float: (offset, value) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 4 <= @length", offset + 4 <= @length)
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("@is_mutable == true", @is_mutable)

		return glurg.common.data.write_float(@data, offset, value)

	read_byte: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 1 <= @length", offset + 1 <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_byte(@data, offset)

	read_unsigned_byte: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 1 <= @length", offset + 1 <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_unsigned_byte(@data, offset)

	read_short: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 2 <= @length", offset + 2 <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_short(@data, offset)

	read_unsigned_short: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 2 <= @length", offset + 2 <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_unsigned_short(@data, offset)

	read_integer: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 4 <= @length", offset + 4 <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_integer(@data, offset)

	read_unsigned_integer: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 4 <= @length", offset + 4 <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_unsigned_integer(@data, offset)

	read_float: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset + 4 <= @length", offset + 4 <= @length)
		Promise.keep("offset >= 0", offset >= 0)

		return glurg.common.data.read_float(@data, offset)

return Blob
