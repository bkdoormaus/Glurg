// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MODEL_EXPORTER_HPP
#define LIBGLURG_RESOURCES_MESH_MODEL_EXPORTER_HPP

#include <cstdint>
#include <vector>

namespace glurg
{
	class Model;
	class ModelAnimation;

	class ModelExporter
	{
	public:
		ModelExporter();
		~ModelExporter() = default;

		const Model* get_model() const;
		void set_model(const Model* value);

		typedef std::vector<std::uint8_t> Buffer;
		void to_dae(Buffer& output_buffer);

	private:
		const Model* model;
	};
}

#endif
