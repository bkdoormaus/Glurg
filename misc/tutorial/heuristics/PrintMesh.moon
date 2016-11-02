class PrintMesh extends glurg.heuristics.GL3StaticModelHeuristic
	new: (data) =>
		super(data)
		@attributes.target_binding = data["target"]

	save_model: (call) =>
		if super\save_model(call)
			program = @program_filter\get_current_program!
			if program\has_attribute(@attributes.target_binding)
				mesh = @\make_mesh(@attributes.target_binding)

				filename = string.format("%s/%010d.mesh", @output_directory, call.index)
				file, message = io.open(filename, "w")
				if not file
					glurg.common.Log\error("couldn't open '%s': %s", filename, message)
				else
					for i = 1, mesh.num_values
						value = mesh\get_value(i)
						m = string.format("%f, %f, %f, %f", value.x, value.y, value.z, value.w)
						file\write(m, "\n")

				glurg.common.Log\info('medium', "saved atlas vertex data: #{filename}")

return PrintMesh
