-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Blob = require "glurg.common.Blob"
Promise = require "glurg.common.Promise"
Mesh = require "glurg.resources.Mesh"
Call = require "glurg.trace.Call"

marshal_model_enumeration = (enumeration, value) ->
	Promise.keep("enumeration", Promise.IsString(enumeration))
	Promise.keep("value", Promise.IsString(value))

	v = glurg.resources.model["#{enumeration}_#{value}"]
	if not v then
		error "enumeration not found"

	return v

class Model
	new: (positions) =>
		Promise.keep("positions", Promise.IsClass(positions, Mesh))

		@_model = glurg.resources.Model.new!
		@_model.positions = positions._mesh
		@num_vertices = positions.num_values
		@positions = positions
		@normals = false
		@texture_coordinates = {}
		@colors = {}

	extract_index_data: (index_format, call) =>
		Promise.keep("index_format", Promise.IsString(index_format))
		Promise.keep("call", Promise.IsClass(call, Call))

		value = call\get_argument_by_name("data")\query!
		if value == nil or value.length == 0 then
			return false

		@\set_index_data(index_format, value.data, value.length)

		return true

	set_index_data: (index_format, blob) =>
		Promise.keep("index_format", Promise.IsString(index_format))
		Promise.keep("blob", Promise.IsClass(blob, Blob))

		index_size = switch index_format
			when 'unsigned_byte' then 1
			when 'unsigned_short' then 2
			when 'unsigned_integer' then 4
			else error "invalid index format"

		index_format = marshal_model_enumeration('index_format', index_format)
		index_count = (blob.length / index_size)
		assert(index_count % 3 == 0)
		@_model\set_index_data(index_format, blob.data, index_count)

	save_dae: (filename) =>
		Promise.keep("filename", Promise.IsString(filename))

		exporter = glurg.resources.ModelExporter.new!
		outputBuffer = glurg.common.DataBuffer.new!

		@\_refresh!

		exporter.model = @_model
		exporter\to_dae(outputBuffer)

		fileStream = glurg.common.SimpleFileStream.new!
		fileStream\open(filename, glurg.common.file_mode_write)
		fileStream\write(outputBuffer.data, outputBuffer.size)
		fileStream\close!

	_refresh: =>
		if @normals
			@_model.normals = @normals._mesh
		else
			@_model.normals = nil

		@_model.num_texture_coordinates = #@texture_coordinates
		for i = 1, #@texture_coordinates
			@_model\set_texture_coordinates(i - 1, @texture_coordinates[i]._mesh)

		@_model.num_colors = #@colors
		for i = 1, #@colors
			@_model\set_colors(i - 1, @colors[i]._mesh)

return Model
