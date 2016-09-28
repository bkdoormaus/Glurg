// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstdio>
#include <cstdlib>
#include "glurg/common/hash.hpp"

glurg::Hash::Hash()
{
	std::memset(this->data, 0, HASH_SIZE);
}

std::string glurg::Hash::to_string() const
{
	std::string result;

	result.reserve(HASH_SIZE * 2);
	for (std::size_t i = 0; i < HASH_SIZE; ++i)
	{
		char buffer[3] = { 0 };
		std::snprintf(buffer, sizeof(buffer), "%02x", this->data[i]);

		result.append(buffer);
	}

	return result;
}

const std::uint8_t* glurg::Hash::get_bytes() const
{
	return this->data;
}

bool glurg::Hash::operator ==(const glurg::Hash &other) const
{
	return std::memcmp(this->data, other.data, HASH_SIZE) == 0;
}

bool glurg::Hash::operator !=(const glurg::Hash &other) const
{
	return !((*this) == other);
}

bool glurg::Hash::operator <(const glurg::Hash &other) const
{
	return std::memcmp(this->data, other.data, HASH_SIZE) < 0;
}
