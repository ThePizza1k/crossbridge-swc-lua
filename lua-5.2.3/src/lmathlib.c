/*
** $Id: lmathlib.c,v 1.83.1.1 2013/04/12 18:48:47 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/


#include <stdlib.h>
#include <math.h>

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "lfastf.h"


#undef PI
#define PI	((lua_Number)(3.1415926535897932384626433832795))
#define RADIANS_PER_DEGREE	((lua_Number)(PI/180.0))



static int math_abs (lua_State *L) {
  lua_pushnumber(L, l_mathop(fabs)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_abs(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		if (a1 < 0){
			setnvalue(res, -a1);
		} else {
			setnvalue(res, a1);
		}
    return 1;
	}
	return -1;
}

static int math_sin (lua_State *L) {
  lua_pushnumber(L, l_mathop(sin)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_sin(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, sin(a1));
    return 1;
	}
	return -1;
}

static int math_sinh (lua_State *L) {
  lua_pushnumber(L, l_mathop(sinh)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_sinh(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, sinh(a1));
    return 1;
	}
	return -1;
}

static int math_cos (lua_State *L) {
  lua_pushnumber(L, l_mathop(cos)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_cos(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, cos(a1));
    return 1;
	}
	return -1;
}

static int math_cosh (lua_State *L) {
  lua_pushnumber(L, l_mathop(cosh)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_cosh(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, cosh(a1));
    return 1;
	}
	return -1;
}

static int math_tan (lua_State *L) {
  lua_pushnumber(L, l_mathop(tan)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_tan(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, tan(a1));
    return 1;
	}
	return -1;
}

static int math_tanh (lua_State *L) {
  lua_pushnumber(L, l_mathop(tanh)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_tanh(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, tanh(a1));
    return 1;
	}
	return -1;
}

static int math_asin (lua_State *L) {
  lua_pushnumber(L, l_mathop(asin)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_asin(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, asin(a1));
    return 1;
	}
	return -1;
}

static int math_acos (lua_State *L) {
  lua_pushnumber(L, l_mathop(acos)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_acos(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, acos(a1));
    return 1;
	}
	return -1;
}

static int math_atan (lua_State *L) {
  lua_pushnumber(L, l_mathop(atan)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_atan(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, atan(a1));
    return 1;
	}
	return -1;
}

static int math_atan2 (lua_State *L) {
  lua_pushnumber(L, l_mathop(atan2)(luaL_checknumber(L, 1),
                                luaL_checknumber(L, 2)));
  return 1;
}

static int f_math_atan2(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 2 && nresults <= 1 && ttisnumber(args) && ttisnumber(args + 1)){
		double a1 = nvalue(args);
		double a2 = nvalue(args + 1); // how does StkId work? who knows.
		setnvalue(res, atan2(a1, a2))
		return 1;
	}
	return -1;
}

static int math_ceil (lua_State *L) {
  lua_pushnumber(L, l_mathop(ceil)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_ceil(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, ceil(a1));
    return 1;
	}
	return -1;
}

static int math_floor (lua_State *L) {
  lua_pushnumber(L, l_mathop(floor)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_floor(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, floor(a1));
    return 1;
	}
	return -1;
}

static int math_fmod (lua_State *L) {
  lua_pushnumber(L, l_mathop(fmod)(luaL_checknumber(L, 1),
                               luaL_checknumber(L, 2)));
  return 1;
}

static int f_math_fmod(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 2 && nresults <= 1 && ttisnumber(args) && ttisnumber(args + 1)){
		double a1 = nvalue(args);
		double a2 = nvalue(args + 1);
		setnvalue(res, fmod(a1, a2));
		return 1;
	}
	return -1;
}

static int math_modf (lua_State *L) {
  lua_Number ip;
  lua_Number fp = l_mathop(modf)(luaL_checknumber(L, 1), &ip);
  lua_pushnumber(L, ip);
  lua_pushnumber(L, fp);
  return 2;
}

static int f_math_modf(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 2 && ttisnumber(args)){
		double a1 = nvalue(args);
		double ip;
		setnvalue(res + 1, modf(a1, &ip));
		setnvalue(res, ip);
    return 2;
	}
	return -1;
}

static int math_sqrt (lua_State *L) {
  lua_pushnumber(L, l_mathop(sqrt)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_sqrt(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, sqrt(a1));
    return 1;
	}
	return -1;
}

static int math_pow (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  lua_Number y = luaL_checknumber(L, 2);
  lua_pushnumber(L, l_mathop(pow)(x, y));
  return 1;
}

static int f_math_pow(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 2 && nresults <= 1 && ttisnumber(args) && ttisnumber(args + 1)){
		double a1 = nvalue(args);
		double a2 = nvalue(args + 1);
		setnvalue(res, pow(a1, a2));
		return 1;
	}
	return -1;
}

static int math_log (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  lua_Number res;
  if (lua_isnoneornil(L, 2))
    res = l_mathop(log)(x);
  else {
    lua_Number base = luaL_checknumber(L, 2);
    if (base == (lua_Number)10.0) res = l_mathop(log10)(x);
    else res = l_mathop(log)(x)/l_mathop(log)(base);
  }
  lua_pushnumber(L, res);
  return 1;
}

static int f_math_log(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nresults <= 1 && ttisnumber(args)) { // first check
		double a1;
		switch(nparams){
			case 1:
				a1 = nvalue(args);
				setnvalue(res, log(a1));
				break;
			case 2:
				if (ttisnumber(args + 1)) {
					double base = nvalue(args + 1);
					if (base == 10.0) {
						setnvalue(res, log10(a1));
					} else {
						setnvalue(res, log(a1) / log(base));
					}
				} else {
					return -1;
				}
				break;
			default:
				return -1;
		}
		return 1;
	}
	return -1;
}

#if defined(LUA_COMPAT_LOG10)
static int math_log10 (lua_State *L) {
  lua_pushnumber(L, l_mathop(log10)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_log10(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, log10(a1));
		return 1;
	}
	return -1;
}
#endif

static int math_exp (lua_State *L) {
  lua_pushnumber(L, l_mathop(exp)(luaL_checknumber(L, 1)));
  return 1;
}

static int f_math_exp(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, exp(a1));
		return 1;
	}
	return -1;
}

static int math_deg (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)/RADIANS_PER_DEGREE);
  return 1;
}

static int f_math_deg(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, a1 / RADIANS_PER_DEGREE);
		return 1;
	}
	return -1;
}

static int math_rad (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)*RADIANS_PER_DEGREE);
  return 1;
}

static int f_math_rad(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double a1 = nvalue(args);
		setnvalue(res, a1 * RADIANS_PER_DEGREE);
		return 1;
	}
	return -1;
}

static int math_frexp (lua_State *L) {
  int e;
  lua_pushnumber(L, l_mathop(frexp)(luaL_checknumber(L, 1), &e));
  lua_pushinteger(L, e);
  return 2;
}

static int f_math_frexp(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 2 && ttisnumber(args)){
		double a1 = nvalue(args);
		int e;
		setnvalue(res, frexp(a1, &e));
		setnvalue(res + 1, (double)e);
    return 2;
	}
	return -1;
}

static int math_ldexp (lua_State *L) {
  lua_Number x = luaL_checknumber(L, 1);
  int ep = luaL_checkint(L, 2);
  lua_pushnumber(L, l_mathop(ldexp)(x, ep));
  return 1;
}

static int f_math_ldexp(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 2 && nresults <= 1 && ttisnumber(args) && ttisnumber(args + 1)){
		double a1 = nvalue(args);
		int ep = (int) nvalue(args + 1);
		setnvalue(res, ldexp(a1, ep));
		return 1;
	}
	return -1;
}

static int math_min (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmin = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d < dmin)
      dmin = d;
  }
  lua_pushnumber(L, dmin);
  return 1;
}

static int f_math_min(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 2 && nresults <= 1 && ttisnumber(args) && ttisnumber(args + 1)){
		double a1 = nvalue(args);
		double a2 = nvalue(args + 1);
		
		double min = (a2 < a1) ? a2 : a1;
		
		int i;
		for (i = 2; i < nparams; i++){
			if (!ttisnumber(args + i)){
				return -1;
			}
			
			double a = nvalue(args + i);
			
			if (a < min) {min = a;}
		}
		
		setnvalue(res, min);
		return 1;
	}
	return -1;
}


static int math_max (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmax = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d > dmax)
      dmax = d;
  }
  lua_pushnumber(L, dmax);
  return 1;
}

static int f_math_max(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 2 && nresults <= 1 && ttisnumber(args) && ttisnumber(args + 1)){
		double a1 = nvalue(args);
		double a2 = nvalue(args + 1);
		
		double max = (a2 > a1) ? a2 : a1;
		
		int i;
		for (i = 2; i < nparams; i++){
			if (!ttisnumber(args + i)){
				return -1;
			}
			
			double a = nvalue(args + i);
			
			if (a > max) {max = a;}
		}
		
		setnvalue(res, max);
		return 1;
	}
	return -1;
}


static int math_random (lua_State *L) { // not gonna make a fast version for this one
  /* the `%' avoids the (rare) case of r==1, and is needed also because on
     some systems (SunOS!) `rand()' may return a value larger than RAND_MAX */
  lua_Number r = (lua_Number)(rand()%RAND_MAX) / (lua_Number)RAND_MAX;
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

