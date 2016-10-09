// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/renderState.hpp"
#include "glurg/resources/renderValue.hpp"

glurg::RenderValue* glurg::RenderState::get_parameter(
	const std::string& name) const
{
	return new RenderValue(this->data["parameters"][name]);
}

glurg::RenderValue* glurg::RenderState::get_parameter(
	const std::string& name, std::size_t index) const
{
	return new RenderValue(
		this->data["parameters"][name + std::to_string(index)]);
}
