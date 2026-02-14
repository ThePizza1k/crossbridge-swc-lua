Reference for LuaReference class
==================================
A LuaReference object holds a reference to a lua object that cannot be directly converted.
This includes: 
  - tables
  - functions
  - threads/coroutines
  - non-flash userdata without a metamethod for conversion to AS3.

Since ActionScript 3 has no finalizers, these references need to be managed manually.
  - This uses reference counting, where you increment or decrement the reference when doing certain things, such as storing it somewhere, or returning it to Lua.
  - Some reference counting is automatic: when Lua calls an AS3 function or setter, any LuaReference it provides starts at 1 reference, and decrements the reference count by one when done.
  - Any LuaReference returned to lua has its reference count decremented by 1 as well. (the reference passes into Lua, where it is 'destroyed')

LuaReference objects are referred to as luaReference.

Type checking is up to the user.
  - The integers for "expected type" are in LuaEnums
  - e.g `lua_ref.checkType(LuaEnums.LUA_TTABLE, 2, "awesomeMethod"); // type check a lua reference received by awesomeMethod as argument 2.`
  - `// if wrong type, throws error: "bad argument #2 to 'awesomeMethod' (expected table, got <type>)"`

### new LuaReference(luaState:int, referenceNum:int)
  - This constructor should not be called by outside code, unless it is directly interacting with the Lua C API.

### luaReference.type() : int
  - Returns an integer corresponding to the Lua Object's type.
  - e.g 5 (`LuaEnums.LUA_TTABLE`), or 7 (`LuaEnums.LUA_TUSERDATA`)

### luaReference.typename() : String
  - Returns a string with the type name of the Lua object.
  - e.g `"table"`, or `"userdata"`.
  - This does not respect `__type` currently. This can be worked around by checking the metatable for now.

todo: document everything else
