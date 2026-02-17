#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

typedef struct buffer {
	int length;
	char bytes;
} user_Buffer;

LUALIB_API void luaL_newuserbuffer(lua_State *L, int length);