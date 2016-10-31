-- This Source Code Form is subject to the terms of the Mozilla Public
-- License, v. 2.0. If a copy of the MPL was not distributed with this
-- file, You can obtain one at http://mozilla.org/MPL/2.0/.
--
-- Copyright 2016 [bk]door.maus

glurg = require "glurg"
Promise = require "glurg.common.Promise"
MeshBlob = require "glurg.resources.MeshBlob"

class Mesh
	new: (blob) =>
		Promise.keep("blob", Promise.IsClass(blob, MeshBlob))

		@_mesh = glurg.resources.Mesh.new(blob._blob)
		@num_values = @_mesh.num_values
		@blob = blob

	get_value: (index) =>
		Promise.keep("index", Promise.IsNumber(index))
		Promise.keep("index >= 1", index >= 1)
		Promise.keep("index <= @num_values", index <= @num_values)

		return @_mesh\get_value(index - 1)

return Mesh
