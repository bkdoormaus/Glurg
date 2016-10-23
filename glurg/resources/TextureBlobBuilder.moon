-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"
TextureBlob = require "glurg.resources.TextureBlob"
Call = require "glurg.trace.Call"

marshal_texture_enumeration = (enumeration, value) ->
	Promise.keep("enumeration", Promise.IsString(enumeration))
	Promise.keep("value", Promise.IsString(value))


	v = glurg.resources.texture["#{enumeration}_#{value}"]
	if not v then
		error "enumeration not found"

	return v

marshal_description = (description) ->
	Promise.keep("description", Promise.IsTable(description))
	Promise.keep("description.swizzle", Promise.IsString(description.swizzle))
	Promise.keep("description.storage", Promise.IsString(description.storage))
	Promise.keep("description.bit_size", Promise.IsNumber(description.bit_size))

	result = glurg.resources.PixelComponentDescription.new()
	result.swizzle = marshal_texture_enumeration('swizzle', description.swizzle)
	result.storage = marshal_texture_enumeration('storage', description.storage)
	result.bit_size = description.bit_size

	return result

class TextureBlobBuilder
	new: =>
		@_builder = glurg.resources.TextureBlobBuilder.new!

		@\set_red_description({ swizzle: 'red', storage: 'disabled', bit_size: 0 })
		@\set_green_description({ swizzle: 'green', storage: 'disabled', bit_size: 0 })
		@\set_blue_description({ swizzle: 'blue', storage: 'disabled', bit_size: 0 })
		@\set_alpha_description({ swizzle: 'alpha', storage: 'disabled', bit_size: 0 })
		@\set_wrap_mode({ s: 'repeat', t: 'repeat', r: 'repeat' })
		@\set_type('2d')
		@\set_minification_filter('nearest')
		@\set_magnification_filter('nearest')
		@\set_width(1)
		@\set_height(1)
		@\set_depth(1)
		@\set_mipmap_level(0)

		@has_pixel_data = false

	set_red_description: (description) =>
		@_builder\set_red_description(marshal_description(description))

	set_green_description: (description) =>
		@_builder\set_green_description(marshal_description(description))

	set_blue_description: (description) =>
		@_builder\set_blue_description(marshal_description(description))

	set_alpha_description: (description) =>
		@_builder\set_alpha_description(marshal_description(description))

	set_wrap_mode: (wrap_mode) =>
		Promise.keep("wrap_mode", Promise.IsTable(wrap_mode))

		@_builder\set_wrap_mode(0,
			marshal_texture_enumeration('wrap', wrap_mode.s or 'repeat'))
		@_builder\set_wrap_mode(1,
			marshal_texture_enumeration('wrap', wrap_mode.t or 'repeat'))
		@_builder\set_wrap_mode(2,
			marshal_texture_enumeration('wrap', wrap_mode.r or 'repeat'))

	set_type: (type) =>
		@_builder\set_type(marshal_texture_enumeration('type', type))

	set_minification_filter: (filter) =>
		@_builder\set_minification_filter(marshal_texture_enumeration('filter', filter))

	set_magnification_filter: (filter) =>
		@_builder\set_magnification_filter(marshal_texture_enumeration('filter', filter))

	set_width: (width) =>
		Promise.keep("width", width >= 1)

		@_builder\set_width(width)

	set_height: (height) =>
		Promise.keep("height", height >= 1)

		@_builder\set_height(height)

	set_depth: (depth) =>
		@_builder\set_depth(depth)

	set_mipmap_level: (mipmap_level) =>
		@_builder\set_mipmap_level(mipmap_level)

	build: =>
		Promise.keep("@has_pixel_data", @has_pixel_data == true)

		return TextureBlob(@_builder\build!)

	extract_2d: (call) =>
		Promise.keep("call", Promise.IsClass(call, Call))

		@\set_width(call\get_argument_by_name("width")\query!)
		@\set_height(call\get_argument_by_name("height")\query!)
		@\set_depth(1)
		@\set_mipmap_level(call\get_argument_by_name("level"))

		pixel_data = call\get_argument_by_name("pixels")\query!
		if pixel_data == nil or pixel_data.length == 0
			return false

		pixel_format = call\get_argument_by_name("format")\query!.value\query!
		pixel_type = call\get_argument_by_name("type")\query!.value\query!

		@has_pixel_data = @_builder\extract_from_call(pixel_format,
			pixel_type,
			pixel_data.data)

		return @has_pixel_data

return TextureBlobBuilder
