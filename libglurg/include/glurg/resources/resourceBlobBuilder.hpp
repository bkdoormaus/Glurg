// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_RESOURCE_BLOB_BUILDER_HPP
#define LIBGLURG_RESOURCES_RESOURCE_BLOB_BUILDER_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "glurg/resources/resourceBlob.hpp"

namespace glurg
{
	class RenderState;
	class ResourceBlobReadBuffer;

	template <typename B>
	class ResourceBlobBuilder
	{
	public:
		typedef typename std::enable_if<std::is_base_of<glurg::ResourceBlob, B>::value, B>::type Blob;
		virtual ~ResourceBlobBuilder() = default;

		virtual Blob* build() = 0;
		virtual void extract(const RenderState& state) = 0;
	};
}

#endif
