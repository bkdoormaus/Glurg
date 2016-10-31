-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Log = require "glurg.common.Log"
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
		@uniforms = {}
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

	_add_uniform: (name, location) =>
		if location == -1
			return false

		uniform = { call: false }
		if @uniforms[name] == nil
			Promise.keep("@uniforms[location] == nil",
				@uniforms[location] == nil)

			@uniforms[name] = uniform
			@uniforms[location] = uniform

		return true

	_set_uniform: (key, call, trace) =>
		Promise.keep_any("key", Promise.IsNumber(key), Promise.IsString(key))
		Promise.keep("call", Promise.IsClass(call, Call))
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("@uniforms[key] != nil", @uniforms[key] != nil)

		if @uniforms[key].call then
			trace\delete_call(@uniforms[key].call.index)
		@uniforms[key].call = call

	get_uniform: (key) =>
		Promise.keep_any("key", Promise.IsNumber(key), Promise.IsString(key))

		if @uniforms[key] and @uniforms[key].call then
			call = @uniforms[key].call
			switch call.signature.name
				when "glUniform1f", "glUniform1i", "glUniform1ui"
					v0 = call\get_argument_by_name("v0")\query!
					return v0, 'scalar'
				when "glUniform2f", "glUniform2i", "glUniform2ui"
					v0 = call\get_argument_by_name("v0")\query!
					v1 = call\get_argument_by_name("v1")\query!
					return { x: v0, y: v1 }, 'vec2'
				when "glUniform3f", "glUniform3i", "glUniform3ui"
					v0 = call\get_argument_by_name("v0")\query!
					v1 = call\get_argument_by_name("v1")\query!
					v2 = call\get_argument_by_name("v2")\query!
					return { x: v0, y: v1, z: v2 }, 'vec3'
				when "glUniform4f", "glUniform4i", "glUniform4ui"
					v0 = call\get_argument_by_name("v0")\query!
					v1 = call\get_argument_by_name("v1")\query!
					v2 = call\get_argument_by_name("v2")\query!
					v4 = call\get_argument_by_name("v4")\query!
					return { x: v0, y: v1, z: v2, w: v4 }, 'vec4'
				when "glUniform1fv", "glUniform1iv", "glUniform1uiv" 
					count = call\get_argument_by_name("count")\query!
					value = call\get_argument_by_name("value")\query!
					if count == 1
						return value, 'scalar'
					else
						return value, 'scalar_array'
				when "glUniform2fv", "glUniform2iv", "glUniform2uiv"
					count = call\get_argument_by_name("count")\query!
					value = call\get_argument_by_name("value")\query!
					result = {}
					for i = 1, count
						result[i] = { 
							x: value[i * 2 + 0],
							y: value[i * 2 + 1]
						}
					if count == 1
						return value[1], 'vec2'
					else
						return value, 'vec2_array'
				when "glUniform3fv", "glUniform3iv", "glUniform3uiv"
					count = call\get_argument_by_name("count")\query!
					value = call\get_argument_by_name("value")\query!
					result = {}
					for i = 1, count
						result[i] = {
							x: value[i * 3 + 0],
							y: value[i * 3 + 1],
							z: value[i * 3 + 2]
						}
					if count == 1
						return value[1], 'vec3'
					else
						return value, 'vec3_array'
				when "glUniform4fv", "glUniform4iv", "glUniform4uiv"
					count = call\get_argument_by_name("count")\query!
					value = call\get_argument_by_name("value")\query!
					result = {}
					for i = 1, count
						result[i] = {
							x: value[i * 4 + 0],
							y: value[i * 4 + 1],
							z: value[i * 4 + 2],
							w: value[i * 4 + 3]
						}
					if count == 1
						return value[1], 'vec4'
					else
						return value, 'vec4_array'
				when "glUniformMatrix2fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				when "glUniformMatrix3fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				when "glUniformMatrix4fv"
					count = call\get_argument_by_name("count")\query!
					transpose = call\get_argument_by_name("transpose")\query!
					value = call\get_argument_by_name("value")\query!
					result = {}
					for i = 1, count
						result[i] = {}
						if transpose.value_name == "GL_TRUE"
							for j = 1, 4 do
								result[i][j] = {}
							for j = 1, 4 do
								result[i][1][j] = value[i * 16 + j * 4 + 0]
								result[i][2][j] = value[i * 16 + j * 4 + 1]
								result[i][3][j] = value[i * 16 + j * 4 + 2]
								result[i][4][j] = value[i * 16 + j * 4 + 3]
						else
							for j = 1, 4 do
								result[i][j] = {
									value[i * 16 + j * 4 + 0],
									value[i * 16 + j * 4 + 1],
									value[i * 16 + j * 4 + 2],
									value[i * 16 + j * 4 + 3]
								}

					if count == 1
						return result[1], 'mat4'
					else
						return result, 'mat4_array'
				when "glUniformMatrix2x3fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				when "glUniformMatrix2x4fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				when "glUniformMatrix3x2fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				when "glUniformMatrix3x4fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				when "glUniformMatrix4x2fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				when "glUniformMatrix4x3fv"
					Log\warn('high', "cannot get uniform; type not yet implemented")
					return nil
				else
					Log\error("unknown glUniform call")
					return nil
		else
			return nil

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

	_remove_shader: (shader) =>
		@_shaders[shader] = nil

	_get_program: (name) =>
		Promise.keep("@_programs[name] != nil", @_programs[name] != nil)

		return @_programs[name]

	_add_program: (program) =>
		Promise.keep("@_programs[program.name] == nil",
			@_programs[program.name] == nil)

		@_programs[program.name] = program

	_remove_program: (program) =>
		@_programs[program] = nil

	get_current_program: =>
		return @\_get_program(@current_program)

	glCreateShader: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		shader_type = call\get_argument_by_name("type")\query!.value_name
		name = call.result\query!

		@\_add_shader(Shader(name, shader_type))

		return false

	glShaderSource: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		name = call\get_argument_by_name("shader")\query!
		@\_get_shader(name)\_extract_source(call)

		return false

	glCreateProgram: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		name = call.result\query!
		@\_add_program(Program(name))

		return false

	glUseProgram: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		@current_program = call\get_argument_by_name("program")\query!

		return false

	glAttachShader: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		shader = call\get_argument_by_name("shader")\query!

		@\_get_program(program)\_attach_shader(@\_get_shader(shader))

		return false

	glDetachShader: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		shader = call\get_argument_by_name("shader")\query!

		@\_get_program(program)\_detach_shader(@\_get_shader(shader))

		return false

	glDeleteShader: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		shader = call\get_argument_by_name("shader")\query!
		@\_get_shader(shader).is_deleted = true
		@\_remove_shader(shader)

		return false

	glDeleteProgram: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		@\_get_program(program).is_deleted = true
		@\_remove_program(program)

		return false

	glBindAttribLocation: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		name = call\get_argument_by_name("name")\query!
		index = call\get_argument_by_name("index")\query!

		@\_get_program(program)\_set_attribute(name, index)

		return false

	glGetUniformLocation: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		program = call\get_argument_by_name("program")\query!
		name = call\get_argument_by_name("name")\query!
		location = call.result\query!
		@\_get_program(program)\_add_uniform(name, location)

	_set_uniform: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		location = call\get_argument_by_name("location")\query!
		@\get_current_program!\_set_uniform(location, call, trace)

		return true

	glUniform1f: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform2f: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform3f: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform4f: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform1i: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform2i: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform3i: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform4i: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform1ui: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform2ui: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform3ui: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform4ui: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform1fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform2fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform3fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform4fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform1iv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform2iv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform3iv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform4iv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform1uiv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform2uiv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform3uiv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniform4uiv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix2fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix3fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix4fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix2x3fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix2x4fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix3x2fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix3x4fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix4x2fv: (trace, call) =>
		return @\_set_uniform(trace, call)

	glUniformMatrix4x3fv: (trace, call) =>
		return @\_set_uniform(trace, call)

return ProgramFilter
