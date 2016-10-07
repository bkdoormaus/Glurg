// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/textureResource.hpp"
#include "glurg/resources/renderState.hpp"
#include "glurg/resources/renderValue.hpp"

glurg::RenderValue* glurg::RenderState::get_parameter(
	const std::string& name) const
{
	return new RenderValue(this->data["parameters"][name]);
}

glurg::RenderValue* glurg::RenderState::get_parameter(
	const std::string& name, std::size_t index) const
{
	return new RenderValue(
		this->data["parameters"][name + std::to_string(index)]);
}

glurg::TextureResource* glurg::RenderState::extract_texture(
	std::size_t index, int type, int level) const
{
	std::string texture_key = get_format_texture_data_key(index, type, level);

	if (!this->data.isMember("textures") ||
		!this->data["textures"].isMember(texture_key))
	{
		return nullptr;
	}

	auto& texture = this->data["textures"][texture_key];

	int width = texture["__width__"].asInt();
	int height = texture["__height__"].asInt();
	int depth = texture["__depth__"].asInt();

	int pixel_components = get_pixel_components_enum(
		texture["__format__"].asString());

	auto textureInfo = std::make_shared(get_parameter("GL_TEXTURE", 0));
	textureInfo = std::make_shared(
		textureInfo->get_field_by_name(get_gl_texture_target(type, true)));
}

std::string glurg::RenderState::get_format_texture_data_key(
	std::size_t index, int type, int level)
{
	std::string result;

	result += "GL_TEXTURE";
	result += std::to_string(index);
	result += ", ";

	result += get_gl_texture_target(type, false);
	result += ", ";

	result += "level = ";
	result += std::to_string(level);

	return result;
}

int glurg::RenderState::get_pixel_components_enum(
	const std::string& value)
{
	if (value == "GL_RED")
	{
		return TextureResource::pixel_components_r;
	}
	else if (value == "GL_RG")
	{
		return TextureResource::pixel_components_rg;
	}
	else if (value == "GL_RGB")
	{
		return TextureResource::pixel_components_rgb;
	}
	else if (value == "GL_RGBA")
	{
		return TextureResource::pixel_components_rgba;
	}
	else
	{
		throw std::runtime_error("unknown OpenGL texture format");
	}
}

std::string glurg::RenderState::get_gl_texture_target(int type, bool collapse)
{
	switch (type)
	{
		case TextureResource::type_1d:
			return "GL_TEXTURE_1D";
		case TextureResource::type_2d:
			return "GL_TEXTURE_2D";
		case TextureResource::type_3d:
			return "GL_TEXTURE_3D";
		case TextureResource::type_1d_array:
			return "GL_TEXTURE_1D_ARRAY";
		case TextureResource::type_2d_array:
			return "GL_TEXTURE_2D_ARRAY";
	}

	if (collapse)
	{
		switch (type)
		{
			case TextureResource::type_cube_map_positive_x:
			case TextureResource::type_cube_map_negative_x:
			case TextureResource::type_cube_map_positive_y:
			case TextureResource::type_cube_map_negative_y:
			case TextureResource::type_cube_map_positive_z:
			case TextureResource::type_cube_map_negative_z:
				return "GL_TEXTURE_CUBE_MAP";
		}
	}
	else
	{
		switch (type)
		{
			case TextureResource::type_cube_map_positive_x:
				return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
			case TextureResource::type_cube_map_negative_x:
				return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
			case TextureResource::type_cube_map_positive_y:
				return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
			case TextureResource::type_cube_map_negative_y:
				return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
			case TextureResource::type_cube_map_positive_z:
				return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
			case TextureResource::type_cube_map_negative_z:
				return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
		}
	}

	throw std::runtime_error("invalid texture type");
}
