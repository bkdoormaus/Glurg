-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Promise = require "glurg.common.Promise"
Filter = require "glurg.filters.Filter"
Call = require "glurg.trace.Call"
Trace = require "glurg.trace.Trace"

class VertexArray
	new: (name) =>
		@name = name
		@descriptions = {}
		@element_array_buffer = 0

	_update_description: (trace, array_buffer, call) =>
		index = call\get_argument_by_name("index")\query!

		local description
		if @descriptions[index]
			description = @descriptions[index]
			trace\delete_call(description.call.index)
		else
			description = {}
			@descriptions[index] = description

		@descriptions[index].call = call
		@descriptions[index].array_buffer = array_buffer

	dispose: (trace) =>
		for _, description in pairs(@descriptions)
			trace\delete_call(description.call.index)


class VertexArrayFilter extends Filter
	new: =>
		@_vertex_arrays = {}
		@_current_array_buffer = 0
		@current_vertex_array = 0

	_get_vertex_array: (name) =>
		if @_vertex_arrays[name] == nil
			@_vertex_arrays[name] = VertexArray(name)
		return @_vertex_arrays[name]

	get_current_vertex_array: (name) =>
		return @\_get_vertex_array(@current_vertex_array)

	glBindBuffer: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		target = call\get_argument_by_name("target")\query!.value_name
		buffer = call\get_argument_by_name("buffer")\query!
		switch target
			when "GL_ELEMENT_ARRAY_BUFFER"
				if @current_vertex_array != 0
					vao = @\_get_vertex_array(@current_vertex_array)
					vao.element_array_buffer = buffer
			when "GL_ARRAY_BUFFER"
				@_current_array_buffer = buffer

		return true

	glBindVertexArray: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		@current_vertex_array = call\get_argument_by_name("array")\query!

		return false

	glVertexAttribPointer: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		vao = @\_get_vertex_array(@current_vertex_array)
		vao\_update_description(trace, @_current_array_buffer, call)

		return true

	glDeleteVertexArrays: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		arrays = call\get_argument_by_name("arrays")\query!
		for i = 1, #arrays
			if @_vertex_arrays[arrays[i]]
				@_vertex_arrays[arrays[i]].is_deleted = true
				@_vertex_arrays[arrays[i]]\dispose(trace)
				@_vertex_arrays[arrays[i]] = nil

	dispose: (trace) =>
		for _, vao in pairs(@_vertex_arrays)
			vao\dispose(trace)

return VertexArrayFilter
