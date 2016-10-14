// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include "glurg/resources/renderState.hpp"
#include "glurg/resources/renderValue.hpp"

std::shared_ptr<glurg::RenderValue> glurg::RenderState::get(
	const std::string& name) const
{
	return std::make_shared<RenderValue>(this->data[name]);
}
