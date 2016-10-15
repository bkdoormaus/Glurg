// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_TEXTURE_TEXTURE_RESOURCE_HPP
#define LIBGLURG_RESOURCES_TEXTURE_TEXTURE_RESOURCE_HPP

#include "glurg/resources/resource.hpp"
#include "glurg/resources/texture/pixel.hpp"

namespace glurg
{
	class TextureResource
	{
	public:
		TextureResource(const TextureBlob* blob);
		~TextureBlob() = default;
	};
}

#endif
