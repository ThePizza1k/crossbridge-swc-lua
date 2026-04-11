#define lbuflib_c
#define LUA_LIB

#include <math.h> // for nan stuff

#include <stdlib.h>
#include <string.h>

#include <stdint.h>
//#include <sys/endian.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "AS3/AS3.h"

#define BUFFER_SIZE_CAP 16777216

#define FlashObj unsigned int
FlashObj* push_newflashref(lua_State *L); // Grab from flashlib.c
LUALIB_API void luaL_registerAS3Conversion(lua_State *L, const char *className); // Also grab from flashlib.c

/*
buffer library provides Lua with a fixed size, mutable block of memory.

This buffer library reads and writes in big endian order, to better correspond with Actionscript 3 ByteArrays.
(Note that this assumes little endian order, but this is fine because nobody gaf about big endian architectures and this is only ever running in flash anyways)

__as3 metamethod and the automatic conversion system means that Lua can provide buffers to AS3 defined functions and AS3 will see it as a ByteArray, and vice versa.

this *might* have memory safety issues. I haven't exactly verified it or anything. I am range checking everything at least.
*/

// For readability
typedef float f32_t;
typedef double f64_t;


typedef struct buffer {
	int length;
	char bytes[1];
} user_Buffer;


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
	double db_value = (double) value;
  if (isnan(db_value)) { // safety since user can generate signaling NAN.
		lua_pushnumber(L, NAN);
	} else {
		lua_pushnumber(L, db_value);
	}
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
	if (isnan(value)) { // safety since user can generate signaling NAN.
		lua_pushnumber(L, NAN);
	} else {
		lua_pushnumber(L, value);
	}
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
	luaL_argcheck(L, count < BUFFER_SIZE_CAP, 4, "attempted out of range buffer access");
	range_check(L, offset, buf->length, count);
	memset(&(buf->bytes[offset]), value, count);
	return 0;
}

static int buf_copy(lua_State *L) { // buffer.copy(target: buffer, targetOffset: number, source: buffer, sourceOffset: number?, count: number?): ()
	buffer_check(L, 1);
	struct buffer *targetBuffer = (struct buffer*) lua_touserdata(L, 1);
	int targetOffset = luaL_checkint(L, 2);
	buffer_check(L, 3);
	struct buffer *sourceBuffer = (struct buffer*) lua_touserdata(L, 3);
	int sourceOffset = luaL_optinteger(L, 4, 0);
	unsigned int count = luaL_optunsigned(L, 5, (unsigned int) sourceBuffer->length);
	luaL_argcheck(L, count < BUFFER_SIZE_CAP, 5, "attempted out of range buffer access");
	range_check(L, targetOffset, targetBuffer->length, count);
	range_check(L, sourceOffset, sourceBuffer->length, count);
	memmove(&(targetBuffer->bytes[targetOffset]), &(sourceBuffer->bytes[sourceOffset]), count);
	return 0;
}

static int buf_readbits(lua_State *L) { // Based off of luau's implementation.
	buffer_check(L, 1);
	struct buffer *targetBuffer = (struct buffer*) lua_touserdata(L, 1);
	int bit_offset = luaL_checkinteger(L, 2);
	int bit_count = luaL_checkinteger(L, 3);
	
	if (bit_offset < 0) {return luaL_argerror(L, 2, "attempted out of range buffer access");}
	if (bit_count > 48 || bit_count < 0) {return luaL_argerror(L, 3, "bit count is out of range of [0, 48]");}
	
	int start_byte = bit_offset >> 3; // right shift by 3 (to divide by 8)
	int end_byte = (bit_offset + bit_count + 7) >> 3; // This end_byte is not actually read that's why 7 is added.
	
	range_check(L, start_byte, targetBuffer->length, end_byte - start_byte);
	uint64_t data = 0;
	
	uint8_t *bytes = (uint8_t*) targetBuffer->bytes;
	
	int byte;
	for (byte = start_byte; byte < end_byte; byte++){
		data = (data << 8) + bytes[byte];
	}
	
	int subbyte_offset = (8-((bit_count + bit_offset) & 0x7)) & 0x7; // See reasoning under buf_writebits
	
	uint64_t mask = (1ULL << bit_count) - 1;
	
	lua_pushnumber(L, (double) ((data >> subbyte_offset) & mask));
	return 1;
}

