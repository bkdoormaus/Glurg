-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"

marshal_wrap_mode = (wrap_mode) ->
	return switch wrap_mode
		when glurg.resources.texture.wrap_none
			'none'
		when glurg.resources.texture.wrap_border_clamp
			'border_clamp'
		when glurg.resources.texture.wrap_edge_clamp
			'edge_clamp'
		when glurg.resources.texture.wrap_repeat
			'repeat'
		when glurg.resources.texture.wrap_mirror_edge_clamp
			'mirror_edge_clamp'
		when glurg.resources.texture.wrap_mirror_repeat
			'mirror_repeat'
		else
			error "unknown wrap mode"

marshal_texture_type = (texture_type) ->
	return switch texture_type
		when glurg.resources.texture.type_none
			'none'
		when glurg.resources.texture.type_1d
			'1d'
		when glurg.resources.texture.type_2d
			'2d'
		when glurg.resources.texture.type_3d
			'3d'
		when glurg.resources.texture.type_cube_map_positive_x
			'cube_map_positive_x'
		when glurg.resources.texture.type_cube_map_negative_x
			'cube_map_negative_x'
		when glurg.resources.texture.type_cube_map_positive_y
			'cube_map_positive_y'
		when glurg.resources.texture.type_cube_map_negative_y
			'cube_map_negative_y'
		when glurg.resources.texture.type_cube_map_positive_z
			'cube_map_positive_z'
		when glurg.resources.texture.type_cube_map_negative_z
			'cube_map_negative_z'
		when glurg.resources.texture.type_1d_array
			'1d_array'
		when glurg.resources.texture.type_2d_array
			'2d_array'
		else
			error "unknown texture type"

marshal_zoom_filter = (zoom_filter) ->
	return switch zoom_filter
		when glurg.resources.texture.filter_none
			'none'
		when glurg.resources.texture.filter_nearest
			'nearest'
		when glurg.resources.texture.filter_linear
			'linear'
		when glurg.resources.texture.filter_nearest_mipmap_nearest
			'nearest_mipmap_nearest'
		when glurg.resources.texture.filter_linear_mipmap_nearest
			'linear_mipmap_nearest'
		when glurg.resources.texture.filter_nearest_mipmap_linear
			'nearest_mipmap_linear'
		when glurg.resources.texture.filter_linear_mipmap_linear
			'linear_mipmap_linear'
		else
			error "unknown zoom filter"

marshal_pixel_description = (pixel_description) ->
	result =
		swizzle: switch pixel_description.swizzle
			when glurg.resources.texture.swizzle_red then 'red'
			when glurg.resources.texture.swizzle_green then 'green'
			when glurg.resources.texture.swizzle_blue then 'blue'
			when glurg.resources.texture.swizzle_alpha then 'alpha'
			when glurg.resources.texture.swizzle_zero then 'zero'
			when glurg.resources.texture.swizzle_one then 'one'
			else error "unknown swizzle mode"
		storage: switch pixel_description.storage
			when glurg.resources.texture.storage_none
				'none'
			when glurg.resources.texture.storage_disabled
				'disabled'
			when glurg.resources.texture.storage_signed_normalized
				'signed_normalized'
			when glurg.resources.texture.storage_unsigned_normalized
				'unsigned_normalized'
			when glurg.resources.texture.storage_float
				'float'
			when glurg.resources.texture.storage_signed_integral
				'signed_integral'
			when glurg.resources.texture.storage_unsigned_integral
				'unsigned_integral'
			else
				error "unknown swizzle mode"
		bit_size: pixel_description.bit_size
	
	return result

class TextureBlob
	new: (blob) =>
		Promise.keep("blob", Promise.IsUserdata(blob))

		@_blob = blob
		@description =
			red: marshal_pixel_description(blob.red_description)
			green: marshal_pixel_description(blob.green_description)
			blue: marshal_pixel_description(blob.blue_description)
			alpha: marshal_pixel_description(blob.alpha_description)
		@wrap_mode =
			s: marshal_wrap_mode(blob\get_wrap_mode(0))
			t: marshal_wrap_mode(blob\get_wrap_mode(1))
			r: marshal_wrap_mode(blob\get_wrap_mode(2))
		@type = marshal_texture_type(blob.type)
		@minification_filter = marshal_zoom_filter(blob.minification_filter)
		@magnification_filter = marshal_zoom_filter(blob.magnification_filter)
		@width = blob.width
		@height = blob.height
		@depth = blob.depth
		@mipmap_level = blob.mipmap_level
		@hash = blob.hash\to_string!
		@is_data = glurg.resources.is_compatible_texture_blob(blob) == false

return TextureBlob
