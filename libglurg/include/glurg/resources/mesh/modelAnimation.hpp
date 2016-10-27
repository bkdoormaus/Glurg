// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_MESH_MODEL_ANIMATION_HPP
#define LIBGLURG_RESOURCES_MESH_MODEL_ANIMATION_HPP

#include <memory>
#include <vector>

namespace glurg
{
	class ModelFrame;

	class ModelAnimation
	{
	public:
		ModelAnimation(std::size_t num_bones);
		~ModelAnimation() = default;

		std::size_t get_num_frames() const;

		const ModelFrame* get_frame_at(std::size_t index) const;
		void set_frame_at(std::size_t index, const ModelFrame& frame);
		void push_frame(const ModelFrame& frame);

	private:
		typedef std::unique_ptr<ModelFrame> ModelFramePointer;
		std::vector<ModelFramePointer> frames;
	};
}

#endif
