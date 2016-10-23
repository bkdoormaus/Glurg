-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

Texture = require "glurg.resources.Texture"
TextureBlobBuilder = require "glurg.resources.TextureBlobBuilder"
Trace = require "glurg.trace.Trace"

Args = { ... }
if #Args < 2
	io.stderr\write("save_images_2d.moon <file.trace> <output-directory>\n")

	return 1

hashes = {}
save_image_from_call = (call) ->
	builder = TextureBlobBuilder!
	if not builder\extract_2d(call) then
		io.stdout\write("couldn't extract texture.\n")
		return false

	blob = builder\build!
	if blob.is_data then
		io.stdout\write("texture is data, not pixels.\n")
		return false

	if hashes[blob.hash] then
		io.stdout\write("texture already saved from previous call\n")
		return false

	texture = Texture(blob)
	texture\save_png(string.format("%s/%9d.png", Args[2], call.index))

	io.stdout\write("extracted texture.\n")
	hashes[blob.hash] = true
	
	return true

Funcs =
	"glTexImage2D": true
	"glTexSubImage2D": true

trace = Trace(Args[1])
while not trace.is_end_of_file
	event = trace\next_event!
	if event.type == 'leave' and Funcs[event.call.signature.name]
		io.stdout\write("extracting texture from call #{event.call.index}...\n")
		save_image_from_call(event.call)
		trace\delete_call(event.call_index)