static int buf_writebits(lua_State *L) { // Based off of luau's implementation.
	buffer_check(L, 1);
	
	struct buffer *targetBuffer = (struct buffer*) lua_touserdata(L, 1);
	int bit_offset = luaL_checkinteger(L, 2);
	int bit_count = luaL_checkinteger(L, 3);
	uint64_t value = (uint64_t) luaL_checknumber(L, 4);
	
	if (bit_offset < 0) {return luaL_argerror(L, 2, "attempted out of range buffer access");}
	if (bit_count > 48 || bit_count < 0) {return luaL_argerror(L, 3, "bit count is out of range of [0, 48]");}
	
	int start_byte = bit_offset >> 3; // left shift by 3 (to divide by 8)
	int end_byte = (bit_offset + bit_count + 7) >> 3; // This end_byte is not actually read that's why 7 is added.
	
	range_check(L, start_byte, targetBuffer->length, end_byte - start_byte);
	uint64_t data = 0;
	
	uint8_t *bytes = (uint8_t*) targetBuffer->bytes;
	
	int byte;
	for (byte = start_byte; byte < end_byte; byte++){ // read in data.
		data = (data << 8) + bytes[byte];
	}
	
	int subbyte_offset = (8-((bit_count + bit_offset) & 0x7)) & 0x7; // Reasoning for this line is below.
	
	uint64_t mask = ((1ULL << bit_count) - 1) << subbyte_offset;
	
	data = (data & ~mask) | ((value << subbyte_offset) & mask);
	
	lua_pushunsigned(L,mask);
	
	for (byte = end_byte - 1; byte >= start_byte; byte--){
		bytes[byte] = data & 0xff;
		data >>= 8;
	}
	
	return 1;
}

/* thinking

write 10 bits: 0b1001001001
offset 0:
|00000000|00000000|00000000|00000000|
|10010010|01000000|00000000|00000000|
Left shift by 6

size + off = 10, 10%8 = 2, (8 - 2) % 8 = 6

write 10 bits: 0b1001001001
offset 6:
|00000000|00000000|00000000|00000000|
|00000010|01001001|00000000|00000000|
Left shift by 0.

size + off = 16, 16%8 = 0, (8 - 0) % 8 = 0

*/

/* Awesome test code!
local buf = buffer.new(65536)

math.randomseed(61631)

local off = 6
local v = math.random(0,2^12 - 1)
print(v)
buf:writebits(off, 12, v)

print(buf:readbits(off, 12))

print()

local v2 = buf:readu32(0)
local str = ""
local str2 = ""
for i = 31, 0, -1 do
  str = str .. bit32.extract(v2, i, 1)
  if i < 12 then
    str2 = str2 .. bit32.extract(v, i, 1)
  end
end

print(str)
print(str2)
*/

static int buf_length(lua_State *L) {
	buffer_check(L, 1);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	lua_pushinteger(L, buf->length);
	return 1;
}

static int buf_tostring(lua_State *L) {
	struct buffer *buf = (struct buffer*) lua_touserdata(L, 1);
	lua_pushfstring(L, "buffer: %p", buf);
	return 1;
}

static int buf_toAS3(lua_State *L) { // called by lflashlib via metamethod.
	struct buffer *buf = (struct buffer*) lua_touserdata(L,1);
	char* bytes = buf->bytes;
	int length = buf->length;
	FlashObj* obj = push_newflashref(L);
	inline_as3(
		"import flash.utils.ByteArray;\n"
		"var ba:ByteArray = new ByteArray();\n"
		"CModule.readBytes(%1, %2, ba);\n"
		"ba.position = 0;\n"
		"__lua_objrefs[%0] = ba;\n"
		"__lua_objrefs[ba] = %0;\n"
		:
		: "r"(obj), "r"(bytes), "r"(length)
	);
	return 1;
}

LUALIB_API void luaL_newuserbuffer(lua_State *L, int length) { // Creates a buffer and pushes it to the top of the stack.
	struct buffer *buf = lua_newuserdata(L, sizeof(int) + length);
	memset(&(buf->bytes), 0, length);
	buf->length = length;
	luaL_setmetatable(L, "buffer");
}

