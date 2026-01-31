package
{
	import crossbridge.lua.CModule;
	import crossbridge.lua.__lua_objrefs;
  import crossbridge.lua.LuaReference;
	
	public class LuaState
	{
		
		private var luaState:int;
		
		
		public function LuaState()
		{
			this.luaState = Lua.luaL_newstate();
			Lua.luaL_openlibs(luaState);
		}
		
		private function checkNull() : void
		{
			if (this.luaState == 0) {throw new Error("Attempted to use a closed lua state!", 1009); } // error 1009 is null reference error, which is reasonable equivalent to this case?
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
        if (obj.constructor in __lua_typerefs[L]) {
          Lua.lua_rawgeti(L, LuaEnums.LUA_REGISTRYINDEX, t_ref);
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
						Lua.lua_pushvalue(L,ind);
						var ref:int = Lua.luaL_ref(L, LuaEnums.LUA_REGISTRYINDEX);
						return new LuaReference(L, ref);
					}
				case (LuaEnums.LUA_TLIGHTUSERDATA):
					return Lua.lua_touserdata(L,ind); // dubious return for this (returns as int), but we're not using light userdata anyways.
				default:
					throw new Error("Unexpected lua type of id: " + t);
					return null; // Wtf?
			}
		}
		
		public function close() : void
		{
			checkNull();
			Lua.lua_close(this.luaState);
			this.luaState = 0;
		}
		
		public function setGlobal(name:String, obj:Object) : void
		{
			checkNull();
			pushAS3(this.luaState,obj);
			Lua.lua_setglobal(this.luaState, name);
		}
		
		public function getGlobal(name:String) : Object
		{
			checkNull();
			Lua.lua_getglobal(this.luaState,name);
			var obj:Object = getAS3(this.luaState,-1);
			Lua.lua_pop(this.luaState, 1);
			return obj;
		}
		
		public function callGlobal(name:String, ... args) : Array
		{
			checkNull();
			Lua.lua_settop(this.luaState, 0);
			Lua.lua_getglobal(this.luaState, name);
			var i:int = 0;
			for (i = 0; i < args.length; i++){
				pushAS3(this.luaState,args[i]);
			}
			var errCode:int = Lua.lua_pcallk(this.L, args.length, Lua.LUA_MULTRET, 0, 0, null); // lua_call is defined as a macro, so its not available to us. this is equivalent.
			if (errCode == 0) {
				var results:int = Lua.lua_gettop(this.L);
				var arr:Array = new Array(results + 1);
				for (i = 1; i <= results; i++){
					arr[i] = getAS3(this.luaState,i);
				}
				Lua.lua_pop(this.luaState, results);
				arr[0] = errCode;
				return arr;
			} else {
				var str:String = getAS3(this.luaState,-1);
				Lua.lua_pop(this.luaState, 1);
				trace("Lua error: " + str);
				return [errCode,str];
			}
		}
		
		public function doString(chunk:String) : void // Use for untrusted code (i.e, to set up an environment or something)
		{
			checkNull();
			var errCode:int = 0;
			errCode = Lua.luaL_loadstring(this.luaState, chunk);
			if (errCode != 0) {
				var str:String = getAS3(this.luaState,-1);
				Lua.lua_pop(this.luaState, 1);
				throw new Error("Lua error (doString, load):" + str);
			}
			errCode = Lua.lua_pcallk(this.luaState, 0, 0, 0, 0, null);
			if (errCode != 0) {
				var str:String = getAS3(this.luaState,-1);
				Lua.lua_pop(this.luaState, 1);
				throw new Error("Lua error (doString, call):" + str);
			}
		}
		
    public function newTable(narr:int = 0, nrec:int = 0) : LuaReference
    {
      Lua.lua_createtable(this.luaState, narr, nrec);
      var ref:int = Lua.luaL_ref(this.luaState, LuaEnums.LUA_REGISTRYINDEX);
      return new LuaReference(this.luaState, ref);
    }


	}
	
}