#define lrandlib_c
#define LUA_LIB

#include <stdlib.h>
#include <math.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

typedef unsigned long long int uint64_t;

struct splitmix64_state {
	uint64_t s;
};

uint64_t splitmix64(struct splitmix64_state *state) { // generator to seed xoshiro.
	uint64_t result = (state->s += 0x9E3779B97F4A7C15ULL);
	result = (result ^ (result >> 30)) * 0xBF58476D1CE4E5B9ULL;
	result = (result ^ (result >> 27)) * 0x94D049BB133111EBULL;
	return result ^ (result >> 31);
}

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

struct xoshiro256p_state {
  uint64_t s[4]; 
};

uint64_t xoshiro256p_next(struct xoshiro256p_state *state) { // this is a lot of 64-bit operations but they're cheap enough to not care.
  uint64_t *s = state->s;
	const uint64_t result = s[0] + s[3];

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 45);

	return result;
}

void xoshiro256p_seed(struct xoshiro256p_state *state, uint64_t seed) {
  struct splitmix64_state smstate = {seed};
  
  uint64_t *s = state->s;
  s[0] = splitmix64(&smstate);
  s[1] = splitmix64(&smstate);
  s[2] = splitmix64(&smstate);
  s[3] = splitmix64(&smstate);
}

static int rand_advance(lua_State *L) { // Accessed as closure.
  struct xoshiro256p_state *state = (struct xoshiro256p_state*) lua_touserdata(L,lua_upvalueindex(1));
  uint64_t result = ((xoshiro256p_next(state)>>12) | 0x3ff0000000000000ULL); // todo: figure out better way to do this
  double r = *((double*)&result) - 1.0;
  // below copied from math.random...
  switch (lua_gettop(L)) {  /* check number of arguments */
    case 0: {  /* no arguments */
      lua_pushnumber(L, r);  /* Number between 0 and 1 */
      break;
    }
    case 1: {  /* only upper limit */
      lua_Number u = luaL_checknumber(L, 1);
      luaL_argcheck(L, (lua_Number)1.0 <= u, 1, "interval is empty");
      lua_pushnumber(L, l_mathop(floor)(r*u) + (lua_Number)(1.0));  /* [1, u] */
      break;
    }
    case 2: {  /* lower and upper limits */
      lua_Number l = luaL_checknumber(L, 1);
      lua_Number u = luaL_checknumber(L, 2);
      luaL_argcheck(L, l <= u, 2, "interval is empty");
      lua_pushnumber(L, l_mathop(floor)(r*(u-l+1)) + l);  /* [l, u] */
      break;
    }
    default: return luaL_error(L, "wrong number of arguments");
  }
  return 1;
}

static int rand_seed(lua_State *L) { // Accessed as closure.
  int argc = lua_gettop(L);
  uint64_t seed = 0ULL;
  double input;
  switch(argc){
    case 0: // TODO: some stuff to generate a seed.
      break;
    case 1:
      input = luaL_checknumber(L,1);
      seed = *((uint64_t*)&input);
      break;
    default:
      return luaL_error(L, "wrong number of arguments (expected 0 or 1, got %d)", argc);
  }
  struct xoshiro256p_state *state = (struct xoshiro256p_state*) lua_touserdata(L,lua_upvalueindex(1));
  xoshiro256p_seed(state, seed);
  return 0;
}

static int rand_tostring(lua_State *L) { // also a closure.
  lua_pushfstring(L, "(RNG : %p)", lua_touserdata(L,lua_upvalueindex(1)));
  return 1;
}

static int rand_new(lua_State *L) {
  int argc = lua_gettop(L);
  uint64_t seed = 0ULL;
  double input;
  switch(argc){
    case 0: // TODO: some stuff to generate a seed.
      break;
    case 1:
      input = luaL_checknumber(L,1);
      seed = *((uint64_t*)&input);
      break;
    default:
      return luaL_error(L, "wrong number of arguments (expected 0 or 1, got %d)", argc);
  }
  struct xoshiro256p_state *state = (struct xoshiro256p_state*) lua_newuserdata(L,sizeof(struct xoshiro256p_state));
  xoshiro256p_seed(state, seed);
  lua_newtable(L); // metatable
  lua_newtable(L); // __index table
  lua_pushvalue(L,-3); // push udata
  lua_pushcclosure(L,rand_advance,1);
  lua_setfield(L,-2,"random"); // set in __index
  lua_pushvalue(L,-3); // push udata
  lua_pushcclosure(L,rand_seed,1);
  lua_setfield(L,-2,"randomseed"); // set in __index
  lua_setfield(L,-2,"__index"); // set __index table in metatable
  lua_pushvalue(L,-2);
  lua_pushcclosure(L,rand_tostring,1);
  lua_setfield(L,-2,"__tostring"); // set __tostring function in metatable.
  lua_pushliteral(L, "random");
  lua_setfield(L, -2, "__type");
  lua_setmetatable(L, -2);
  return 1;
}


static const luaL_Reg randlib[] = {
  {"new",   rand_new},
  {NULL, NULL}
};

LUAMOD_API int luaopen_rand (lua_State *L) {
  luaL_newlib(L, randlib);
  return 1;
}