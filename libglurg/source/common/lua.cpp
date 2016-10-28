// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright 2016 [bk]door.maus

#include <sol.hpp>
#include "glurg/common/lua.hpp"

extern "C" int luaopen_glurg(lua_State* L)
{
	sol::state_view state(L);
	sol::table glurg = state.create_table();
	sol::stack::push(L, glurg);

	glurg::lua::export_common(L);
	glurg::lua::export_resources(L);
	glurg::lua::export_trace(L);

	return 1;
}
