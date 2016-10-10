// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_COMMON_HASH_HPP
#define LIBGLURG_COMMON_HASH_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace glurg
{
	class Hash
	{
	public:
		// Size of hash, in bytes.
		static const std::size_t HASH_SIZE = 16;

		Hash();
		~Hash() = default;

		std::string to_string() const;
		const std::uint8_t* get_bytes() const;

		bool operator ==(const Hash& other) const;
		bool operator !=(const Hash& other) const;
		bool operator <(const Hash& other) const;

		static glurg::Hash hash(const std::uint8_t* data, std::size_t size);

	private:
		std::uint8_t data[HASH_SIZE];
	};
}

#endif
