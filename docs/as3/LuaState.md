Reference for LuaState class
==================================

This is provided for interactions directly involving a lua state.
Primarily, this class handles interactions with globals, loading new functions, and creating new Lua objects.
Interactions with specific Lua Objects are handled in the LuaReference class.

LuaState objects are referred to as luaState.

### new LuaState()
  - Creates a new Lua State, and returns it.
  - The libraries are opened automatically.

### luaState.close()
  - Closes the Lua State.
  - This invalidates all LuaReferences connected to this lua state!

### luaState.setGlobal(name:String, obj:Object) : void
  - Sets the provided object as a global of the given name
  - Equivalent to `_G[name] = obj`
  - The object will be converted to a Lua value if possible.

### luaState.getGlobal(name:String) : Object
  - Gets the global of the given name
  - Equivalent to `_G[name]`
  - The lua value will be converted to an AS3 value if possible.

### luaState.callGlobal(name:String, ... args) : Array
  - Calls the global of the given name with the given args
  - Equivalent to `_G[name](...)`
  - Returns an array, where arr[0] is the error code.
  - If there is no error (arr[0] == LuaEnums.LUA_OK == 0), the rest of the array is for the return values.
  - If there is an error, arr[1] will contain the error message from Lua.

### luaState.doString(chunk:String) : void
  - Parses the provided string as Lua code and executes it.
  - The string takes no arguments and returns no values.
  - Only to be used for trusted code (to set up an environment or something)
  - Throws an error if loading the string or running it fails.

### luaState.loadString(chunk:String) : Array
  - Parses the provided string as Lua code and returns the function.
  - The return value is an array, in the same sense as luaState.callGlobal.
  - arr[0] is error code, arr[1] is the LuaReference for the function, or an error string.

### luaState.newTable(narr:int = 0, nrec:int = 0) : LuaReference
  - Creates a new table, pre-allocating memory for the given size.
  - narr is for the number of entries in a sequence (keys 1, 2, 3, ..., n)
  - nrec is for the number of other entries.

### luaState.tableFromPairs(pairs:Vector.<Array>) : LuaReference
  - Creates a new table, filling it from a Vector.<Array> that describes each pair.
  - Each entry of the vector is an array of size 2, where entry 0 is the key and entry 1 is the value.
  - e.g Vector.<Array> pairs = new \<Array>[["key", "value"], ["key2", "value2"], [true, false]]