static int buf_new(lua_State *L) {
	int length = luaL_checkint(L, 1);
	lua_pop(L, 1);
	luaL_argcheck(L, length > 0, 1, "buffer length must be greater than 0");
	if (length > BUFFER_SIZE_CAP) {
		luaL_error(L, "bad argument #1 to 'new' (buffer length cannot be greater than %d)", BUFFER_SIZE_CAP);
	}
	luaL_newuserbuffer(L, length);
	return 1;
}

static int buf_fromstring(lua_State *L) {
	size_t length;
	const char* str = luaL_checklstring (L, 1, &length);
	if (length > BUFFER_SIZE_CAP) {
		luaL_error(L, "bad argument #1 to 'new' (buffer length cannot be greater than %d)", BUFFER_SIZE_CAP);
	}
	luaL_newuserbuffer(L, length);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,-1);
	memcpy(&(buf->bytes[0]), str, length);
	return 1;
}

static int buf_fromAS3(lua_State *L) { // called by lflashlib for conversion to buffer.
	FlashObj* obj = (FlashObj*) lua_touserdata(L, 1);
	int length = -1;
	inline_as3( // read object and length
		"import flash.utils.ByteArray;\n"
		"var ba:ByteArray = __lua_objrefs[%1] as ByteArray;\n"
		"%0 = ba.length;\n"
		: "=r"(length)
		: "r"((int) obj)
	);
	lua_pop(L, 1);
	if (length > BUFFER_SIZE_CAP) {
		luaL_error(L, "error converting from AS3 (buffer length cannot be greater than %d)", BUFFER_SIZE_CAP);
	}
	luaL_newuserbuffer(L, length);
	struct buffer *buf = (struct buffer*) lua_touserdata(L,-1);
	inline_as3(
		"var temp:uint = ba.position;\n" // Preserve bytearray position.
		"ba.position = 0;\n"
		"CModule.writeBytes(%0, %1, ba);\n"
		"ba.position = temp;\n"
		:
		: "r"((int) &(buf->bytes[0])), "r"(length)
	);
	return 1;
}

static const luaL_Reg buflib[] = {
	{"new",                 buf_new},
	{"fromstring",   buf_fromstring},
	{"writeu8",         buf_writeu8},
	{"readu8",           buf_readu8},
	{"writei8",         buf_writei8},
	{"readi8",           buf_readi8},
	{"writeu16",       buf_writeu16},
	{"readu16",         buf_readu16},
	{"writei16",       buf_writei16},
	{"readi16",         buf_readi16},
	{"writeu32",       buf_writeu32},
	{"readu32",         buf_readu32},
	{"writei32",       buf_writei32},
	{"readi32",         buf_readi32},
	{"writef32",       buf_writef32},
	{"readf32",         buf_readf32},
	{"writef64",       buf_writef64},
	{"readf64",         buf_readf64},
	{"readstring",   buf_readstring},
	{"writestring", buf_writestring},
	{"readbits",       buf_readbits},
	{"writebits",     buf_writebits},
	{"fill",               buf_fill},
	{"copy",               buf_copy},
	{"len",              buf_length},
	{NULL, NULL}
};

LUAMOD_API int luaopen_buf (lua_State *L) {
	luaL_newlibtable(L,buflib); // put lib table at 2
	luaL_newmetatable(L, "buffer"); // metatable at index 3
	lua_pushvalue(L,2); // copy lib table to index 4
	lua_setfield(L, 3, "__index");
	lua_pushliteral(L, "buffer");
	lua_setfield(L, 3, "__type");
	lua_pushvalue(L, 3); // copy metamethod table.
	lua_pushcclosure(L, buf_length, 1);
	lua_setfield(L, 3, "__len");
	lua_pushcfunction(L, buf_toAS3);
	lua_setfield(L, 3, "__as3");
	lua_pushcfunction(L, buf_tostring);
	lua_setfield(L, 3, "__tostring");
	luaL_setfuncs(L, buflib, 1);

	lua_pushcfunction(L, buf_fromAS3);
	luaL_registerAS3Conversion(L, "flash.utils.ByteArray"); // buffer library is designed to correspond to AS3 ByteArray.

	return 1;
}
