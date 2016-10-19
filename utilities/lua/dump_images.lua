-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

local glurg = require "glurg"

local args = { ... }
if #args < 2 then
	io.stderr:write("dump_images <file.trace> <output_dir>\n")
	os.exit(1)
end

local stream = glurg.common.SnappyFileStream.new()
stream:open(args[1], glurg.common.file_mode_read)

local trace = glurg.trace.TraceFile.new()
if not trace:verify_is_compatible_version(stream) then
	io.stderr.write("trace file is not compatible");
	os.exit(1)
end

local textures = {}
local function extract_texture(call)
	local target = call:get_argument_at(0):to_enumeration()
	local GL_TEXTURE_2D =
		target.signature:get_value_by_name("GL_TEXTURE_2D"):to_integer()
	if target.value:to_integer() ~= GL_TEXTURE_2D then
		return false, "not texture 2D"
	end

	local textureBuilder = glurg.resources.TextureBlobBuilder.new()
	textureBuilder:set_texture_type(glurg.resources.texture.type_2d)
	textureBuilder:set_mipmap_level(call:get_argument_at(1):to_integer())
	textureBuilder:set_width(call:get_argument_at(3):to_integer())
	textureBuilder:set_height(call:get_argument_at(4):to_integer())
	textureBuilder:set_depth(1)
	textureBuilder:set_wrap_mode(0, glurg.resources.texture.wrap_edge_clamp)
	textureBuilder:set_wrap_mode(1, glurg.resources.texture.wrap_edge_clamp)
	textureBuilder:set_minification_filter(glurg.resources.texture.filter_nearest)
	textureBuilder:set_magnification_filter(glurg.resources.texture.filter_nearest)

	local blob = call:get_argument_at(8):to_blob()
	local data_format = call:get_argument_at(6):to_enumeration().value
	local data_type = call:get_argument_at(7):to_enumeration().value
	textureBuilder:extract_from_call_args(
		data_format:to_integer(), data_type:to_integer(), blob.data)

	local textureBlob = textureBuilder:build()
	if glurg.resources.is_compatible_texture_blob(textureBlob) then
		local textureResource = glurg.resources.Texture.new(textureBlob)
		local hash = textureBlob.hash:to_string()

		if not textures[hash] then
			local pixelBuffer = glurg.common.PixelDataBuffer.new()
			local outputPng = glurg.common.PixelDataBuffer.new()

			textureResource:decode_image(pixelBuffer)
			glurg.common.pixel_data_buffer_to_png(
				pixelBuffer, textureBlob.width, textureBlob.height, 4,
				outputPng)

			local fileStream = glurg.common.SimpleFileStream.new()
			local filename = args[2] .. "/" .. hash .. ".png"
			fileStream:open(filename, glurg.common.file_mode_write)
			fileStream:write(outputPng.data, outputPng.size)
			fileStream:close()

			print("saved:", filename)

			textures[hash] = true
		else
			return false, "already saved image"
		end
	else
		return false, "contains non-pixel data"
	end

	return true
end

while not stream.is_end_of_file do
	local event = glurg.trace.read_event(trace, stream)
	if event.type == glurg.trace.event_leave then
		local call = trace:get_call(event.call_index)
		if call.signature.name == "glTexImage2D" then
			local r, m = extract_texture(call)
			if r then
				print("extracted texture at call:", event.call_index)
			else
				print("couldn't extract texture at call:", event.call_index)
				print("reason:", m)
			end
		end
		trace:delete_call(call)
	end
end
