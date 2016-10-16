// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_TRACE_PROCESS_HPP
#define LIBGLURG_TRACE_PROCESS_HPP

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

namespace glurg
{
	class Process
	{
	public:
		typedef boost::iostreams::stream<boost::iostreams::file_descriptor>
			Stream;

		enum
		{
			mode_read,
			mode_write
		};

		Process();
		~Process();

		std::string get_program_name() const;
		void set_program_name(std::string value);

		std::size_t get_num_program_arguments() const;
		std::string get_program_argument_at(std::size_t index) const;
		void set_program_argument_at(
			std::size_t index, const std::string& value);
		void remove_program_argument_at(std::size_t index);
		void push_program_argument(const std::string& value);

		void open(int mode);
		void close();
		bool is_open() const;

		Stream& get_stream();

	private:
		static std::string escape_string(const std::string& string);
		std::string build_popen_string();

		std::FILE* handle;
		Stream stream;

		std::string program_name;
		std::vector<std::string> program_arguments;
	};
}

#endif
