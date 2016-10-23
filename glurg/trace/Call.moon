-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"
Value = require "glurg.trace.Value"

class CallSignature
	new: (signature) =>
		Promise.keep("signature", Promise.IsUserdata(signature))

		@_signature = signature
		@id = signature.id
		@name = signature.name
		@num_parameters = signature.num_parameters

		@_parameters = {}
		for i = 0, signature.num_parameters - 1
			@_parameters[signature\get_parameter_at(i)] = i + 1

	get_parameter_index: (name) =>
		Promise.keep("name", Promise.IsString(name))
		Promise.keep("name", @_parameters[name] != nil)

		return @_parameters[name]

	get_parameter_name: (index) =>
		Promise.keep("index", Promise.IsNumber(index))
		Promise.keep("index", index >= 1)
		Promise.keep("index", index <= @num_parameters)

		return @_signature\get_parameter_at(index - 1)

class Call
	new: (call) =>
		Promise.keep("call", Promise.IsUserdata(call))

		@_call = call
		@index = call.index + 1
		@signature = CallSignature(call.signature)
		@result = Value(call.result) if call.result != nil

		@num_arguments = @signature.num_parameters

	get_argument_by_name: (argument_name) =>
		Promise.keep("argument_name", Promise.IsString(argument_name))

		index = @signature\get_parameter_index(argument_name)
		return Value(@_call\get_argument_at(index - 1))

	get_argument_by_index: (argument_index) =>
		Promise.keep("argument_index", Promise.IsNumber(argument_index))
		Promise.keep("argument_index", argument_index >= 1)
		Promise.keep("argument_index", argument_index <= @signature.num_parameters)
		
		return Value(@_call\get_argument_at(argument_index - 1))

return Call
