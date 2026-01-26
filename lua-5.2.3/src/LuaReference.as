package crossbridge.lua{

	/*
		A LuaReference object holds a reference to a lua object that cannot be directly converted.
		This would include:
			tables
			functions
			threads/coroutines
			non-flash userdata without a metamethod for conversion to AS3.
		
		Things to keep in mind:
			Free LuaReference objects when you are done with them, so that lua can potentially garbage collect the lua object.
			If a LuaReference is received, and an error is thrown, then the LuaReference will be automatically freed.
			Otherwise, it will stay until you free it.
			Type checking is up to you.
				The integers for "expected type" are in LuaEnums
				e.g lua_ref.checkType(LuaEnums.LUA_TTABLE, 2, "awesomeMethod"); // type check a lua reference received as argument 2.
				// if wrong type, throws error: "bad argument #2 to 'awesomeMethod' (expected table, got <type>)"
	*/

	public class LuaReference {
		
		public var L:int; // lua state pointer
		public var ref:int; // index in LUA_REGISTRYINDEX
		
		public function LuaReference(luaState:int, referenceNum:int)
		{
			this.L = luaState;
			this.ref = referenceNum;
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
				case (LuaEnums.LUA_TTABLE):
					if (ind < 0) {ind = Lua.lua_gettop(L) + ind + 1;}
					Lua.lua_pushvalue(L,ind);
					var ref:int = Lua.luaL_ref(L, LuaEnums.LUA_REGISTRYINDEX);
					return new LuaReference(L, ref);
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
						Lua.lua_pushvalue(L,ind);
						var ref:int = Lua.luaL_ref(L, LuaEnums.LUA_REGISTRYINDEX);
						return new LuaReference(L, ref);
					}
				case (LuaEnums.LUA_TTHREAD):
					if (ind < 0) {ind = Lua.lua_gettop(L) + ind + 1;}
					Lua.lua_pushvalue(L,ind);
					var ref:int = Lua.luaL_ref(L, LuaEnums.LUA_REGISTRYINDEX);
					return new LuaReference(L, ref);
				case (LuaEnums.LUA_TLIGHTUSERDATA):
					return Lua.lua_touserdata(L,ind); // dubious return for this (returns as int), but we're not using light userdata anyways.
				default:
					throw new Error("Unexpected lua type of id: " + t);
					return null; // Wtf?
			}
		}
		
		public function type() : int
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var t:int = Lua.lua_type(this.L,-1);
			Lua.lua_pop(this.L,1);
			return t;
		}
		
		public function typename() : String
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var t:int = Lua.lua_type(this.L,-1);
			Lua.lua_pop(this.L,1);
			return LuaEnums.LUA_TYPEMAP[t];
		}
	
		public function free() : void
		{
			if (this.ref == LuaEnums.LUA_NOREF) {return;}
			Lua.luaL_unref(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			this.ref = LuaEnums.LUA_NOREF;
		}
		
		public function clone() : LuaReference
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			var ref:int = Lua.luaL_ref(this.L,LuaEnums.LUA_REGISTRYINDEX);
			return new LuaReference(this.L, ref);
		}
		
		public function getField(key : Object) : Object
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			pushAS3(this.L, key);
			Lua.lua_gettable(this.L,-2); // can trigger __index
			var ret:Object = getAS3(this.L, -1);
			Lua.lua_pop(this.L,2); // pop object and value.
			return ret;
		}
		
		public function setField(key : Object, value : Object) : void
		{
			Lua.lua_rawgeti(this.L,LuaEnums.LUA_REGISTRYINDEX,this.ref);
			pushAS3(this.L,key);
			pushAS3(this.L,value);
			Lua.lua_settable(this.L,-3); // what happens if __newindex throws an error?
			Lua.lua_pop(this.L,1);
		}
		
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
					arr[i] = this.getAS3(i);
				}
				Lua.lua_pop(this.luaState, results);
				arr[0] = errCode;
				return arr;
			} else {
				var str:String = this.getAS3(-1);
				Lua.lua_pop(this.luaState, 1);
				trace("Lua error: " + str);
				return [errCode,str];
			}
		}
		
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
	}
}