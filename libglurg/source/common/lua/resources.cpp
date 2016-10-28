// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <sol.hpp>
#include "glurg/common/lua.hpp"
#include "glurg/common/process.hpp"
#include "glurg/resources/renderState.hpp"
#include "glurg/resources/resourceBlob.hpp"
#include "glurg/resources/texture/textureResource.hpp"
#include "glurg/resources/texture/textureResourceBlob.hpp"
#include "glurg/resources/texture/textureResourceBlobBuilder.hpp"

void glurg::lua::export_resources(lua_State* L)
{
	sol::table glurg = sol::stack::get<sol::table>(L, -1);

	sol::table resources = glurg.create_named("resources");
	resources["is_compatible_texture_blob"] = &glurg::TextureResource::is_compatible_texture_blob;
	resources.new_usertype<glurg::PixelComponentDescription>(
		"PixelComponentDescription",
		sol::constructors<sol::types<>>(),
		"swizzle", &glurg::PixelComponentDescription::swizzle,
		"storage", &glurg::PixelComponentDescription::storage,
		"bit_size", &glurg::PixelComponentDescription::bit_size);
	resources.new_usertype<glurg::RenderState>(
		"RenderState",
		sol::constructors<sol::types<>>(),
		"get", &glurg::RenderState::get,
		"populate", &glurg::RenderState::populate<glurg::Process::Stream>);
	resources.new_usertype<glurg::Resource>(
		"Resource",
		"blob", sol::readonly_property(&glurg::Resource::get_blob));
	resources.new_usertype<glurg::TextureResource>(
		"Texture",
		sol::constructors<sol::types<const glurg::TextureResourceBlob*>>(),
		"fetch_pixel", &glurg::TextureResource::fetch_pixel,
		"decode_image", &glurg::TextureResource::decode_image,
		"blob", sol::readonly_property(&glurg::Resource::get_blob));
	resources.new_usertype<glurg::TextureResourceBlob>(
		"TextureBlob",
		"red_description", sol::readonly_property(&glurg::TextureResourceBlob::get_red_description),
		"green_description", sol::readonly_property(&glurg::TextureResourceBlob::get_green_description),
		"blue_description", sol::readonly_property(&glurg::TextureResourceBlob::get_blue_description),
		"alpha_description", sol::readonly_property(&glurg::TextureResourceBlob::get_alpha_description),
		"get_wrap_mode", &glurg::TextureResourceBlob::get_wrap_mode,
		"type", sol::readonly_property(&glurg::TextureResourceBlob::get_texture_type),
		"minification_filter", sol::readonly_property(&glurg::TextureResourceBlob::get_minification_filter),
		"magnification_filter", sol::readonly_property(&glurg::TextureResourceBlob::get_magnification_filter),
		"width", sol::readonly_property(&glurg::TextureResourceBlob::get_width),
		"height", sol::readonly_property(&glurg::TextureResourceBlob::get_height),
		"depth", sol::readonly_property(&glurg::TextureResourceBlob::get_depth),
		"mipmap_level", sol::readonly_property(&glurg::TextureResourceBlob::get_mipmap_level),
		"pixels", sol::readonly_property(&glurg::TextureResourceBlob::get_pixels),
		"pixels_size", sol::readonly_property(&glurg::TextureResourceBlob::get_pixels_size),
		"hash", sol::readonly_property(&glurg::TextureResourceBlob::get_hash),
		"data", sol::readonly_property(&glurg::TextureResourceBlob::get_data),
		"size", sol::readonly_property(&glurg::TextureResourceBlob::get_size));
	resources.new_usertype<glurg::TextureResourceBlobBuilder>(
		"TextureBlobBuilder",
		"set_red_description", &glurg::TextureResourceBlobBuilder::set_red_description,
		"set_green_description", &glurg::TextureResourceBlobBuilder::set_green_description,
		"set_blue_description", &glurg::TextureResourceBlobBuilder::set_blue_description,
		"set_alpha_description", &glurg::TextureResourceBlobBuilder::set_alpha_description,
		"set_wrap_mode", &glurg::TextureResourceBlobBuilder::set_wrap_mode,
		"set_type", &glurg::TextureResourceBlobBuilder::set_texture_type,
		"set_minification_filter", &glurg::TextureResourceBlobBuilder::set_minification_filter,
		"set_magnification_filter", &glurg::TextureResourceBlobBuilder::set_magnification_filter,
		"set_width", &glurg::TextureResourceBlobBuilder::set_width,
		"set_height", &glurg::TextureResourceBlobBuilder::set_height,
		"set_depth", &glurg::TextureResourceBlobBuilder::set_depth,
		"set_mipmap_level", &glurg::TextureResourceBlobBuilder::set_mipmap_level,
		"set_pixel_data", &glurg::TextureResourceBlobBuilder::set_pixel_data,
		"set_binding_point", &glurg::TextureResourceBlobBuilder::set_binding_point,
		"build", &glurg::TextureResourceBlobBuilder::build,
		"extract_from_state", &glurg::TextureResourceBlobBuilder::extract_from_state,
		"extract_framebuffer_from_state", &glurg::TextureResourceBlobBuilder::extract_framebuffer_from_state,
		"extract_from_call", &glurg::TextureResourceBlobBuilder::extract_from_call);
	sol::table texture = resources.create_named("texture");
	texture["swizzle_red"] = glurg::PixelComponentDescription::swizzle_red;
	texture["swizzle_green"] = glurg::PixelComponentDescription::swizzle_green;
	texture["swizzle_blue"] = glurg::PixelComponentDescription::swizzle_blue;
	texture["swizzle_alpha"] = glurg::PixelComponentDescription::swizzle_alpha;
	texture["swizzle_zero"] = glurg::PixelComponentDescription::swizzle_zero;
	texture["swizzle_one"] = glurg::PixelComponentDescription::swizzle_one;
	texture["storage_none"] = glurg::PixelComponentDescription::storage_none;
	texture["storage_disabled"] = glurg::PixelComponentDescription::storage_disabled;
	texture["storage_signed_normalized"] = glurg::PixelComponentDescription::storage_signed_normalized;
	texture["storage_unsigned_normalized"] = glurg::PixelComponentDescription::storage_unsigned_normalized;
	texture["storage_float"] = glurg::PixelComponentDescription::storage_float;
	texture["storage_signed_integral"] = glurg::PixelComponentDescription::storage_signed_integral;
	texture["storage_unsigned_integral"] = glurg::PixelComponentDescription::storage_unsigned_integral;
	texture["wrap_none"] = glurg::TextureResourceBlob::wrap_none;
	texture["wrap_border_clamp"] = glurg::TextureResourceBlob::wrap_border_clamp;
	texture["wrap_edge_clamp"] = glurg::TextureResourceBlob::wrap_edge_clamp;
	texture["wrap_repeat"] = glurg::TextureResourceBlob::wrap_repeat;
	texture["wrap_mirror_edge_clamp"] = glurg::TextureResourceBlob::wrap_mirror_edge_clamp;
	texture["wrap_mirror_repeat"] = glurg::TextureResourceBlob::wrap_mirror_repeat;
	texture["type_none"] = glurg::TextureResourceBlob::type_none;
	texture["type_1d"] = glurg::TextureResourceBlob::type_1d;
	texture["type_2d"] = glurg::TextureResourceBlob::type_2d;
	texture["type_3d"] = glurg::TextureResourceBlob::type_3d;
	texture["type_cube_map_positive_x"] = glurg::TextureResourceBlob::type_cube_map_positive_x;
	texture["type_cube_map_negative_x"] = glurg::TextureResourceBlob::type_cube_map_negative_x;
	texture["type_cube_map_positive_y"] = glurg::TextureResourceBlob::type_cube_map_positive_y;
	texture["type_cube_map_negative_y"] = glurg::TextureResourceBlob::type_cube_map_negative_y;
	texture["type_cube_map_positive_z"] = glurg::TextureResourceBlob::type_cube_map_positive_z;
	texture["type_cube_map_negative_z"] = glurg::TextureResourceBlob::type_cube_map_negative_z;
	texture["type_1d_array"] = glurg::TextureResourceBlob::type_1d_array;
	texture["type_2d_array"] = glurg::TextureResourceBlob::type_2d_array;
	texture["filter_none"] = glurg::TextureResourceBlob::filter_none;
	texture["filter_nearest"] = glurg::TextureResourceBlob::filter_nearest;
	texture["filter_linear"] = glurg::TextureResourceBlob::filter_linear;
	texture["filter_nearest_mipmap_nearest"] = glurg::TextureResourceBlob::filter_nearest_mipmap_nearest;
	texture["filter_linear_mipmap_nearest"] = glurg::TextureResourceBlob::filter_linear_mipmap_nearest;
	texture["filter_nearest_mipmap_linear"] = glurg::TextureResourceBlob::filter_nearest_mipmap_linear;
	texture["filter_linear_mipmap_linear"] = glurg::TextureResourceBlob::filter_linear_mipmap_linear;
}
