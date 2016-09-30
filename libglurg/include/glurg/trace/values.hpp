// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_VALUES_HPP
#define LIBGLURG_TRACE_VALUES_HPP

#include <cstdint>
#include <string>

namespace glurg
{
	class Array;
	class Bitmask;
	class Enumeration;
	class FileStream;
	class TraceFile;

	class Value
	{
	public:
		typedef Value* (* ReadFunction)(TraceFile&, FileStream&);
		typedef std::uint8_t Type;

		virtual Type get_type() const = 0;

		virtual bool to_boolean() const;

		virtual std::int32_t to_signed_integer() const;
		virtual std::uint32_t to_unsigned_integer() const;

		virtual float to_single() const;
		virtual double to_double() const;

		virtual std::string to_string() const;

		virtual std::uint8_t* to_pointer() const;
		virtual std::uint32_t to_handle() const;

		virtual Enumeration* to_enumeration() const;
		virtual Bitmask* to_bitmask() const;
		virtual Array* to_array() const;
	};
}

#endif
