#define lbuflib_c
#define LUA_LIB

#include <stdlib.h>
#include <string.h>

#include <stdint.h>
//#include <sys/endian.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "AS3/AS3.h"

#define BUFFER_SIZE_CAP 16777216

// typedefs for readability

/*
typedef unsigned char uint8_t;
typedef char int8_t;

typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned long uint32_t;
typedef long int32_t;

*/
typedef float f32_t;
typedef double f64_t;


typedef struct buffer {
	int length;
	char bytes[1];
} user_Buffer;

// todo: try something like (off == (off & 0x00FFFFFF))
static inline void range_check(lua_State *L, int offset, int len, size_t size){
	if (offset < 0 || offset >= len || (offset+(int)size) > len) {
		luaL_error(L, "buffer access out of range");
	}
}

static inline void buffer_check(lua_State *L, int index) {
	if ((lua_getmetatable(L,index) != 0) && lua_rawequal(L, -1, lua_upvalueindex(1))) {
		lua_pop(L,1);
	} else {
		luaL_argerror(L, index, "expected buffer");
	}
}

static int buf_readu8(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(uint8_t));
	lua_pushunsigned(L, (uint8_t) buf->bytes[offset]);
	return 1;
}

static int buf_writeu8(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(uint8_t));
	uint8_t value = (uint8_t) luaL_checkunsigned(L,3);
	buf->bytes[offset] = value;
	return 0;
}

static int buf_readi8(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(int8_t));
	lua_pushinteger(L, (int8_t) buf->bytes[offset]);
	return 1;
}

static int buf_writei8(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(int8_t));
	int8_t value = (int8_t) luaL_checkint(L,3);
	buf->bytes[offset] = value;
	return 0;
}

static int buf_readi16(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(int16_t));
	int16_t value;
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	valbytes[0] = bufbytes[offset+1];
	valbytes[1] = bufbytes[offset];
	lua_pushinteger(L, value);
	return 1;
}

static int buf_writei16(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(int16_t));
	int16_t value = luaL_checkint(L,3);
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	bufbytes[offset]   = valbytes[1]; // This is slightly faster??
	bufbytes[offset+1] = valbytes[0];
	return 0;
}

static int buf_readu16(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(uint16_t));
	uint16_t value;
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	valbytes[0] = bufbytes[offset+1];
	valbytes[1] = bufbytes[offset];
	lua_pushunsigned(L, value);
	return 1;
}

static int buf_writeu16(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(uint16_t));
	int16_t value = luaL_checkunsigned(L,3);
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	bufbytes[offset]   = valbytes[1];
	bufbytes[offset+1] = valbytes[0];
	return 0;
}

static int buf_readi32(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(int32_t));
	int32_t value;
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	valbytes[0] = bufbytes[offset + 3];
	valbytes[1] = bufbytes[offset + 2];
	valbytes[2] = bufbytes[offset + 1];
	valbytes[3] = bufbytes[offset];
	lua_pushinteger(L, value);
	return 1;
}

static int buf_writei32(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(int32_t));
	int32_t value = luaL_checkint(L,3);
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	bufbytes[offset]     = valbytes[3];
	bufbytes[offset + 1] = valbytes[2];
	bufbytes[offset + 2] = valbytes[1];
	bufbytes[offset + 3] = valbytes[0];
	return 0;
}

static int buf_readu32(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(uint32_t));
	uint32_t value;
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	valbytes[0] = bufbytes[offset + 3];
	valbytes[1] = bufbytes[offset + 2];
	valbytes[2] = bufbytes[offset + 1];
	valbytes[3] = bufbytes[offset];
	lua_pushunsigned(L, value);
	return 1;
}

static int buf_writeu32(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(uint32_t));
	uint32_t value = luaL_checkunsigned(L,3);
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	bufbytes[offset]     = valbytes[3];
	bufbytes[offset + 1] = valbytes[2];
	bufbytes[offset + 2] = valbytes[1];
	bufbytes[offset + 3] = valbytes[0];
	return 0;
}

static int buf_readf32(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(f32_t));
	float value;
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	valbytes[0] = bufbytes[offset + 3];
	valbytes[1] = bufbytes[offset + 2];
	valbytes[2] = bufbytes[offset + 1];
	valbytes[3] = bufbytes[offset];
	lua_pushnumber(L, (double) value);
	return 1;
}

static int buf_writef32(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(f32_t));
	float value = (float) luaL_checknumber(L,3);
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	bufbytes[offset]     = valbytes[3];
	bufbytes[offset + 1] = valbytes[2];
	bufbytes[offset + 2] = valbytes[1];
	bufbytes[offset + 3] = valbytes[0];
	return 0;
}

