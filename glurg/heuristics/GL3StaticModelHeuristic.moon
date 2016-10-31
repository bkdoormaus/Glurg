-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Log = require "glurg.common.Log"
Promise = require "glurg.common.Promise"
BufferFilter = require "glurg.filters.BufferFilter"
ProgramFilter = require "glurg.filters.ProgramFilter"
VertexArrayFilter = require "glurg.filters.VertexArrayFilter"
Model = require "glurg.resources.Model"
Mesh = require "glurg.resources.Mesh"
MeshBlobBuilder = require "glurg.resources.MeshBlobBuilder"
Call = require "glurg.trace.Call"
Trace = require "glurg.trace.Trace"

class GL3StaticModelHeuristic
	new: (data) =>
		@array_buffer_filter = BufferFilter("GL_ARRAY_BUFFER")
		@element_array_buffer_filter = BufferFilter("GL_ELEMENT_ARRAY_BUFFER")
		@program_filter = ProgramFilter!
		@vertex_array_filter = VertexArrayFilter!
		@attributes =
			position_binding: data["position"]
			normal_binding: data["normal"]
			texture_coordinate_binding: data["texture"]
		@output_directory = data["output-dir"] or "."

	get_filters: =>
		return @array_buffer_filter, @element_array_buffer_filter, @program_filter, @vertex_array_filter

	make_mesh: (attribute_name) =>
		program = @program_filter\get_current_program!
		attribute_index = program\get_attribute(attribute_name)

		vertex_array = @vertex_array_filter\get_current_vertex_array!
		description = vertex_array.descriptions[attribute_index]

		buffer = @array_buffer_filter\get_buffer(description.array_buffer)

		builder = MeshBlobBuilder!
		extracted_description = builder\extract_attrib_call(description.call)
		extracted_buffer = builder\extract_buffer_call(buffer.data)
		if extracted_description and extracted_buffer
			return Mesh(builder\build!)

	make_model: (positions_mesh, normals_mesh, texture_coordinates_mesh, call) =>
		model = Model(positions_mesh)
		model.normals = normals_mesh
		model.texture_coordinates[1] = texture_coordinates_mesh

		index_format = switch call\get_argument_by_name("type")\query!.value_name
			when 'GL_UNSIGNED_BYTE' then index_format = 'unsigned_byte'
			when 'GL_UNSIGNED_SHORT' then index_format = 'unsigned_short'
			when 'GL_UNSIGNED_INT' then index_format = 'unsigned_integer'
			else error "unknown index type"

		vertex_array = @vertex_array_filter\get_current_vertex_array!
		element_array_buffer = @element_array_buffer_filter\get_buffer(vertex_array.element_array_buffer)

		model\extract_index_data(index_format, element_array_buffer.data)

		return model

	is_compatible: =>
		program = @program_filter\get_current_program!

		vertices = program\has_attribute(@attributes.position_binding)
		normals = program\has_attribute(@attributes.normal_binding)
		textures = program\has_attribute(@attributes.texture_coordinate_binding)

		return vertices and normals and textures

	save_model: (call) =>
		if @\is_compatible!
			positions_mesh = @\make_mesh(@attributes.position_binding)
			normals_mesh = @\make_mesh(@attributes.normal_binding)
			texture_coordinates_mesh = @\make_mesh(@attributes.texture_coordinate_binding)

			if positions_mesh and normals_mesh and texture_coordinates_mesh
				model = @\make_model(positions_mesh, normals_mesh, texture_coordinates_mesh, call)

				filename = string.format("%s/%010d.dae", @output_directory, call.index)
				model\save_dae(filename)

				Log\info('medium', "saved model: #{filename}")


	glDrawElements: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		@\save_model(call)

		return false

	glDrawRangeElements: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		@\save_model(call)

		return false

return GL3StaticModelHeuristic
