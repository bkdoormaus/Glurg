// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/trace/call.hpp"
#include "glurg/trace/callSignature.hpp"
#include "glurg/trace/values.hpp"

glurg::Call::Call(Index index, const CallSignature* signature)
{
	this->index = index;
	this->signature = signature;

	this->thread = 0;
	this->arguments.resize(signature->get_num_parameters());
}

glurg::Call::Index glurg::Call::get_call_index() const
{
	return this->index;
}

const glurg::CallSignature* glurg::Call::get_call_signature() const
{
	return this->signature;
}

glurg::Call::Thread glurg::Call::get_thread() const
{
	return this->thread;
}

void glurg::Call::set_thread(Thread thread)
{
	this->thread = thread;
}

const glurg::Value* glurg::Call::get_argument_at(ArgumentIndex index) const
{
	return this->arguments.at(index).get();
}

void glurg::Call::set_argument_at(
	ArgumentIndex index, const glurg::Value* value)
{
	this->arguments.at(index) = ValuePointer(value->clone());
}

const glurg::Value* glurg::Call::get_return_value() const
{
	return this->return_value.get();
}

void glurg::Call::set_return_value(const glurg::Value* value)
{
	this->return_value = ValuePointer(value->clone());
}