static int math_randomseed (lua_State *L) { // not gonna make a fast version for this one
  srand(luaL_checkunsigned(L, 1));
  (void)rand(); /* discard first value to avoid undesirable correlations */
  return 0;
}


static int math_clamp (lua_State *L) {
  lua_Number val = luaL_checknumber(L, 1);
  lua_Number min = luaL_checknumber(L, 2);
  lua_Number max = luaL_checknumber(L, 3);
  luaL_argcheck(L, min <= max, 2, "minimum must be less than or equal to maximum");
  if (val < min) {
    lua_pushnumber(L,min);
  } else if (val > max) {
    lua_pushnumber(L,max);
  } else {
    lua_pushnumber(L,val);
  }
  return 1;  
}

static int f_math_clamp(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 3 && nresults <= 1 && ttisnumber(args) && ttisnumber(args + 1) && ttisnumber(args + 2)){
		double val = nvalue(args);
		double min = nvalue(args + 1);
		double max = nvalue(args + 2);
		
		if (min > max) {return -1;}
		
		if (val < min){
			setnvalue(res, min);
		} else if (val > max) {
			setnvalue(res, max);
		} else {
			setnvalue(res, val);
		}
		
		return 1;
	}
	return -1;
}


static int math_sign (lua_State *L) {
  lua_Number val = luaL_checknumber(L, 1);
  if (val < 0) {
    lua_pushnumber(L, -1);
  } else if (val > 0) {
    lua_pushnumber(L, 1);
  } else {
    lua_pushnumber(L, 0);
  }
  return 1;
}

