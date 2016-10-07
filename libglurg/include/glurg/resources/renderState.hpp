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

		TextureResource* extract_texture(
			std::size_t index, int type, int level) const;

		template <typename Stream>
		void populate(Stream& stream);

	private:
		static std::string get_format_texture_data_key(
			std::size_t index, int type, int level);
		static std::string get_gl_texture_target(int type, bool collapse);
		static int get_pixel_components_enum(const std::string& value);

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
