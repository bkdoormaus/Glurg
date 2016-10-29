-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Model = require "glurg.resources.Model"
Mesh = require "glurg.resources.Mesh"
MeshBlobBuilder = require "glurg.resources.MeshBlobBuilder"
Trace = require "glurg.trace.Trace"

Args = { ... }
if #Args < 5
	io.stderr\write("save_models.moon <file.trace> <position-attrib> <normal-attrib> <texture-attrib> <output-dir>\n")

	return 1

local trace

Attribs =
	position: Args[2]
	normal: Args[3]
	texture: Args[4]

GLState =
	buffer: {}
	program: {}
	vao: {}
	current_program: 0
	current_vao: 0
	current_array_buffer: 0
	current_element_array_buffer: 0

get_buffer = (id) ->
	if GLState.buffer[id] == nil
		GLState.buffer[id] = {}

	return GLState.buffer[id]

update_buffer = (id, call) ->
	e = get_buffer(id)
	if e.call then
		trace\delete_call(e.call.index)
	e.call = call

delete_buffer = (id) ->
	if GLState.buffer[id]
		if GLState.buffer[id].call
			trace\delete_call(GLState.buffer[id].call.index)

	GLState.buffer[id] = nil

get_program = (id) ->
	if GLState.program[id] == nil
		GLState.program[id] = { attribs: {} }

	return GLState.program[id]

update_program = (id, call) ->
	e = get_program(id)

	index = call\get_argument_by_name("index")\query!
	name = call\get_argument_by_name("name")\query!
	e.attribs[index] = name
	e.attribs[name] = index

delete_program = (id) ->
	GLState.program[id] = nil

get_vao = (id) ->
	if GLState.vao[id] == nil
		GLState.vao[id] = { meshes: {} }

	return GLState.vao[id]

update_vao_attrib = (id, call) ->
	e = get_vao(id)

	index = call\get_argument_by_name("index")\query!

	if GLState.current_array_buffer != 0
		buffer = get_buffer(GLState.current_array_buffer)

		builder = MeshBlobBuilder!
		extracted_description = builder\extract_attrib_call(call)
		extracted_buffer = builder\extract_buffer_call(buffer.call)
		if extracted_description and extracted_buffer then
			blob = builder\build!
			e.meshes[index] = Mesh(blob)

delete_vao = (id) ->
	GLState.vao[id] = nil

save_model = (call) ->
	program = get_program(GLState.current_program)

	position_index = program.attribs[Attribs.position]
	normal_index = program.attribs[Attribs.normal]
	texture_index = program.attribs[Attribs.texture]

	if not position_index or not normal_index or not texture_index then
		io.stdout\write("program doesn't have matching attributes\n")
		return false

	vao = get_vao(GLState.current_vao)
	position_mesh = vao.meshes[position_index]
	normal_mesh = vao.meshes[normal_index]
	texture_mesh = vao.meshes[texture_index]

	print("num_components", texture_mesh.blob.vertex_description.num_components)
	print("texture_index", texture_index)

	if not position_mesh or not normal_mesh or not texture_mesh
		io.stdout\write("VAO doesn't have matching meshes\n")
		return false
	if not vao.elements then
		io.stdout\write("VAO doesn't have index data\n")
		return false

	model = Model(position_mesh)
	model.normals = normal_mesh
	model.texture_coordinates[1] = texture_mesh

	index_format = switch call\get_argument_by_name("type")\query!.value_name
		when 'GL_UNSIGNED_BYTE' then index_format = 'unsigned_byte'
		when 'GL_UNSIGNED_SHORT' then index_format = 'unsigned_short'
		when 'GL_UNSIGNED_INT' then index_format = 'unsigned_integer'
		else return false

	model\extract_index_data(index_format, get_buffer(vao.elements).call)

	print(call.index)
	model\save_dae(string.format("%s/%010d.dae", Args[5], call.index))

trace = Trace(Args[1])
while not trace.is_end_of_file
	event = trace\next_event!
	if event.type == 'leave'
		switch event.call.signature.name
			when "glBindBuffer"
				target = event.call\get_argument_by_name("target")\query!
				buffer = event.call\get_argument_by_name("buffer")\query!
				if target.value_name == "GL_ELEMENT_ARRAY_BUFFER"
					GLState.current_element_array_buffer = buffer
					if GLState.current_vao != 0 then
						vao = get_vao(GLState.current_vao)
						vao.elements = buffer
				elseif target.value_name == "GL_ARRAY_BUFFER"
					GLState.current_array_buffer = buffer
				trace\delete_call(event.call_index)
			when "glBindVertexArray"
				GLState.current_vao = event.call\get_argument_by_name("array")\query!
				trace\delete_call(event.call_index)
			when "glUseProgram"
				GLState.current_program = event.call\get_argument_by_name("program")\query!
			when "glBindAttribLocation"
				program = event.call\get_argument_by_name("program")\query!
				update_program(program, event.call)
			when "glVertexAttribPointer"
				update_vao_attrib(GLState.current_vao, event.call)
			when "glBufferData"
				target = event.call\get_argument_by_name("target")\query!
				if target.value_name == "GL_ELEMENT_ARRAY_BUFFER"
					update_buffer(GLState.current_element_array_buffer, event.call)
				elseif target.value_name == "GL_ARRAY_BUFFER"
					update_buffer(GLState.current_array_buffer, event.call)
			when "glDrawElements", "glDrawRangeElements"
				if save_model(event.call) then
					io.stdout\write("saved model from call #{event.call_index}\n")
				else
					io.stdout\write("couldn't save model from call #{event.call_index}\n")
				trace\delete_call(event.call_index)
			else
				trace\delete_call(event.call_index)
