-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

moonscript = require "moonscript"
Log = require "glurg.common.Log"
Promise = require "glurg.common.Promise"
Trace = require "glurg.trace.Trace"

export glurg = {
	common: {
		Blob: require "glurg.common.Blob"
		Log: require "glurg.common.Log"
		Promise: require "glurg.common.Promise"
		Retrace: require "glurg.common.Retrace"
	},
	filters: {
		BufferFilter: require "glurg.filters.BufferFilter"
		Filter: require "glurg.filters.Filter"
		ProgramFilter: require "glurg.filters.ProgramFilter"
		VertexArrayFilter: require "glurg.filters.VertexArrayFilter"
	},
	heuristics: {
		GL3StaticModelHeuristic: require "glurg.heuristics.GL3StaticModelHeuristic"
	},
	resources: {
		Mesh: require "glurg.resources.Mesh"
		MeshBlob: require "glurg.resources.MeshBlob"
		MeshBlobBuilder: require "glurg.resources.MeshBlobBuilder"
		Model: require "glurg.resources.Model"
		Texture: require "glurg.resources.Texture"
		TextureBlob: require "glurg.resources.TextureBlob"
		TextureBlobBuilder: require "glurg.resources.TextureBlobBuilder"
	},
	trace: {
		Call: require "glurg.trace.Call"
		Event: require "glurg.trace.Event"
		Trace: require "glurg.trace.Trace"
		Value: require "glurg.trace.Value"
	}
}

SwapBuffers = {
	"glXSwapBuffers": true
}

class Heuristic
	new: (filename) =>
		success, result = pcall(require, "glurg.heuristics." .. filename)

		if success then
			@heuristic = result
		else
			success, result = pcall(moonscript.dofile, filename)
			if not success then
				error result, 0
			@heuristic = result

		@data = {}
		@lower_call_range = nil
		@upper_call_range = nil
		@lower_frame_range = nil
		@upper_frame_range = nil

	set_data: (key, value) =>
		@data[key] = value

	run: (filename) =>
		heuristic = self.heuristic(@data)
		filters = { heuristic\get_filters! }

		trace = Trace(filename)
		current_frame = 1
		while not trace.is_end_of_file
			event = trace\next_event!
			if event.type == 'leave'
				call_name = event.call.signature.name
				can_delete_call = true

				for i = 1, #filters
					func = filters[i]["gl"]
					if filters[i][call_name]
						func = filters[i][call_name]

					if func != nil then
						r = func(filters[i], trace, event.call)
						if r then
							can_delete_call = false

				is_lower_call = not @lower_call_range or event.call_index >= @lower_call_range
				is_upper_call = not @upper_call_range or @upper_call_range >= event.call_index
				is_lower_frame = not @lower_frame_range or current_frame >= @lower_frame_range
				is_upper_frame = not @upper_frame_range or @upper_frame_range >= current_frame

				if is_lower_call and is_lower_frame and is_upper_call and is_upper_frame then
					func = heuristic["gl"]
					if heuristic[call_name]
						func = heuristic[call_name]

					if func != nil then
						r = func(heuristic, trace, event.call)
						if r then
							can_delete_call = false

				if SwapBuffers[call_name]
					Log\info('low', "finished frame #{current_frame}")
					current_frame += 1

				if can_delete_call then
					trace\delete_call(event.call_index)

		if heuristic.dispose
			heuristic\dispose(trace)

return Heuristic
