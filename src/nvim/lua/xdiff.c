#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "nvim/vim.h"
#include "nvim/xdiff/xdiff.h"

#ifndef MIN
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#endif // MIN

static int write_string(void *priv, mmbuffer_t *mb, int nbuf) {
  luaL_Buffer *buf = (luaL_Buffer*)priv;
  for (int i = 0; i < nbuf; i++) {
    long size = mb[i].size;
    for (long total = 0; total < size; total += LUAL_BUFFERSIZE) {
      int tocopy = MIN(size - total, LUAL_BUFFERSIZE);
      char* p = luaL_prepbuffer(buf);
      if (!p) {
        return -1;
      }
      memcpy(p, mb[i].ptr + total, tocopy);
      luaL_addsize(buf, tocopy);
    }
  }
  return 0;
}

static int write_func(void *priv, mmbuffer_t *mb, int nbuf) {
  lua_State *L = (lua_State*)priv;
  int fidx = lua_gettop(L);
  for (int i = 0; i < nbuf; i++) {
    lua_pushvalue(L, fidx);
    lua_pushlstring(L, mb[i].ptr, (size_t)mb[i].size);
    if (lua_pcall(L, 1, 0, 0) != 0) {
      return -1;
    }
  }
  lua_settop(L, fidx);
  return 0;
}

/* Reading routines */

static mmfile_t parse_argument(lua_State *L, int idx) {
  if (lua_type(L, idx) != LUA_TSTRING) {
    luaL_argerror(L, idx, "expecting string");
  }
  mmfile_t mf;
  mf.ptr = lua_tolstring(L, idx, &mf.size);
  return mf;
}

int lua_xdl_diff(lua_State *L) {
  mmfile_t m1 = parse_argument(L, 1);
  mmfile_t m2 = parse_argument(L, 2);

  xdemitconf_t cfg;
  xpparam_t    params;
  xdemitcb_t   ecb;

  memset(&cfg   , 0, sizeof(cfg));
  memset(&params, 0, sizeof(params));
  memset(&ecb   , 0, sizeof(ecb));

  luaL_Buffer b;

  if (lua_gettop(L) == 3 && lua_type(L, 3) == LUA_TFUNCTION) {
    ecb.priv = L;
    ecb.outf = write_func;
  } else {
    luaL_buffinit(L, &b);
    ecb.priv = &b;
    ecb.outf = write_string;
  }

  if (xdl_diff(&m1, &m2, &params, &cfg, &ecb) == -1) {
    return luaL_error(L, "Error while performing diff operation");
  } else if (lua_gettop(L) == 3) {
    return 0;
  } else {
    luaL_pushresult((luaL_Buffer*) ecb.priv);
    return 1;
  }
}
