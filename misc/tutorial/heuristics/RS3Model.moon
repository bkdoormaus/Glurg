TextureAtlasFilter = require "Heuristics/TextureAtlasFilter"

class RS3Model
	new: (model, texture) =>
		@model = model
		@texture = texture

class RS3ModelGroup
	new: (id) =>
		@id = id
		@models = {}

	add_model: (model, texture) =>
		table.insert(@models, RS3Model(model, texture))

	save_node: (directory) =>
		for i = 1, #@models
			filename = string.format("%s/%010d.%03d", directory, @id, i)
			@models[i].model\save_dae(filename .. ".dae")

			if @models[i].texture != nil
				@models[i].texture\save_png(filename .. ".png")

			glurg.common.Log\info('medium', "saved model #{@id}.#{i}")

class ExtractRS3Model extends glurg.heuristics.GL3StaticModelHeuristic
	new: (data) =>
		super(data)

		@texture_atlas_filter = TextureAtlasFilter!
		@attributes.atlas_binding = data["atlas"]
		@attributes.color_binding = data["color"]
		@atlas_sampler = data["atlas-sampler"]
		@atlas_meta = data["atlas-meta"]
	
	get_filters: =>
		return @texture_atlas_filter, super\get_filters!

	is_compatible: =>
		program = @program_filter\get_current_program!
		atlas = program\has_attribute(@attributes.atlas_binding)
		color = program\has_attribute(@attributes.color_binding)

		return super\is_compatible! and atlas

	examine_meshes: =>
		atlas_mesh = @\make_mesh(@attributes.atlas_binding)

		meshes = {}
		p = atlas_mesh\get_value(1) -- previous
		p_start = 0
		index = 1
		while index < atlas_mesh.num_values
			c = atlas_mesh\get_value(index + 1) -- current
			d = 1
			dx = math.abs(p.x - c.x) > d
			dy = math.abs(p.y - c.y) > d
			dz = math.abs(p.z - c.z) > d
			dw = math.abs(p.w - c.w) > d
			if dx or dy or dz or dw
				table.insert(meshes, { start: p_start, stop: index - 1, meta: p })
				p = c
				p_start = index
			index += 1
		table.insert(meshes, { start: p_start, stop: index - 1, meta: p})

		return meshes

	calculate_byte_offset_size: (attribute_index, offset_start, offset_end, call) =>
		-- See glurg.resources.MeshBlobBuilder.extract_attrib_call for specifics
		-- about how vertex descriptino is extracted
		vertex_array = @vertex_array_filter\get_current_vertex_array!
		description = vertex_array.descriptions[attribute_index]

		size = call\get_argument_by_name("size")\query!
		local num_components
		if size.value_name == 'GL_BGRA'
			num_components = 4
		else
			size = size.value\query!

		format = call\get_argument_by_name("type")\query!
		local component_size
		switch format.value_name
			when 'GL_FLOAT' then component_size = 4
			when 'GL_BYTE', 'GL_UNSIGNED_BYTE' then component_size = 1
			when 'GL_SHORT', 'GL_UNSIGNED_SHORT' then component_size = 2
			when 'GL_INT', 'GL_UNSIGNED_INT' then component_size = 4
			else error "unknown or unsupported data format"

		stride = call\get_argument_by_name("stride")\query!
		pointer = call\get_argument_by_name("pointer")\query!
		element_offset = 0
		local element_size
		if stride == 0
			element_size = num_components * component_size
			if type(pointer) == "number"
				element_offset = pointer
		else
			element_size = stride
			element_offset = 0

		byte_count = (offset_end - offset_start) * element_size
		byte_offset = offset_start * element_size + element_offset

		return byte_offset, byte_count

	make_sliced_mesh: (attribute_name, offset_start, offset_end) =>
		program = @program_filter\get_current_program!
		attribute_index = program\get_attribute(attribute_name)

		vertex_array = @vertex_array_filter\get_current_vertex_array!
		description = vertex_array.descriptions[attribute_index]

		buffer = @array_buffer_filter\get_buffer(description.array_buffer)
		buffer_offset, buffer_size = @\calculate_byte_offset_size(
			attribute_index, offset_start, offset_end, description.call)

		builder = glurg.resources.MeshBlobBuilder!
		extracted_description = builder\extract_attrib_call(description.call)
		extracted_buffer = builder\set_vertex_data(buffer.data\make_slice(buffer_offset, buffer_size))
		if extracted_description and extracted_buffer
			return glurg.resources.Mesh(builder\build!)

	make_sliced_model: (positions_mesh, normals_mesh, texture_coordinates_mesh, colors_mesh, offset_start, offset_end, call) =>
		model = glurg.resources.Model(positions_mesh)
		model.normals = normals_mesh
		model.texture_coordinates[1] = texture_coordinates_mesh
		model.colors[1] = colors_mesh

		index_format, index_size = switch call\get_argument_by_name("type")\query!.value_name
			when 'GL_UNSIGNED_BYTE' then 'unsigned_byte', 1
			when 'GL_UNSIGNED_SHORT' then 'unsigned_short', 2
			when 'GL_UNSIGNED_INT' then 'unsigned_integer', 4
			else error "unknown index type"

		vertex_array = @vertex_array_filter\get_current_vertex_array!
		element_array_buffer = @element_array_buffer_filter\get_buffer(vertex_array.element_array_buffer)

		index_data = element_array_buffer.data
		index_data_start = nil
		index_data_end = nil
		do
			offset = 0
			while offset < index_data.length
				value = switch index_format
					when 'unsigned_byte' then index_data\read_unsigned_byte(offset)
					when 'unsigned_short' then index_data\read_unsigned_short(offset)
					when 'unsigned_integer' then index_data\read_unsigned_integer(offset)
				if index_data_start == nil
					if value >= offset_start
						index_data_start = offset
				else
					if value <= offset_end
						index_data_end = offset + index_size
					else
						break

				offset += index_size

		glurg.common.Log\info('low',
			"index sub-region start: %d, index sub-region end: %d",
			index_data_start, index_data_end)

		index_data = index_data\get_mutable!
		do
			offset = index_data_start
			while offset < index_data_end
				switch index_format
					when 'unsigned_byte'
						value = index_data\read_unsigned_byte(offset) - offset_start
						index_data\write_unsigned_byte(offset, value)
					when 'unsigned_short'
						value = index_data\read_unsigned_short(offset) - offset_start
						index_data\write_unsigned_short(offset, value)
					when 'unsigned_integer'
						value = index_data\read_unsigned_integer(offset) - offset_start
						index_data\write_unsigned_integer(offset, value)

				offset += index_size

		index_length = (index_data_end - index_data_start)
		model\set_index_data(index_format, index_data\make_slice(index_data_start, index_length))

		return model

	get_texture_coords: (texture_meta, atlas_meta) =>
		atlas_slot = math.floor(texture_meta.x + 0.5)
		pixel_size = texture_meta.y
		atlas_slot_size = atlas_meta.x
		atlas_slot_pixel_size = atlas_meta.y
		atlas_pixel_size = atlas_meta.z

		y_slot = math.floor(atlas_slot / atlas_slot_size)
		x_slot = atlas_slot - atlas_slot_size * y_slot

		x = x_slot * 16
		y = y_slot * 16

		return x, y

	get_mesh_texture: (texture_meta) =>
		program = @program_filter\get_current_program!
		unit = program\get_uniform(@atlas_sampler)
		if unit
			atlas = @texture_atlas_filter\get_current_texture(unit)
			atlas_meta = program\get_uniform(@atlas_meta)

			if atlas and atlas_meta then
				glurg.common.Log\info('low', "getting texture from atlas %d...", atlas.name)
				x, y = @\get_texture_coords(texture_meta, atlas_meta)
				glurg.common.Log\info('low', "texture coordinates: (%f, %f)", x, y)
				sub = atlas\get_sub(x, y)
				if not sub then
					sub = atlas\get_sub(x - 32, y - 32)
				if sub then
					glurg.common.Log\info('low', "got texture")
				else
					glurg.common.Log\info('low', "didn't get texture")
				return sub

		return nil

	save_model: (call) =>
		if @\is_compatible!
			meshes = @\examine_meshes!
			glurg.common.Log\info('low', "examined meshes, got %d total", #meshes)
			group = RS3ModelGroup(call.index)
			for i = 1, #meshes
				local positions_mesh, normals_mesh, texture_coordinates_mesh
				do
					p = @attributes.position_binding
					n = @attributes.normal_binding
					t = @attributes.texture_coordinate_binding
					c = @attributes.color_binding
					start = meshes[i].start
					stop = meshes[i].stop
					glurg.common.Log\info('low', "mesh #{i} start: #{start}")
					glurg.common.Log\info('low', "mesh #{i} stop: #{stop}")
					glurg.common.Log\info('low', "aTextureMeta: (%f, %f, %f, %f)",
						meshes[i].meta.x, meshes[i].meta.y, meshes[i].meta.z, meshes[i].meta.w)
					positions_mesh = @\make_sliced_mesh(p, start, stop)
					normals_mesh = @\make_sliced_mesh(n, start, stop)
					texture_coordinates_mesh = @\make_sliced_mesh(t, start, stop)
					colors_mesh = @\make_sliced_mesh(c, start, stop)

				if positions_mesh and normals_mesh and texture_coordinates_mesh
					model = @\make_sliced_model(
						positions_mesh, normals_mesh, texture_coordinates_mesh, colors_mesh,
						meshes[i].start, meshes[i].stop, call)
					texture = @\get_mesh_texture(meshes[i].meta)
					glurg.common.Log\info('low', "extracted model")

					group\add_model(model, (texture or {}).texture)

			group\save_node(@output_directory)
		super\save_model(call)

return ExtractRS3Model
