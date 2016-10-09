// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_RENDER_VALUE_HPP
#define LIBGLURG_RESOURCES_RENDER_VALUE_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <json/json.h>

namespace glurg
{
	class RenderValue
	{
	public:
		typedef std::vector<std::uint8_t> Blob;

		RenderValue(const Json::Value& data);
		~RenderValue() = default;

		glm::vec2 to_vector2() const;
		glm::vec2 get_vector2(const std::string& name) const;

		glm::vec3 to_vector3() const;
		glm::vec3 get_vector3(const std::string& name) const;

		glm::vec4 to_vector4() const;
		glm::vec4 get_vector4(const std::string& name) const;

		int to_integer() const;
		int get_integer(const std::string& name) const;

		float to_single() const;
		float get_single(const std::string& name) const;

		std::string to_string() const;
		std::string get_string(const std::string& name) const;

		void extract_blob(Blob& blob) const;
		void extract_field_blob(const std::string& name, Blob& blob) const;

		RenderValue* get_field_by_name(const std::string& name) const;
		RenderValue* get_field_by_name(
			const std::string& name, std::size_t index) const;
		RenderValue* get_field_by_index(std::size_t index) const;

		bool has_field(const std::string& name) const;
		bool has_field(const std::string& name, std::size_t index) const;
		bool has_field(std::size_t index) const;

	private:
		Json::Value data;
	};
}

#endif
