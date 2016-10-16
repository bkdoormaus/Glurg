// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCES_RESOURCE_HPP
#define LIBGLURG_RESOURCES_RESOURCE_HPP

namespace glurg
{
	class ResourceBlob;
	class ResourceFingerprint;

	class Resource
	{
	public:
		virtual ~Resource() = default;

		virtual const ResourceBlob* get_blob() const = 0;
		virtual const ResourceFingerprint& get_fingerprint() const = 0;
	};
}

#endif
