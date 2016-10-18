// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#ifndef LIBGLURG_COMMON_LUA_HPP
#define LIBGLURG_COMMON_LUA_HPP

extern "C"
{
	#include "lua.h"

	extern "C" int luaopen_glurg(lua_State* L);
}

#endif
