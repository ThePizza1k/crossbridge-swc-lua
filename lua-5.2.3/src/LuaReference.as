package crossbridge.lua{

	/*
		A LuaReference object holds a reference to a lua object that cannot be directly converted.
		This would include:
			tables
			functions
			threads/coroutines
			non-flash userdata without a metamethod for conversion to AS3.
		
		Things to keep in mind:
			Maintain the reference count, to ensure that lua values can be garbage collected when you are done with them.
			Some reference counting is automatic: when Lua calls an AS3 function or setter, any LuaReference it provides starts at 1 reference, and decrements the reference count by one when done.
			Any LuaReference returned to lua has its reference count decremented by 1 as well. (the reference passes into Lua, where it is 'destroyed')
			Type checking is up to you.
				The integers for "expected type" are in LuaEnums
				e.g lua_ref.checkType(LuaEnums.LUA_TTABLE, 2, "awesomeMethod"); // type check a lua reference received as argument 2.
				// if wrong type, throws error: "bad argument #2 to 'awesomeMethod' (expected table, got <type>)"
	*/

	public class LuaReference {
		
		public var L:int; // lua state pointer
		public var ref:int; // index in LUA_REGISTRYINDEX
		private var refCount:int; // number of references
		
		public function LuaReference(luaState:int, referenceNum:int) // should i cache type?
		{
			this.L = luaState;
			this.ref = referenceNum;
			this.refCount = 1;
		}
		
		private static function pushAS3(L:int, obj:Object):void
		{
			if (obj is Number){
				Lua.lua_pushnumber(L, obj as Number);
			} else if (obj is Boolean){
				Lua.lua_pushboolean(L, int(obj));
			} else if (obj is String) {
				Lua.lua_pushstring(L, obj as String);
			} else if (obj is Function) {
				// Just be an object for now.
				var fnptr:int = Lua.push_flashref(L);
				__lua_objrefs[fnptr] = obj;
			} else if (obj is LuaReference) {
				Lua.lua_rawgeti(L, LuaEnums.LUA_REGISTRYINDEX, (obj as LuaReference).ref);
			} else if (obj == null) {
				Lua.lua_pushnil(L);
			} else {
				if (__lua_objrefs[obj] == undefined) {
					var udptr:int = Lua.push_flashref(L);
					__lua_objrefs[udptr] = obj;
					__lua_objrefs[obj] = udptr;
					Lua.luaL_getsubtable(L, LuaEnums.LUA_REGISTRYINDEX, "flash_refs");
					Lua.lua_pushvalue(L,-2);
					Lua.lua_rawseti(L,-2, udptr); // Store a reference
					Lua.lua_pop(L,1);
				} else {
					Lua.luaL_getsubtable(L, LuaEnums.LUA_REGISTRYINDEX, "flash_refs");
					Lua.lua_rawgeti(L, -1, __lua_objrefs[obj]); // Retrieve a reference
					Lua.lua_replace(L, -2);
					if (Lua.lua_type(L, -1) == LuaEnums.LUA_TNIL) { // this can happen.
						Lua.lua_pop(L,1);
						var udptr2:int = Lua.push_flashref(L);
						__lua_objrefs[udptr2] = obj;
						__lua_objrefs[obj] = udptr2;
						Lua.luaL_getsubtable(L, LuaEnums.LUA_REGISTRYINDEX, "flash_refs");
						Lua.lua_pushvalue(L, -2);
						Lua.lua_rawseti(L, -2, udptr2);
						Lua.lua_pop(L,1);
					}
				}
				if (obj.constructor in __lua_typerefs[L]) {
					Lua.lua_rawgeti(L, LuaEnums.LUA_REGISTRYINDEX, __lua_typerefs[L][obj.constructor]);
					Lua.lua_pushvalue(L, -2);
					Lua.lua_pcallk(L, 1, 1, 0, 0, null);
					Lua.lua_replace(L, -2);
				}
			}
		}


		private static function getAS3(L:int, ind:int):Object
		{
			var t:int = Lua.lua_type(L,ind);
			switch(t) {
				case (LuaEnums.LUA_TNIL):
					return null;
				case (LuaEnums.LUA_TNUMBER):
					return Lua.lua_tonumberx(L,ind, 0);
				case (LuaEnums.LUA_TBOOLEAN):
					return Boolean(Lua.lua_toboolean(L,ind));
				case (LuaEnums.LUA_TSTRING):
					return Lua.lua_tolstring(L, ind, 0);
				case (LuaEnums.LUA_TTHREAD):
				case (LuaEnums.LUA_TTABLE):
				case (LuaEnums.LUA_TFUNCTION):
					if (ind < 0) {ind = Lua.lua_gettop(L) + ind + 1;}
					Lua.lua_pushvalue(L,ind);
					var ref:int = Lua.luaL_ref(L, LuaEnums.LUA_REGISTRYINDEX);
					return new LuaReference(L, ref);
				case (LuaEnums.LUA_TUSERDATA):
					var ptr:int = Lua.lua_touserdata(L,ind);
					if (__lua_objrefs[ptr] != null) {
						return __lua_objrefs[ptr];
					} else {
						if (ind < 0) {ind = Lua.lua_gettop(L) + ind + 1;}
						var converts:int = Lua.luaL_getmetafield(L, ind, "__as3");
						if (converts != 0) {
							Lua.lua_pushvalue(L, ind);
							Lua.lua_callk(L, 1, 1, 0, null); // lua_call does not exist because swig is stupid idk.
							var obj:Object = __lua_objrefs[Lua.lua_touserdata(L,ind)];
							Lua.lua_pop(L, 1);
							return obj;
						} else {
							Lua.lua_pushvalue(L,ind);
							var ref:int = Lua.luaL_ref(L, LuaEnums.LUA_REGISTRYINDEX);
							return new LuaReference(L, ref);
						}
					}
				case (LuaEnums.LUA_TLIGHTUSERDATA):
					return Lua.lua_touserdata(L,ind); // dubious return for this (returns as int), but we're not using light userdata anyways.
				default:
					throw new Error("Unexpected lua type of id: " + t);
					return null; // Wtf?
			}
		}
		
		private static function getDataAS3(L:int, ind:int):Object // Excludes objects that could never be converted from Lua.
		{
			var t:int = Lua.lua_type(L,ind);
			switch(t) {
				case (LuaEnums.LUA_TNIL):
					return null;
				case (LuaEnums.LUA_TNUMBER):
					return Lua.lua_tonumberx(L,ind, 0);
				case (LuaEnums.LUA_TBOOLEAN):
					return Boolean(Lua.lua_toboolean(L,ind));
				case (LuaEnums.LUA_TSTRING):
					return Lua.lua_tolstring(L, ind, 0);
				case (LuaEnums.LUA_TTABLE):
					if (ind < 0) {ind = Lua.lua_gettop(L) + ind + 1;}
					Lua.lua_pushvalue(L,ind);
					var ref:int = Lua.luaL_ref(L, LuaEnums.LUA_REGISTRYINDEX);
					return new LuaReference(L, ref);
				case (LuaEnums.LUA_TUSERDATA):
					var ptr:int = Lua.lua_touserdata(L,ind);
					if (__lua_objrefs[ptr] != null) {
						return __lua_objrefs[ptr];
					} else {
						if (ind < 0) {ind = Lua.lua_gettop(L) + ind + 1;}
						var converts:int = Lua.luaL_getmetafield(L, ind, "__as3");
						if (converts != 0) {
							Lua.lua_pushvalue(L, ind);
							Lua.lua_callk(L, 1, 1, 0, null); // lua_call does not exist because swig is stupid idk.
							var obj:Object = __lua_objrefs[Lua.lua_touserdata(L,ind)];
							Lua.lua_pop(L, 1);
							return obj;
						} else { // Doesn't convert.
							return null;
						}
					}
				default:
					return null;
			}
		}
		
		/*
			Returns an integer corresponding to the type of the Lua object.
			e.g 5 (== LuaEnums.LUA_TTABLE), or 7 (== LuaEnums.LUA_TUSERDATA)
		*/
		public function type() : int
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var t:int = Lua.lua_type(this.L,-1);
			Lua.lua_pop(this.L,1);
			return t;
		}
		
		/*
			Returns a string with the type name of the Lua object.
			e.g "table", or "userdata".
		*/
		public function typename() : String
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var t:int = Lua.lua_type(this.L,-1);
			if (t != LuaEnums.LUA_TUSERDATA) {
				Lua.lua_pop(this.L,1);
				return LuaEnums.LUA_TYPEMAP[t];
			} else {
				var hasField = Lua.luaL_getmetafield(L, -1, "__type");
				if (hasField == 0) {
					Lua.lua_pop(this.L, 1);
					return LuaEnums.LUA_TYPEMAP[t];
				} else {
					var tName:String = Lua.lua_tolstring(L, -1);
					Lua.lua_pop(this.L, 2);
					return tName;
				}
			}
		}
	
		/*
			Returns a new LuaReference that references the same object.
			This may be useful in some cases idk.
		*/
		public function clone() : LuaReference
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var newRef:int = Lua.luaL_ref(this.L, LuaEnums.LUA_REGISTRYINDEX);
			return new LuaReference(this.L, newRef);
		}

		/*
			Increments the reference count.
			Use this when you copy a reference somehow (such as storing it somehow, or retrieving it and giving it to Lua)
			e.g
				arr[3] = ref;
				ref.incRef(); // increment since we stored it.

			e.g 2
				var ref:LuaReference = arr[3];
				ref.incRef(); // Increment, since we retrieved it and sent it somewhere.
				return ref;
		*/
		public function incRef() : void
		{
			this.refCount += 1;
		}

		/*
			Decrements the reference count, freeing it if the reference count is 0.
			Use this when you destroy a reference somehow, such as clearing where the LuaReference is stored.
			This gets called automatically by Lua in two cases:
				1. On LuaReferences it gave to AS3 for a setter/function call, after said setter/function call completed.
				2. On a LuaReference returned by AS3, after it retrieves the value.

			e.g
				var ref:LuaReference = arr[3];
				delete arr[3];
				ref.decRef(); // We destroyed the array's reference.
				// Cannot use the reference past here. (It may have been freed)

		*/
		public function decRef() : void
		{
			this.refCount -= 1;
			if (this.refCount <= 0) {
				Lua.luaL_unref(this.L, LuaEnums.LUA_REGISTRYINDEX, this.ref);
				this.ref = LuaEnums.LUA_NOREF; // Ensure that, if lifetimes are incorrect, it will just convert to nil.
			}
		}

		/*
			Equivalent to ref[key] in Lua.
			Can trigger __index metamethod.
			Recommended to only use in functions called from Lua, in case __index throws an error.
		
			key: Converted to Lua equivalent to index the object
			returns: Equivalent AS3 value to ref[key].
		*/
		public function getField(key : Object) : Object
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			pushAS3(this.L, key);
			Lua.lua_gettable(this.L,-2); // can trigger __index
			var ret:Object = getAS3(this.L, -1);
			Lua.lua_pop(this.L,2); // pop object and value.
			return ret;
		}
		
		/*
			Equivalent to ref[key] = value in Lua.
			Can trigger __newindex metamethod.
			Recommended to only use in functions called from Lua, in case __newindex throws an error.

			key: Converted to Lua-side equivalent to index the object
			value: Converted to Lua-side equivalent to set in the object.
		*/
		public function setField(key : Object, value : Object) : void
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			pushAS3(this.L,key);
			pushAS3(this.L,value);
			Lua.lua_settable(this.L,-3); // not entirely sure what happens if __newindex throws an error
			Lua.lua_pop(this.L,1);
		}
		
		/*
			Equivalent to ref(...) in Lua.
			Can trigger __call metamethod.
			
			args are converted to lua-side equivalents.
      
			Returns: An array, where arr[0] is the error code. (See https://www.lua.org/manual/5.2/manual.html#lua_pcall for error codes)
			If there is no error, arr[0] is 0, and the rest of the array is the as3-equivalent to the values returned by the function.
			Otherwise, arr[0] will be something else, and arr[1] will be the error message.
		*/
		public function call(... args) : Array
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var i:int = 0;
			for (i = 0; i < args.length; i++){
				pushAS3(this.L,args[i]);
			}
			var errCode:int = Lua.lua_pcallk(this.L, args.length, Lua.LUA_MULTRET, 0, 0, null); // lua_call is defined as a macro, so its not available to us. this is equivalent.
			if (errCode == 0) {
				var results:int = Lua.lua_gettop(this.L);
				var arr:Array = new Array(results + 1);
				for (i = 1; i <= results; i++){
					arr[i] = getAS3(this.L, i);
				}
				Lua.lua_pop(this.L, results);
				arr[0] = errCode;
				return arr;
			} else {
				var str:String = getAS3(this.L,-1);
				Lua.lua_pop(this.L, 1);
				trace("Lua error: " + str);
				return [errCode,str];
			}
		}

		/*
			Equivalent to ref(...) in Lua.
			This version does not return any values except those regarding the execution of the function.
			
			Returns: An array, where arr[0] is the error code.
			If there is no error, arr[0] is 0.
			If there is an error, arr[0] is an error code, and arr[1] is the error message.
		*/
		public function execute(... args) : Array
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var i:int = 0;
			for (i = 0; i < args.length; i++){
				pushAS3(this.L,args[i]);
			}
			var errCode:int = Lua.lua_pcallk(this.L, args.length, 0, 0, 0, null); // lua_call is defined as a macro, so its not available to us. this is equivalent.
			if (errCode == 0) {return [0];}
			else {
				var str:String = getAS3(this.L,-1);
				Lua.lua_pop(this.L, 1);
				trace("Lua error: " + str);
				return [errCode,str];
			}
		}

		/*
			Checks the type of the lua reference, throwing an error if it is wrong.
			arg number and function name can be provided for additional error information.

			Use values in LuaEnum to specify type (e.g, LuaEnums.LUA_TBOOLEAN)
		*/
		public function checkType(expected:int, arg:int = -1, fname:String = null) : void
		{
			var actual:int = this.type();
			if (actual != expected) {
				if (arg == -1) {
					throw new Error("expected " + LuaEnums.LUA_TYPEMAP[expected] + ", got " + LuaEnums.LUA_TYPEMAP[actual]);
				} else {
					if (fname != null) {
						throw new Error("bad argument #" + arg + " to '" + fname + "' (expected " + LuaEnums.LUA_TYPEMAP[expected] + ", got " + LuaEnums.LUA_TYPEMAP[actual] + ")");
					} else {
						throw new Error("bad argument #" + arg + " (expected " + LuaEnums.LUA_TYPEMAP[expected] + ", got " + LuaEnums.LUA_TYPEMAP[actual] + ")");
					}
				}
			}
		}

		/*
			Returns the metatable of the given Lua object.
			If there is no metatable, then returns null.
		*/
		public function getMetatable() : LuaReference
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var hasMetaTable:Boolean = Lua.lua_getmetatable(this.L,-1) != 0;
			if (hasMetaTable) {
				var ref:Object = getAS3(this.L,-1);
				Lua.lua_pop(this.L,2);
				return (ref as LuaReference); // Guaranteed to be a LuaReference?
			} else {
				Lua.lua_pop(this.L,1);
				return null;
			}
		}

		/*
			Sets the metatable of the given Lua object to a given table.
		*/
		public function setMetatable(tab:LuaReference) : void
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,tab.ref);
			Lua.lua_setmetatable(this.L,-2);
			Lua.lua_pop(this.L,1);
		}
		
		/*
			Gets a Vector.<Array> representing the pairs of the table.
			Each Array has two items: {key, value}.
			
			Warning: This can generate new LuaReference objects that may need cleared.
		*/
		public function getPairs() : Vector.<Array>
		{
			var pairs:Vector.<Array> = new Vector.<Array>();
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			Lua.lua_pushnil(this.L);
			while (Lua.lua_next(this.L, -2) != 0) {
				/* Key at -2, value at -1. */
				var pair:Array = [null, null];
				pair[0] = getAS3(this.L, -2);
				pair[1] = getAS3(this.L, -1);
				pairs.push(pair);
				Lua.lua_pop(this.L,1);
				/* Key now at -1, table now at -2 */
			}
			Lua.lua_pop(this.L, 1);
			return pairs;
		}
		
		/*
			Gets a Vector.<Array> representing the pairs of the table.
			Each array has two items: {key, value}.
			Any non-data portions are cut. (e.g functions, threads, unconvertible userdata)
			Tables are converted recursively instead of to LuaReferences.
			Can take a maxDepth parameter to cut off the recursion.
		*/
		public function getData(maxDepth:int = 16) : Vector.<Array>
		{
			if (maxDepth < 0) {return null;} // this should probably error or something but that's scary...
			var pairs:Vector.<Array> = new Vector.<Array>();
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			Lua.lua_pushnil(this.L);
			var tempRef:LuaReference = null;
			while (Lua.lua_next(this.L, -2) != 0) {
				/* Key at -2, value at -1. */
				var pair:Array = [null, null];
				pair[0] = getDataAS3(this.L, -2);
				pair[1] = getDataAS3(this.L, -1);
				if (pair[0] is LuaReference) {
					tempRef = pair[0];
					pair[0] = tempRef.getData(maxDepth - 1);
					tempRef.decRef(); // free
				}
				if (pair[1] is LuaReference) {
					tempRef = pair[1];
					pair[1] = tempRef.getData(maxDepth - 1);
					tempRef.decRef();
				}
				if (pair[0] != null && pair[1] != null) {
					pairs.push(pair);
				}
				Lua.lua_pop(this.L,1);
				/* Key now at -1, table now at -2 */
			}
			Lua.lua_pop(this.L, 1);
			return pairs;
		}
		
		// Helper to deal with recursion easier.
		private static function setFieldsOnTop(L:int, pairs:Vector.<Array>){
			// We assume table is on top of stack (-1)
			for (var i:int = 0; i < pairs.length; i++) {
				var key:* = pairs[i][0];
				var value:* = pairs[i][1];
				
				if (key is Vector.<Array>) {
					Lua.lua_createtable(L, 0, 0);
					setFieldsOnTop(key);
				} else {
					pushAS3(L, key);
				}
				if (value is Vector.<Array>) {
					Lua.lua_createtable(L, 0, 0);
					setFieldsOnTop(value);
				} else {
					pushAS3(L, value);
				}
				Lua.lua_settable(L, -3);
			}
		}
		
		/*
			Takes a Vector.<Array> representing the pairs to set, and sets them.
			(See getData and getPairs)
		*/
		public function setFields(pairs:Vector.<Array>) : void
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			setFieldsOnTop(this.L, pairs);
			Lua.lua_pop(this.L, 1); // pop it.
		}
		
		/*
			Returns a reasonable string conversion.
			Calls __tostring metamethod if it exists.
		*/
		public function toString() : String
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			Lua.luaL_tolstring(this.L, -1, null);
			var str:String = getAS3(this.L,-1);
			Lua.lua_pop(this.L, 2);
			return str;
		}

	}
}