static int f_math_sign(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double val = nvalue(args);
		if (val < 0.0) {
			setnvalue(res, -1.0);
		} else if (val > 0) {
			setnvalue(res, 1.0);
		} else {
			setnvalue(res, 0.0);
		}
		return 1;
	}
	return -1;
}


static int math_round (lua_State *L) {
  lua_Number val = luaL_checknumber(L, 1);
  if (val < 0.0) {
    lua_pushnumber(L,l_mathop(ceil)(val - 0.5));
  } else {
    lua_pushnumber(L,l_mathop(floor)(val + 0.5));
  }
  return 1;
}

static int f_math_round(lua_State *L, StkId res, int nresults, StkId args, int nparams) {
	if (nparams >= 1 && nresults <= 1 && ttisnumber(args)){
		double val = nvalue(args);
		if (val < 0.0) {
			setnvalue(res, ceil(val - 0.5));
		} else {
			setnvalue(res, floor(val + 0.5));
		}
		return 1;
	}
	return -1;
}




static const luaL_fastReg f_mathlib[] = {
  {"abs",   {f_math_abs, math_abs}},
  {"acos",  {f_math_acos, math_acos}},
  {"asin",  {f_math_asin, math_asin}},
  {"atan2", {f_math_atan2, math_atan2}},
  {"atan",  {f_math_atan, math_atan}},
  {"ceil",  {f_math_ceil, math_ceil}},
  {"cosh",  {f_math_cosh, math_cosh}},
  {"cos",   {f_math_cos, math_cos}},
  {"deg",   {f_math_deg, math_deg}},
  {"exp",   {f_math_exp, math_exp}},
  {"floor", {f_math_floor, math_floor}},
  {"fmod",  {f_math_fmod, math_fmod}},
  {"frexp", {f_math_frexp, math_frexp}},
  {"ldexp", {f_math_ldexp, math_ldexp}},
#if defined(LUA_COMPAT_LOG10)
  {"log10", {f_math_log10, math_log10}},
#endif
  {"log",   {f_math_log, math_log}},
  {"max",   {f_math_max, math_max}},
  {"min",   {f_math_min, math_min}},
  {"modf",  {f_math_modf, math_modf}},
  {"pow",   {f_math_pow, math_pow}},
  {"rad",   {f_math_rad, math_rad}},
  {"sinh",  {f_math_sinh, math_sinh}},
  {"sin",   {f_math_sin, math_sin}},
  {"sqrt",  {f_math_sqrt, math_sqrt}},
  {"tanh",  {f_math_tanh, math_tanh}},
  {"tan",   {f_math_tan, math_tan}},
  {"clamp", {f_math_clamp, math_clamp}},
  {"sign",  {f_math_sign, math_sign}},
  {"round", {f_math_round, math_round}},
  {NULL, {NULL, NULL}}
};

static const luaL_Reg mathlib[] = {
	{"random",     math_random},
	{"randomseed", math_randomseed},
	{NULL, NULL}
};

/*
** Open math library
*/
LUAMOD_API int luaopen_math (lua_State *L) {
  luaL_newlib(L, mathlib);
  lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");
	luaL_setFfuncs(L, f_mathlib);
  return 1;
}

