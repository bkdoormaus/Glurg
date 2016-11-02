class PrintUniforms extends glurg.heuristics.GL3StaticModelHeuristic
	new: (data) =>
		super(data)
		@program_filter = glurg.filters.ProgramFilter!

	get_filters: =>
		return @program_filter

	save_program_state: (call) =>
		program = @program_filter\get_current_program!
		uniforms = program\get_uniforms!
		
		filename = string.format("%s/%010d", @output_directory, call.index)
		do
			file, message = io.open(filename .. ".attribs", "w")
			if not file then
				glurg.common.Log\error("couldn't open '%s.attribs': %s", filename, message)
			for i = 0, 15
				attribute = program\get_attribute(i)
				if attribute
					file\write(string.format("%s = %d", attribute, i), "\n")
		do
			file, message = io.open(filename .. ".uniforms", "w")
			if not file
				glurg.common.Log\error("couldn't open '%s.uniforms': %s", filename, message)
			else
				for i = 1, #uniforms do
					uniform, uniform_type = program\get_uniform(uniforms[i].name)

					switch uniform_type
						when 'scalar'
							file\write(string.format(
								"#{uniforms[i].name}: %0.04f", uniform),  "\n")
						when 'vec2'
							file\write(string.format(
								"#{uniforms[i].name}: { x = %0.2f, y = %0.2f }",
								uniform.x, uniform.y), "\n")
						when 'vec3'
							file\write(string.format(
								"#{uniforms[i].name}: { x = %0.2f, y = %0.2f, z = %0.2f }"
								uniform.x, uniform.y, uniform.z), "\n")
						when 'vec4'
							file\write(string.format(
								"#{uniforms[i].name}: { x = %0.2f, y = %0.2f, z = %0.2f, w = %0.2f }",
								uniform.x, uniform.y, uniform.z, uniform.w), "\n")
						when 'scalar_array'
							for j = 1, #uniform
								file\write(string.format(
									"#{uniforms[i].name}[%d]: #{uniform[j]}", j - 1), "\n")
						when 'vec2_array'
							for j = 1, #uniform
								file\write(string.format(
									"#{uniforms[i].name}[%d]: { x = %0.2f, y = %0.2f }",
									j - 1, uniform[j].x, uniform[j].y), "\n")
						when 'vec3_array'
							for j = 1, #uniform
								file\write(string.format(
									"#{uniforms[i].name}[%d]: { x = %0.2f, y = %0.2f, z = %0.2f }",
									j - 1, uniform[j].x, uniform[j].y, uniform[j].z), "\n")
						when 'vec4_array'
							for j = 1, #uniform
								file\write(string.format(
									"#{uniforms[i].name}[%d]: { x = %0.2f, y = %0.2f, z = %0.2f, w = %0.2f }",
									j - 1, uniform[j].x, uniform[j].y, uniform[j].z, uniform[j].w), "\n")
						when 'mat4', 'mat3', 'mat2', 'mat2x3', 'mat2x4', 'mat3x2', 'mat3x4', 'mat4x2', 'mat4x3'
							values = {}
							for j = 1, #uniform
								r = {}
								for k = 1, #uniform[j]
									r[k] = string.format("%0.04f", uniform[j][k])
								values[j] = "[" .. table.concat(r, ", ") .. "]"
							values = table.concat(values, ", ")

							file\write("#{uniforms[i].name}: [#{values}]", "\n")
						when 'mat4_array', 'mat3_array', 'mat2_array', 'mat2x3_array', ', "\n")mat2x4_array', 'mat3x2_array', 'mat3x4_array', 'mat4x2_array', 'mat4x3_array'
							for m = 1, #uniform
								values = {}
								for j = 1, #uniform[m]
									r = {}
									for k = 1, #uniform[m][j]
										r[j] = string.format("%0.04f", uniform[m][j][k])
									values[j] = "[" .. table.concat(r, ", ") .. "]"
								values = "[" .. table.concat(values, ", ") .. "]"

								file\write(string.format(
									"#{uniforms[i].name}[%d]: [#{values}]", k - 1), "\n")
		do
			file, message = io.open(filename .. ".vertex.glsl", "w")
			if not file
				glurg.common.Log\error("couldn't open '%s.vertex.glsl': %s", filename, message)
			else
				vertex_source = program\get_shader_source("GL_VERTEX_SHADER")
				file\write(vertex_source)
		do
			file, message = io.open(filename .. ".fragment.glsl", "w")
			if not file
				glurg.common.Log\error("couldn't open '%s.fragment.glsl': %s", filename, message)
			else
				fragment_source = program\get_shader_source("GL_FRAGMENT_SHADER")
				file\write(fragment_source)

	glDrawElements: (trace, call) =>
		@\save_program_state(call)

	glDrawRangeElements: (trace, call) =>
		@\save_program_state(call)

return PrintUniforms
