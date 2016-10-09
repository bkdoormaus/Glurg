// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_RENDER_STATE_HPP
#define LIBGLURG_RESOURCES_RENDER_STATE_HPP

#include <memory>
#include <string>
#include <json/json.h>

namespace glurg
{
	class RenderValue;
	class TextureResource;

	class RenderState
	{
	public:
		RenderState() = default;
		~RenderState() = default;

		RenderValue* get_parameter(const std::string& name) const;
		RenderValue* get_parameter(
			const std::string& name, std::size_t index) const;

		template <typename Stream>
		void populate(Stream& stream);

	private:
		Json::Value data;
	};
}

template <typename Stream>
void glurg::RenderState::populate(Stream& stream)
{
	Json::CharReaderBuilder builder;
	std::string errors;
	
	Json::parseFromStream(builder, stream, &this->data, errors);
}

#endif
