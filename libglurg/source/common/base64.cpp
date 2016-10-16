// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include "glurg/common/base64.hpp"

static const char base64_characters[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

const std::uint8_t get_char_index(char c)
{
	const char* begin = base64_characters;
	const char* end = base64_characters + sizeof(base64_characters) + 1;
	const char* result = std::find(begin, end, c);

	if (result == end)
		throw std::runtime_error("invalid base64 character");

	return std::distance(begin, result);
}

void remove_whitespace(std::string& string)
{
	string.erase(
		std::remove_if(string.begin(), string.end(), std::isspace),
		string.end());
}

std::size_t get_decoded_length(const std::string& string)
{
	std::size_t result = (string.length() * 3 / 4);
	auto e = string.find('=');
	if (e != std::string::npos)
	{
		result -= string.length() - e;
	}

	return result;
}

// Adapted from Wikipedia:
//   https://en.wikipedia.org/wiki/Base64#Sample_Implementation_in_Java
// (as of October 7 2016, in the event of edits)
void glurg::decode_base64(std::string input, std::vector<std::uint8_t>& blob)
{
	remove_whitespace(input);
	std::size_t decoded_length = get_decoded_length(input);

	std::size_t index = 0;
	std::uint8_t bytes[4];

	blob.reserve(decoded_length);
	for (std::size_t i = 0; i < input.length(); i += 4)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			bytes[j] = get_char_index(input[i + j]);
		}

		blob.push_back(bytes[0] << 2 | bytes[1] >> 4);
		if (bytes[2] < 64)
		{
			blob.push_back(bytes[1] << 4 | bytes[2] >> 2);

			if (bytes[3] < 64)
			{
				blob.push_back(bytes[2] << 6 | bytes[3]);
			}
		}
	}
}
