#ifndef NVIM_LUA_DIFF_H
#define NVIM_LUA_DIFF_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int luaopen_xdiff(lua_State *L);

#endif  // NVIM_LUA_DIFF_H
