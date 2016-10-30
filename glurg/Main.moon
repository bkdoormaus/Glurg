-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

ParameterParser = require "glurg.common.ParameterParser"
Heuristic = require "glurg.heuristics.Heuristic"

State = { parameters: {}, options: {} }

State.set_trace = (value) ->
	State.parameters.trace = value

State.set_option = (value) ->
	pattern = "([%w%-]+)=(.+)"
	k, v = value\match(pattern)
	if k and v
		State.options[k] = v

State.set_heuristic = (value) ->
	State.parameters.heuristic = value

State.set_min_frame = (value) ->
	State.parameters.min_frame = tonumber(value)

State.set_max_frame = (value) ->
	State.parameters.max_frame = tonumber(value)

State.set_min_call = (value) ->
	State.parameters.min_call = tonumber(value)

State.set_max_call = (value) ->
	State.parameters.max_call = tonumber(value)


Parameters = ParameterParser("glurg")
do
	Parameters\add(
		"trace", "t", "apitrace file to process", true, State.set_trace)
	Parameters\add(
		"heuristic", "h", "heuristic to run", true, State.set_heuristic)
	Parameters\add(
		"min-frame", nil, "minimum frame to examine", true, State.set_min_frame)
	Parameters\add(
		"max-frame", nil, "maximum frame to examine", true, State.set_max_frame)
	Parameters\add(
		"min-call", nil, "minimum call to examine", true, State.set_min_call)
	Parameters\add(
		"max-call", nil, "maximum call to examine", true, State.set_max_call)
	Parameters\add(
		"option", "o", "set heuristic option in form KEY=VALUE", true, State.set_option)

main = (arguments) ->
	success, result = Parameters\parse(arguments)

	if not success
		if result
			io.stderr\write(result, "\n")
			os.exit(1)
		else
			os.exit(0)

	if State.parameters.trace == nil then
		io.stderr\write("no trace file provided; use --trace <input>\n")
		os.exit(1)

	if State.parameters.heuristic == nil then
		io.stderr\write("no heuristic to run; use --heuristic <name>\n")
		os.exit(1)

	success, result = pcall(Heuristic, State.parameters.heuristic)
	if not success then
		io.stderr\write(result, "\n")
		os.exit(1)

	heuristic = result
	heuristic.lower_call_range = State.parameters.min_call
	heuristic.upper_call_range = State.parameters.max_call
	heuristic.lower_frame_range = State.parameters.min_frame
	heuristic.upper_frame_range = State.parameters.max_frame

	for key, value in pairs(State.options)
		heuristic\set_data(key, value)

	heuristic\run(State.parameters.trace)

main({ ... })
