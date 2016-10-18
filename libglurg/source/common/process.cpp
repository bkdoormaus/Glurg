// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <cstring>
#include <stdexcept>
#include <stdio.h>
#include "glurg/common/process.hpp"

glurg::Process::Process()
{
	this->handle = nullptr;
}

glurg::Process::~Process()
{
	close();
}

std::string glurg::Process::get_program_name() const
{
	return this->program_name;
}

void glurg::Process::set_program_name(std::string value)
{
	if (get_is_open())
	{
		throw std::runtime_error("cannot change program name during exeuction");
	}

	this->program_name = value;
}

std::size_t glurg::Process::get_num_program_arguments() const
{
	return this->program_arguments.size();
}

std::string glurg::Process::get_program_argument_at(std::size_t index) const
{
	return this->program_arguments.at(index);
}

void glurg::Process::set_program_argument_at(
	std::size_t index, const std::string& value)
{
	if (get_is_open())
	{
		throw std::runtime_error("cannot set program arguments during execution");
	}

	this->program_arguments.at(index) = value;
}

void glurg::Process::remove_program_argument_at(std::size_t index)
{
	if (get_is_open())
	{
		throw std::runtime_error("cannot set program arguments during execution");
	}

	this->program_arguments.erase(this->program_arguments.begin() + index);
}

void glurg::Process::push_program_argument(const std::string& value)
{
	if (get_is_open())
	{
		throw std::runtime_error("cannot set program arguments during execution");
	}

	this->program_arguments.push_back(value);
}

void glurg::Process::open(int mode)
{
	close();

	auto command = build_popen_string();
	if (mode == mode_read)
	{
		this->handle = popen(command.c_str(), "r");
	}
	else if (mode == mode_write)
	{
		this->handle = popen(command.c_str(), "w");
	}

	if (!this->handle)
	{
		throw std::runtime_error("couldn't execute process");
	}

	this->stream.open(fileno(this->handle), boost::iostreams::never_close_handle);
}

void glurg::Process::close()
{
	if (this->handle)
	{
		this->stream.close();
		pclose(this->handle);

		this->handle = nullptr;
	}
}

bool glurg::Process::get_is_open() const
{
	return this->handle != nullptr;
}

glurg::Process::Stream& glurg::Process::get_stream()
{
	return this->stream;
}

std::string glurg::Process::escape_string(const std::string& string)
{
	std::string result;

	// Logic adapted from http://archive.is/FRqnz
	for (auto& c: string)
	{
		if (c == '\'')
		{
			result.push_back('\'');
		}

		if (std::strchr(" \t\r\n\"\\'{}()<>;&|`$", c))
		{
			result.push_back('\\');
		}
		result.push_back(c);

		if (c == '\'')
		{
			result.push_back('\'');
		}
	}

	return result;
}

std::string glurg::Process::build_popen_string()
{
	std::string result = escape_string(this->program_name);
	for (auto& argument: this->program_arguments)
	{
		result.append(" ");
		result.append(escape_string(argument));
	}

	return result;
}
