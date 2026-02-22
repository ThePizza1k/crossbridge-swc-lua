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

### luaReference.clone() : LuaReference
  - Returns another LuaReference that refers to the same object.
  - idk why you'd need this but it's there

### luaReference.incRef() : void
  - Increments the reference count.
  - Use this when you somehow copy a reference, such as when storing it, or retrieving it and giving it to Lua.
  - e.g `arr[3] = ref; ref.incRef(); // increment since we stored it.`
  - e.g `var ref:LuaReference = arr[3]; ref.incRef(); return ref;`

### luaReference.decRef() : void
  - Decrements the reference count, freeing the reference if it reaches 0.
  - Use this when you destroy a reference somehow (and you're done with it), such as clearing where the LuaReference is stored.
  - This gets called automatically by Lua in two cases:
    - On LuaReferences it gave to AS3 for a setter/function call, after said setter/function call completed.
		- On a LuaReference returned by AS3, after it retrieves the value.

### luaReference.getField(key : Object) : Object
  - Equivalent to `ref[key]` in Lua.
  - Can trigger `__index` metamethods.
  - Key is converted to Lua equivalent (if possible), and the return value is converted to AS3 equivalent (if possible)
  - This can throw a Lua error, which AS3 should not attempt to catch. (It is still safe to catch regular AS3 errors)
    - If Lua panics, such as in case of an unprotected Lua error, it throws a regular AS3 error.
   
### luaReference.setField(key : Object, value : Object) : void
  - Equivalent to `ref[key] = value` in Lua.
  - Can trigger `__newindex` metamethods.
  - Key and value converted to Lua equivalent (if possible).
  - Same error behavior as getField.

### luaReference.call(...) : Array
  - Equivalent to `ref(...)` in Lua.
  - Does not trigger `__call` metamethods.
  - All args are converted to Lua equivalents, if possible.
  - Returns an array, where arr[0] is the error code. (See https://www.lua.org/manual/5.2/manual.html#lua_pcall for error codes)
    - If there is no error, arr[0] == 0, and remaining elements of the array are the functions return values.
    - If there is an error, arr[0] will be something else, and arr[1] will contain the error message.

### luaReference.execute(...) : Array
  - The same as `call`, but all return values from the function are thrown away.
  - Use this if you are not handling return values.

### luaReference.checkType(expected:int, arg:int = -1, fname:String = null) : void
  - Typechecks a LuaReference, throwing an error if it is wrong.
  - Arg number and function name can be provided for additional error information.
  - Use values in LuaEnum to specify type (e.g, LuaEnums.LUA_TBOOLEAN)

### luaReference.toString() : String
  - Returns a reasonable string conversion.
  - Calls `__tostring` metamethod if it exists.

### luaReference.getMetatable() : LuaReference
  - Returns the metatable of the given lua object, should it exist.
  - Returns null if there is no metatable.

### luaReference.setMetatable(tab : LuaReference) : void
  - Sets the metatable of the given Lua object to the given table.

todo: document everything else
