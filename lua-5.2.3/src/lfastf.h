#include "lua.h"
#include "lauxlib.h"
#include "lobject.h"


// include this header if you want to define fast functions

// this is based on what luau does but i'm doing it cooler :sunglasses:

// these functions can not call user code, yield, fail, or reallocate stack.

// if inputs are bad, return -1 to fall back to a lua_CFunction.

//typedef int (*lua_FastCFunction) (lua_State *L, StkID res, int nresults, StkID args, int nparams);

LUA_API void lua_pushfastcfunction(lua_State *L, lua_FastCFunction fast, lua_CFunction fallback);

typedef struct luaL_fastReg {
	const char* name;
	lua_CFunction fallback;
	lua_FastCFunction fast;
} luaL_fastReg;

LUALIB_API void luaL_setFfuncs (lua_State *L, const luaL_fastReg *l) {
  luaL_checkversion(L);
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
	lua_pushfastcfunction(L, l->fast, l->fallback);
    lua_setfield(L, -(2), l->name);
  }
}

#define luaL_newFLib(L, l) (luaL_newlibtable(L,l), luaL_setFfuncs(L,l,0))