static int buf_readf64(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(f64_t));
	double value;
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	valbytes[0] = bufbytes[offset + 7];
	valbytes[1] = bufbytes[offset + 6];
	valbytes[2] = bufbytes[offset + 5];
	valbytes[3] = bufbytes[offset + 4];
	valbytes[4] = bufbytes[offset + 3];
	valbytes[5] = bufbytes[offset + 2];
	valbytes[6] = bufbytes[offset + 1];
	valbytes[7] = bufbytes[offset];
	lua_pushnumber(L, value);
	return 1;
}

static int buf_writef64(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	range_check(L, offset, buf->length, sizeof(f64_t));
	double value = luaL_checknumber(L,3);
	char* bufbytes = buf->bytes;
	char* valbytes = (char*)&value;
	bufbytes[offset]     = valbytes[7];
	bufbytes[offset + 1] = valbytes[6];
	bufbytes[offset + 2] = valbytes[5];
	bufbytes[offset + 3] = valbytes[4];
	bufbytes[offset + 4] = valbytes[3];
	bufbytes[offset + 5] = valbytes[2];
	bufbytes[offset + 6] = valbytes[1];
	bufbytes[offset + 7] = valbytes[0];
	return 0;
}

static int buf_readstring(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	int count = luaL_checkint(L, 3);
	luaL_argcheck(L, count >= 0, 3, "cannot read a string of negative length");
	range_check(L, offset, buf->length, count);
	lua_pushlstring(L, &(buf->bytes[offset]), (size_t) count);
	return 1;
}

static int buf_writestring(lua_State *L) {
	buffer_check(L,1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L, 2);
	unsigned int slen = 0;
	const char *str = luaL_checklstring(L, 3, &slen);
	unsigned int count = luaL_optunsigned(L, 4, slen);
	luaL_argcheck(L, count <= slen, 4, "count cannot be larger than string length");
	range_check(L, offset, buf->length, count);
	char* bufbytes = &(buf->bytes[offset]);
	memcpy(bufbytes, str, count); // Probably fine.
	return 0;
}

static int buf_fill(lua_State *L) {
	buffer_check(L, 1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	int offset = luaL_checkint(L,2);
	unsigned char value = (unsigned char) luaL_checkint(L,3);
	unsigned int count = luaL_optunsigned(L, 4, (buf->length) - offset);
	memset(&(buf->bytes[offset]), value, count);
	return 0;
}

static int buf_new(lua_State *L) {
	int length = luaL_checkint(L, 1);
	lua_pop(L, 1);
	luaL_argcheck(L, length > 0, 1, "buffer length must be greater than 0");
	if (length > BUFFER_SIZE_CAP) {
		luaL_error(L, "bad argument #1 to 'new' (buffer length cannot be greater than %d)", BUFFER_SIZE_CAP);
	}
	struct buffer *buf = lua_newuserdata(L, sizeof(int) + length);
	memset(&(buf->bytes), 0, length);
	buf->length = length;
	luaL_setmetatable(L, "buffer");
	return 1;
}

LUALIB_API void luaL_newuserbuffer(lua_State *L, int length) { // Creates a buffer and pushes it to the top of the stack.
	struct buffer *buf = lua_newuserdata(L, sizeof(int) + length);
	memset(&(buf->bytes), 0, length);
	buf->length = length;
	luaL_setmetatable(L, "buffer");
}

static const luaL_Reg buflib[] = {
	{"new",     buf_new},
	{"writeu8", buf_writeu8},
	{"readu8", buf_readu8},
	{"writei8", buf_writei8},
	{"readi8", buf_readi8},
	{"writeu16", buf_writeu16},
	{"readu16", buf_readu16},
	{"writei16", buf_writei16},
	{"readi16", buf_readi16},
	{"writeu32", buf_writeu32},
	{"readu32", buf_readu32},
	{"writei32", buf_writei32},
	{"readi32", buf_readi32},
	{"writef32", buf_writef32},
	{"readf32", buf_readf32},
	{"writef64", buf_writef64},
	{"readf64", buf_readf64},
	{"readstring", buf_readstring},
	{"writestring", buf_writestring},
	{"fill", buf_fill},
	{NULL, NULL}
};

LUAMOD_API int luaopen_buf (lua_State *L) {
	luaL_newlibtable(L,buflib); // put lib table at 2
	luaL_newmetatable(L, "buffer"); // metatable at index 3
	lua_pushvalue(L,2); // copy lib table to index 4
	lua_setfield(L, 3, "__index");
	lua_pushliteral(L, "buffer");
	lua_setfield(L, 3, "__type");
	luaL_setfuncs(L, buflib, 1);
	return 1;
}