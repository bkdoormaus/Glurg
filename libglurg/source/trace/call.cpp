// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/trace/call.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/values.hpp"

glurg::trace::Call::Call(Index index, const CallSignature* signature)
{
	this->index = index;
	this->signature = signature;

	this->thread = 0;
	this->arguments.resize(signature->get_num_parameters());
}

glurg::trace::Call::Index glurg::trace::Call::get_call_index() const
{
	return this->index;
}

const glurg::trace::CallSignature*
glurg::trace::Call::get_call_signature() const
{
	return this->signature;
}

glurg::trace::Call::Thread glurg::trace::Call::get_thread() const
{
	return this->thread;
}

void glurg::trace::Call::set_thread(Thread thread)
{
	this->thread = thread;
}

const glurg::trace::Value* glurg::trace::Call::get_argument_at(
	ArgumentIndex index) const
{
	return this->arguments.at(index).get();
}

void glurg::trace::Call::set_argument_at(
	ArgumentIndex index, const glurg::trace::Value* value)
{
	this->arguments.at(index) = value->clone();
}

const glurg::trace::Value* glurg::trace::Call::get_return_value() const
{
	return this->return_value.get();
}

void glurg::trace::Call::set_return_value(const glurg::trace::Value* value)
{
	this->return_value = value->clone();
}
