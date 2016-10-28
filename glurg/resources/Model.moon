-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"
Mesh = require "glurg.resources.Mesh"
Call = require "glurg.trace.Mesh"

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
		@_model\positions = positions._mesh
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

		index_format = marshal_model_enumeration('index_format', index_format)
		@_model\set_index_data(index_format, value.data, value.length / 3)

		return true

	save_dae: (filename) =>
		Promise.keep("filename", Promise.IsString(filename))

		exporter = glurg.common.ModelExporter.new!
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
			model.normals = @normals._mesh
		else
			model.normals = nil

		model.num_texture_coordinates = #@texture_coordinates
		for i = 1, #@texture_coordinates
			model\set_texture_coordinates(i - 1, @texture_coordinates[i]._mesh)

		model.num_colors = #@colors
		for i = 1, #@colors
			model\set_colors(i - 1, @colors[i]._mesh)

return Model
