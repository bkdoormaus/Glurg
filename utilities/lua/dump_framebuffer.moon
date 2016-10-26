-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Trace = require "glurg.trace.Trace"
Retrace = require "glurg.common.Retrace"
Texture = require "glurg.resources.Texture"
TextureBlobBuilder = require "glurg.resources.TextureBlobBuilder"

Args = { ... }
if #Args < 3
	io.stderr\write("dump_framebuffer.moon <file.trace> <start-call> <end-call> <output-dir>\n")

	return 1

save_framebuffer_from_call = (retrace, index) ->
	render_state = retrace\next!

	depth_builder = TextureBlobBuilder!
	if not depth_builder\extract_framebuffer_attachment(render_state, 'depth')
		io.stdout\write("couldn't extract depth attachment\n")
	else
		blob = depth_builder\build!
		texture = Texture(blob)
		texture\save_png(string.format("%s/%010d_depth.png", Args[4], index))

	color_builder = TextureBlobBuilder!
	if not color_builder\extract_framebuffer_attachment(render_state, 'color', 1)
		io.stdout\write("couldn't extract color attachment\n")
	else
		blob = depth_builder\build!
		texture = Texture(blob)
		texture\save_png(string.format("%s/%010d_color.png", Args[4], index))

Funcs = {
	"glDrawArrays": true,
	"glDrawElements": true,
	"glDrawRangeElements": true
}

StartCall = tonumber(Args[2])
EndCall = tonumber(Args[3])

framebuffers = {}
do
	trace = Trace(Args[1])
	while not trace.is_end_of_file
		event = trace\next_event!
		if event.type == 'leave'
			if Funcs[event.call.signature.name] and event.call.index >= StartCall then
				io.stdout\write("queuing call #{event.call.index}\n")
				table.insert(framebuffers, event.call.index)
				if event.call.index > EndCall
					break
			trace\delete_call(event.call_index)
do
	retrace = Retrace(Args[1], framebuffers)
	retrace\start!
	for i = 1, #framebuffers
		io.stdout\write("saving framebuffer from call #{framebuffers[i]}\n")
		io.stdout\flush!
		save_framebuffer_from_call(retrace, framebuffers[i])
	retrace\stop!