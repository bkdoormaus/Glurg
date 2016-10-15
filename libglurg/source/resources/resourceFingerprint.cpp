// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cctype>
#include <cinttypes>
#include <cstdio>
#include "glurg/resources/resourceFingerprint.hpp"

glurg::ResourceFingerprint::ResourceFingerprint(
	std::uint8_t id, std::size_t num_features)
{
	this->id = id;
	this->features.resize(num_features);
}

std::string glurg::ResourceFingerprint::to_string() const
{
	std::string result;

	{
		char buffer[3] = { 0 };
		std::snprintf(buffer, 3, "%02x", this->id);
		result.append(buffer);
	}
	result.append(" ");

	for (std::size_t i = 0; i < this->features.size(); ++i)
	{
		char buffer[9] = { 0 };
		std::snprintf(
			buffer, sizeof(buffer), "%08" PRIx64, this->features[i].integer);

		result.append(buffer);
		result.append(" ");
	}

	return result;
}

bool glurg::ResourceFingerprint::from_string(
	const std::string& serialized_fingerprint)
{
	const char* s = serialized_fingerprint.c_str();

	std::uint32_t id;
	if (sscanf(s, "%x", &id) != 1)
	{
		return false;
	}

	std::vector<Feature> features;
	std::size_t index = 2;
	while (index < serialized_fingerprint.length())
	{
		if (std::isspace(s[index]))
		{
			++index;
			continue;
		}

		Feature feature;
		if (sscanf(s + index, "%" PRIx64, &feature.integer) != 1)
		{
			return false;
		}
		else
		{
			features.push_back(feature);

			while (!std::isspace(s[index]) &&
				index < serialized_fingerprint.size())
			{
				++index;
			}
		}
	}

	this->id = (std::int32_t)id;
	this->features = std::move(features);

	return true;
}

std::uint8_t glurg::ResourceFingerprint::get_id() const
{
	return this->id;
}

std::size_t glurg::ResourceFingerprint::get_num_features() const
{
	return this->features.size();
}

glurg::ResourceFingerprint::Feature glurg::ResourceFingerprint::get_feature(
	std::size_t index) const
{
	return this->features.at(index);
}

void glurg::ResourceFingerprint::set_feature(
	std::size_t index, const glurg::ResourceFingerprint::Feature& value)
{
	this->features.at(index) = value;
}

bool glurg::ResourceFingerprint::operator ==(
	const glurg::ResourceFingerprint& other) const
{
	if (other.id != this->id || other.features.size() != this->features.size())
		return false;

	for (std::size_t i = 0; i < this->features.size(); ++i)
	{
		if (other.features[i].integer != this->features[i].integer)
		{
			return false;
		}
	}

	return true;
}

bool glurg::ResourceFingerprint::operator !=(
	const glurg::ResourceFingerprint& other) const
{
	return !(other == *this);
}
