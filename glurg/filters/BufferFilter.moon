-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Blob = require "glurg.common.Blob"
Promise = require "glurg.common.Promise"
Filter = require "glurg.filters.Filter"
Call = require "glurg.trace.Call"
Trace = require "glurg.trace.Trace"

class Buffer
	new: (name) =>
		@name = name
		@data = false
		@is_deleted = false

	_buffer_data: (call) =>
		size = call\get_argument_by_name("size")\query!
		data = call\get_argument_by_name("data")\query!

		@data = Blob(size)
		if data != nil and data.length >= 1
			@data\copy(data, data.length)

	_buffer_sub_data: (call) =>
		if @data
			offset = call\get_argument_by_name("offset")\query!
			size = call\get_argument_by_name("size")\query!
			data = call\get_argument_by_name("data")\query!

			@data\copy(data, size, offset)

class BufferFilter extends Filter
	new: (target) =>
		@target = target
		@current_buffer = 0
		@_buffers = {}

	_get_buffer: (name) =>
		if @_buffers[name] == nil
			@_buffers[name] = Buffer(name)
		return @_buffers[name]

	get_current_buffer: =>
		return @\_get_buffer(@current_buffer)

	get_buffer: (name) =>
		Promise.keep("@_buffers[name] != nil", @_buffers[name] != nil)

		return @_buffers[name]

	glBindBuffer: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		target = call\get_argument_by_name("target")\query!.value_name
		if target == @target then
			@current_buffer = call\get_argument_by_name("buffer")\query!

		return false

	glBufferData: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		target = call\get_argument_by_name("target")\query!.value_name
		if target == @target then
			@\_get_buffer(@current_buffer)\_buffer_data(call)

		return false

	glBufferSubData: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		target = call\get_argument_by_name("target")\query!.value_name
		if target == @target then
			@\_get_buffer(@current_buffer)\_buffer_sub_data(call)

		return false

	glDeleteBuffers: (trace, call) =>
		Promise.keep("trace", Promise.IsClass(trace, Trace))
		Promise.keep("call", Promise.IsClass(call, Call))

		buffers = call\get_argument_by_name("buffers")\query!
		for i = 1, #buffers
			if @_buffers[buffers[i]]
				@_buffers[buffers[i]].is_deleted = true
				@_buffers[buffers[i]] = nil

		return false

return BufferFilter
