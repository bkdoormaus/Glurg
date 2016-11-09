-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

class Texture
	new: (name) =>
		@name = name
		@is_deleted = false
		@subs = {}
		@width = 1
		@height = 1

	_add_sub: (x, y, width, height, call, compressed) =>
		glurg.common.Log\info('low',
			"extracting texture (%d, %d; %d x %d; compressed = %s) from atlas %d at call %d...",
			x, y, width, height, tostring(compressed), @name, call.index)

		local texture
		do
			builder = glurg.resources.TextureBlobBuilder!
			do
				local extracted
				if compressed
					extracted = builder\extract_compressed_2d(call)
				else
					extracted = builder\extract_2d(call)

				if not extracted
					glurg.common.Log\warn('low', "couldn't extract texture")
					return false, nil

			blob = builder\build!
			if blob.is_data
				glurg.common.Log\warn('low', "couldn't extract texture; is data")
				return false, nil

			texture = glurg.resources.Texture(blob)

		glurg.common.Log\info('low', "successfully extracted texture")
		sub = { :x, :y, :width, :height, :texture }
		table.insert(@subs, sub)

		return true, sub

	get_sub: (x, y) =>
		for i = 1, #@subs
			sub = @subs[#@subs - i + 1]
			match_x = x >= sub.x and x < sub.x + sub.width
			match_y = y >= sub.y and y < sub.y + sub.height

			if match_x and match_y then
				return sub

		return nil

class TextureAtlasFilter extends glurg.filters.Filter
	new: (event) =>
		@_textures = {}
		@_texture_units = {}
		@active_texture_unit = 0
		@event = event or {}

	_get_texture: (name) =>
		if @_textures[name] == nil
			@_textures[name] = Texture(name)

		return @_textures[name]

	_set_texture_unit: (name) =>
		@_texture_units[@active_texture_unit] = name

	get_texture: (name) =>
		return @_textures[name]

	get_current_texture: (unit) =>
		return @\_get_texture(@_texture_units[unit])

	glActiveTexture: (trace, call) =>
		active_unit = call\get_argument_by_name("texture")\query!
		first_unit = active_unit.signature\get_value_by_name("GL_TEXTURE0")\query!

		@active_texture_unit = active_unit.value\query! - first_unit

		return false

	glBindTexture: (trace, call) =>
		target = call\get_argument_by_name("target")\query!
		if target.value_name == "GL_TEXTURE_2D"
			@\_set_texture_unit(call\get_argument_by_name("texture")\query!)

		return false

	_set_texture_size: (trace, call) =>
		target = call\get_argument_by_name("target")\query!
		level = call\get_argument_by_name("level")\query!
		if target.value_name == "GL_TEXTURE_2D" and level == 0
			texture = @get_current_texture(@active_texture_unit)
			texture.width = call\get_argument_by_name("width")\query!
			texture.height = call\get_argument_by_name("height")\query!
		return false

	glCompressedTexImage2D: (trace, call) =>
		return @\_set_texture_size(trace, call)

	glTexImage2D: (trace, call) =>
		return @\_set_texture_size(trace, call)

	glTexSubImage2D: (trace, call) =>
		x = call\get_argument_by_name("xoffset")\query!
		y = call\get_argument_by_name("yoffset")\query!
		width = call\get_argument_by_name("width")\query!
		height = call\get_argument_by_name("height")\query!

		success, sub = @\get_current_texture(@active_texture_unit)\_add_sub(
			x, y, width, height, call, false)
		if success and @event.on_subimage
			return @event\on_subimage(sub, call)

		return false

	glCompressedTexSubImage2D: (trace, call) =>
		x = call\get_argument_by_name("xoffset")\query!
		y = call\get_argument_by_name("yoffset")\query!
		width = call\get_argument_by_name("width")\query!
		height = call\get_argument_by_name("height")\query!

		success, sub = @\get_current_texture(@active_texture_unit)\_add_sub(
			x, y, width, height, call, true)
		if success and @event.on_compressed_subimage
			return @event\on_compressed_subimage(sub, call)

		return false

	glDeleteTextures: (trace, call) =>
		textures = call\get_argument_by_name("textures")\query!
		for i = 1, #textures
			if @_textures[textures[i]]
				@_textures[textures[i]].is_deleted = true
				@_textures[textures[i]] = nil

		return false

return TextureAtlasFilter
