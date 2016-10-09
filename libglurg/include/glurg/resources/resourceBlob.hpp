// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_RESOURCE_BLOB_HPP
#define LIBGLURG_RESOURCES_RESOURCE_BLOB_HPP

#include <cstddef>
#include <cstdint>

namespace glurg
{
	struct Hash;

	class ResourceBlob
	{
	public:
		virtual ~Resource() = default;

		virtual const glurg::Hash& get_hash() const = 0;

		virtual const std::uint8_t* get_data() const = 0;
		virtual const std::size_t get_size() const = 0;
	};
}

#endif
