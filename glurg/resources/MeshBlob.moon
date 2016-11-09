-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Blob = require "glurg.common.Blob"
Promise = require "glurg.common.Promise"

marshal_vertex_format = (format) ->
	return switch format
		when glurg.resources.mesh.format_none
			'none'
		when glurg.resources.mesh.format_float
			'float'
		when glurg.resources.mesh.format_byte
			'byte'
		when glurg.resources.mesh.format_unsigned_byte
			'unsigned_byte'
		when glurg.resources.mesh.format_short
			'short'
		when glurg.resources.mesh.format_unsigned_short
			'unsigned_short'
		when glurg.resources.mesh.format_integer
			'integer'
		when glurg.resources.mesh.format_unsigned_integer
			'unsigned_integer'
		else
			error "unknown vertex description format"

marshal_vertex_normalization = (normalization) ->
	return switch normalization
		when glurg.resources.mesh.normalization_none
			'none'
		when glurg.resources.mesh.normalization_disabled
			'disabled'
		when glurg.resources.mesh.normalization_enabled
			'enabled'
		else
			error "unknown vertex normalization state"

class MeshBlob
	new: (blob) =>
		Promise.keep("blob", Promise.IsUserdata(blob))

		@_blob = blob
		@vertex_description =
			format: marshal_vertex_format(blob.vertex_description.format)
			num_components: blob.vertex_description.num_components
			normalization: marshal_vertex_normalization(blob.vertex_description.normalization)
			stride: blob.vertex_description.stride
			offset: blob.vertex_description.offset
		@vertex_data = Blob(blob.vertex_data, blob.vertex_data_size)
		@hash = blob.hash\to_string!

return MeshBlob
