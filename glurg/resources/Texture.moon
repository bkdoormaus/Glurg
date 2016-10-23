-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"

class Texture
	new: (blob) =>
		Promise.keep("not blob.is_data", not blob.is_data)

		@_texture = glurg.resources.Texture.new(blob._blob)
		@blob = blob

	save_png: (filename) =>
		Promise.keep("filename", Promise.IsString(filename))

		decodedPixelBuffer = glurg.common.PixelDataBuffer.new()
		outputImagePixelBuffer = glurg.common.PixelDataBuffer.new()

		@_texture\decode_image(decodedPixelBuffer)
		glurg.common.pixel_data_buffer_to_png(decodedPixelBuffer, 
			@blob.width, @blob.height, 4,
			outputImagePixelBuffer)

		fileStream = glurg.common.SimpleFileStream.new()
		fileStream\open(filename, glurg.common.file_mode_write)
		fileStream\write(outputImagePixelBuffer.data,
			outputImagePixelBuffer.size)
		fileStream\close!

return Texture
