// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_OPENGL_HPP
#define LIBGLURG_RESOURCES_OPENGL_HPP

#include <string>
#include <GL/gl.h>

namespace glurg { namespace gl
{
	int to_pixel_component_description_swizzle_mode(GLenum value);
	int to_pixel_component_description_swizzle_mode(const std::string& value);

	int to_pixel_component_description_storage_type(GLenum value);
	int to_pixel_component_description_storage_type(const std::string& value);

	int to_texture_blob_texture_type(GLenum value);
	int to_texture_blob_texture_type(const std::string& value);
	GLenum enum_from_texture_blob_texture_type(int value);
	const char* string_from_texture_blob_texture_type(int value);

	int to_texture_blob_wrap_mode(GLenum value);
	int to_texture_blob_wrap_mode(const std::string& value);

	int to_texture_blob_zoom_filter(GLenum value);
	int to_texture_blob_zoom_filter(const std::string& value);
} }

#endif
