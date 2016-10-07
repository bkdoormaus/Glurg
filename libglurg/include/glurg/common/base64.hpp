// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_COMMON_BASE64_HPP
#define LIBGLURG_COMMON_BASE64_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace glurg
{
	void decode_base64(std::string input, std::vector<std::uint8_t>& blob);
}

#endif
