// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MODEL_FRAME_HPP
#define LIBGLURG_RESOURCES_MESH_MODEL_FRAME_HPP

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace glurg
{
	class ModelFrame
	{
	public:
		struct Bone
		{
			glm::vec3 translation;
			glm::vec3 scale;
			glm::vec4 rotation;
		};

		ModelFrame() = default;
		ModelFrame(const ModelFrame& other) = default;
		~ModelFrame() = default;

		std::size_t get_num_bones(std::size_t index) const;

		Bone get_bone_at(std::size_t index) const;
		std::size_t set_bone_at(
			std::size_t index, const Bone& bone);
		std::size_t push_bone(const Bone& bone);
		
	private:
		std::vector<Bone> bones;
	};
}

#endif
