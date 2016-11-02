-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"
MeshBlob = require "glurg.resources.MeshBlob"
Call = require "glurg.trace.Call"

marshal_mesh_enumeration = (enumeration, value) ->
	Promise.keep("enumeration", Promise.IsString(enumeration))
	Promise.keep("value", Promise.IsString(value))


	v = glurg.resources.mesh["#{enumeration}_#{value}"]
	if not v then
		error "enumeration not found"

	return v

class MeshBlobBuilder
	new: =>
		@_builder = glurg.resources.MeshBlobBuilder.new!

		@\set_description_format('float')
		@\set_description_num_components(3)
		@\set_description_normalization('disabled')
		@\set_description_stride(0)
		@\set_description_offset(0)

		@has_vertex_data = false

	set_description_format: (format) =>
		@_builder\set_description_format(marshal_mesh_enumeration('format', format))

	set_description_num_components: (num_components) =>
		Promise.keep("num_components", Promise.IsNumber(num_components))
		Promise.keep("num_components >= 1", num_components >= 1)
		Promise.keep("num_components <= 4", num_components <= 4)

		@_builder\set_description_num_components(num_components)

	set_description_normalization: (normalization) =>
		@_builder\set_description_normalization(marshal_mesh_enumeration('normalization', normalization))

	set_description_stride: (stride) =>
		Promise.keep("stride", Promise.IsNumber(stride))
		Promise.keep("stride >= 0", stride >= 0)

		@_builder\set_description_stride(stride)

	set_description_offset: (offset) =>
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("offset >= 0", offset >= 0)

		@_builder\set_description_offset(offset)

	build: =>
		Promise.keep("@has_vertex_data", @has_vertex_data == true)

		return MeshBlob(@_builder\build!)

	extract_buffer_call: (call) =>
		Promise.keep("call", Promise.IsClass(call, Call))

		value = call\get_argument_by_name("data")\query!
		if value == nil or value.length == 0 then
			return false

		@_builder\set_vertex_data(value.data, value.length)
		@has_vertex_data = true

		return @has_vertex_data

	extract_buffer_call_slice: (call, offset, size) =>
		Promise.keep("call", Promise.IsClass(call, Call))
		Promise.keep("offset", Promise.IsNumber(offset))
		Promise.keep("size", Promise.IsNumber(size))
		Promise.keep("offset >= 0", offset >= 0)
		Promise.keep("size >= 1", size >= 1)

		value = call\get_argument_by_name("data")\query!
		if value == nil or value.length == 0 then
			return false

		if offset + size > value.length
			return false

		@_builder\set_vertex_data(glurg.common.slice_data(value.data, offset), size)
		@has_vertex_data = true

		return @has_vertex_data

	extract_attrib_call: (call) =>
		Promise.keep("call", Promise.IsClass(call, Call))

		-- It's an enumeration. "size" can be GL_BGRA, and so apitrace pushes
		-- the entire thing inside an enum.
		num_components = call\get_argument_by_name("size")\query!
		if num_components.value_name == 'GL_BGRA'
			@\set_description_num_components(4)
		else
			@\set_description_num_components(num_components.value\query!)
		
		format = call\get_argument_by_name("type")\query!
		switch format.value_name
			when 'GL_FLOAT' then format = 'float'
			when 'GL_BYTE' then format = 'byte'
			when 'GL_UNSIGNED_BYTE' then format = 'unsigned_byte'
			when 'GL_SHORT' then format = 'short'
			when 'GL_UNSIGNED_SHORT' then format = 'unsigned_short'
			when 'GL_INT' then format = 'integer'
			when 'GL_UNSIGNED_INT' then format = 'unsigned_integer'
			else error "unknown or unsupported data format"
		@\set_description_format(format)

		normalized = call\get_argument_by_name("normalized")\query!
		if normalized.value_name == "GL_TRUE"
			@\set_description_normalization('enabled')
		else
			@\set_description_normalization('disabled')

		@\set_description_stride(call\get_argument_by_name("stride")\query!)

		offset = call\get_argument_by_name("pointer")\query!
		if type(offset) == "number"
			@\set_description_offset(offset)
		else
			-- The type would be NULL, as a Blob of zero length
			@\set_description_offset(0)

		return true


return MeshBlobBuilder
