-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Promise = require "glurg.common.Promise"
Filter = require "glurg.filters.Filter"
Call = require "glurg.trace.Call"
Trace = require "glurg.trace.Trace"

class Shader
	new: (name, shader_type) =>
		@name = name
		@type = shader_type
		@source = ""
		@is_deleted = false

	_extract_source: (call) =>
		Promise.keep("@name == shader",
			@name == call\get_argument_by_name("shader")\query!)

		s = {}
		argument = call\get_argument_by_name("string")
		for i = 1, #argument
			table.insert(s, argument[i]\query!)
		@source = table.concat(s, "\n")

class Program
	new: (name) =>
		@name = name
		@attributes = {}
		@shaders = {}
		@is_deleted = false

	_set_attribute: (name, index) =>
		Promise.keep("name", Promise.IsString(name))
		Promise.keep("index", Promise.IsNumber(index))

		@attributes[name] = index
		@attributes[index] = name

	get_attribute: (key) =>
		return switch type(key)
			when 'number', 'string' then @attributes[key]
			else error "invalid key type, expected string or number"

	has_attribute: (key) =>
		return switch type(key)
			when 'number', 'string' then @attributes[key] != nil
			else error "invalid key type, expected string or number"

	_attach_shader: (shader) =>
		Promise.keep("shader", Promise.IsClass(shader, Shader))

		if @shaders[shader.type] == nil
			@shaders[shader.type] = {}

		table.insert(@shaders[shader.type], shader)

	_detach_shader: (shader) =>
		Promise.keep("shader", Promise.IsClass(shader, Shader))

		if @shaders[shader.type]
			for i = 1, #@shaders[shader.type]
				if @shaders[shader.type][i] == shader then
					table.remove(@shaders[shader.type], i)
					break

	get_shader_source: (shader_type) =>
		source = [s for i, s in ipairs(@shaders[shader_type] or {})]
		return table.concat(source, "\n")

class ProgramFilter extends Filter
	new: =>
		@_shaders = {}
		@_programs = {}
		@current_program = 0

	_get_shader: (name) =>
		Promise.keep("@_shaders[name] != nil", @_shaders[name] != nil)

		return @_shaders[name]

	_add_shader: (shader) =>
		Promise.keep("@_shaders[shader.name] == nil",
			@_shaders[shader.name] == nil)

		@_shaders[shader.name] = shader

	_get_program: (name) =>
		Promise.keep("@_programs[name] != nil", @_programs[name] != nil)

		return @_programs[name]

	_add_program: (program) =>
		Promise.keep("@_programs[program.name] == nil",
			@_programs[program.name] == nil)

		@_programs[program.name] = program

	get_current_program: =>
		return @\_get_program(@current_program)

	glCreateShader: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		shader_type = call\get_argument_by_name("type")\query!.value_name
		name = call.result\query!

		@\_add_shader(Shader(name, shader_type))

		return false

	glShaderSource: (trace, call)
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		name = call\get_argument_by_name("shader")\query!
		@\_get_shader(name)\_extract_source(call)

		return false

	glCreateProgram: (trace, call)
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		name = call.result\query!
		@\_add_program(Program(name))

		return false

	glAttachShader: (trace, call)
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		shader = call\get_argument_by_name("shader")\query!

		@\_get_program(program)\_attach_shader(@\_get_shader(shader))

		return false

	glDetachShader: (trace, call)
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		shader = call\get_argument_by_name("shader")\query!

		@\_get_program(program)\_detach_shader(@\_get_shader(shader))

		return false

	glDeleteShader: (trace, call)
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		shader = call\get_argument_by_name("shader")\query!
		@\_get_shader(shader).is_deleted = true

		return false

	glDeleteProgram: (trace, call)
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		@\_get_program(program).is_deleted = true

		return false

	glBindAttribLocation: (trace, call)
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		name = call\get_argument_by_name("name")\query!
		index = call\get_argument_by_name("index")\query!

		@\_get_program(program)\_set_attribute(name, index)

		return false

return ProgramFilter
