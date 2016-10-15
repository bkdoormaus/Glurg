// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_RESOURCE_RESOURCE_FINGERPRINT_HPP
#define LIBGLURG_RESOURCE_RESOURCE_FINGERPRINT_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace glurg
{
	class ResourceFingerprint
	{
	public:
		static const std::size_t FEATURE_SIZE = 8;
		union Feature
		{
			std::uint8_t bytes[FEATURE_SIZE];
			std::uint32_t pair[FEATURE_SIZE / sizeof(std::uint32_t)];
			std::uint64_t integer;
		};

		ResourceFingerprint() = default;
		ResourceFingerprint(std::uint8_t id, std::size_t num_features);
		~ResourceFingerprint() = default;

		std::string to_string() const;
		bool from_string(const std::string& serialized_fingerprint);

		std::uint8_t get_id() const;

		std::size_t get_num_features() const;
		Feature get_feature(std::size_t index) const;
		void set_feature(std::size_t index, const Feature& value);

		bool operator ==(const ResourceFingerprint& other) const;
		bool operator !=(const ResourceFingerprint& other) const;

	private:
		std::uint8_t id;
		std::vector<Feature> features;
	};
}

#endif
