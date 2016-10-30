-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Promise = require "glurg.common.Promise"

class ParameterParser
	new: (program_name) =>
		Promise.keep("program_name", Promise.IsString(program_name))

		@program_name = program_name
		@long_forms = {}
		@short_forms = {}
		@parameters = {}

	print_usage: =>
		io.stdout\write("#{@program_name} <options>\n")
		io.stdout\write("\n")
		io.stdout\write("parameters:\n")

		for i = 1, #@parameters
			io.stdout\write("\t", "--", @parameters[i].long_form)
			if @parameters[i].short_form
				io.stdout\write(",", " ", "-", @parameters[i].short_form)
			if @parameters[i].has_argument
				io.stdout\write(" ", "<ARGUMENT>")
			io.stdout\write("\n")

			if type(@parameters[i].description) == "table"
				io.stdout\write("\t\t", table.concat(@parameters[i].description, "\n"))
			else
				io.stdout\write("\t\t", @parameters[i].description, "\n")

	add: (long_form, short_form, description, has_argument, callback) =>
		Promise.keep("long_form", Promise.IsString(long_form))
		Promise.keep_any("short_form",
			short_form == nil, Promise.IsString(short_form))
		Promise.keep_any("description",
			Promise.IsString(description), Promise.IsTable(description))
		Promise.keep("has_argument", Promise.IsBoolean(has_argument))
		Promise.keep("callback", Promise.IsFunction(callback))

		parameter = { 
			:long_form, 
			:short_form, 
			:description,
			:has_argument,
			:callback
		}

		@long_forms[long_form] = parameter
		if short_form
			@short_forms[short_form] = parameter

		table.insert(@parameters, parameter)

	parse: (arguments) =>
		index = 1
		loose = {}

		while index <= #arguments
			argument = arguments[index]

			is_loose = false
			local parameter, parameter_argument
			if #argument > 2 and argument\sub(1, 2) == "--"
				parameter = @long_forms[argument\sub(3)]
			elseif #argument > 1 and argument\sub(1, 1) == "-"
				parameter = @short_forms[argument\sub(2)]
			else
				table.insert(loose, argument)
				is_loose = true

			if not is_loose then
				if not parameter then
					return false, string.format("invalid parameter '%s'", argument)

				if parameter.has_argument
					parameter_argument = arguments[index + 1]
					index += 1

				success, message = pcall(parameter.callback, parameter_argument)
				if not success then
					return false, message

			index += 1

		return true, unpack(loose)

return ParameterParser
