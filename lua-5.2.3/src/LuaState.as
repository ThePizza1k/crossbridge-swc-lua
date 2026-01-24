package
{
	import crossbridge.lua.CModule;
	import crossbridge.lua.__lua_objrefs;
	
	public class LuaState
	{
		private static const LUA_REGISTRYINDEX:int = -1001000; // Lua.LUA_REGISTRYINDEX lies to you!
		
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
		
		private function pushAS3(obj:Object):void
		{
			if (obj is Number){
				Lua.lua_pushnumber(this.luaState, obj as Number);
			} else if (obj is Boolean){
				Lua.lua_pushboolean(this.luaState, int(obj));
			} else if (obj is String) {
				Lua.lua_pushstring(this.luaState, obj as String);
			} else if (obj is Function) {
				// Just be an object for now.
				var fnptr:int = Lua.push_flashref(this.luaState);
				__lua_objrefs[fnptr] = obj;
			} else if (obj == null) {
				Lua.lua_pushnil(this.luaState);
			} else {
				if (__lua_objrefs[obj] == undefined) {
					var udptr:int = Lua.push_flashref(this.luaState);
					__lua_objrefs[udptr] = obj;
					__lua_objrefs[obj] = udptr;
					Lua.luaL_getsubtable(this.luaState, LUA_REGISTRYINDEX, "flash_refs");
					Lua.lua_pushvalue(this.luaState,-2);
					Lua.lua_rawseti(this.luaState,-2, udptr); // Store a reference
					Lua.lua_pop(this.luaState,1);
				} else {
					Lua.luaL_getsubtable(this.luaState, LUA_REGISTRYINDEX, "flash_refs");
					Lua.lua_rawgeti(this.luaState, -1, __lua_objrefs[obj]); // Retrieve a reference
					Lua.lua_replace(this.luaState, -2);
				}
			}
		}
		
		private function getAS3(ind:int):Object
		{
			var t:int = Lua.lua_type(this.luaState,ind);
			switch(t) {
				case (Lua.LUA_TNIL):
					return null;
				case (Lua.LUA_TNUMBER):
					return Lua.lua_tonumberx(this.luaState,ind, 0);
				case (Lua.LUA_TBOOLEAN):
					return Boolean(Lua.lua_toboolean(this.luaState,ind));
				case (Lua.LUA_TSTRING):
					return Lua.lua_tolstring(this.luaState, ind, 0);
				case (Lua.LUA_TTABLE):
					return new Object(); // These are not trivially convertible, so not gonna bother.
				case (Lua.LUA_TFUNCTION):
					if (ind < 0) {ind = Lua.lua_gettop(this.luaState) + ind + 1;}
					Lua.lua_pushvalue(this.luaState,ind);
					var ref:int = Lua.luaL_ref(this.luaState, LUA_REGISTRYINDEX);
					var state:LuaState = this;
					return function(...vaargs):Object
						{
							var results:int = Lua.lua_gettop(state.luaState);
							Lua.lua_rawgeti(state.luaState, LUA_REGISTRYINDEX, ref);
							for(var i:int = 0; i<vaargs.length;i++) {
								var udptr:int = Lua.push_flashref(state.luaState);
								__lua_objrefs[udptr] = vaargs[i];
							}
							var errCode:int = Lua.lua_pcallk(state.luaState, vaargs.length, Lua.LUA_MULTRET, 0, 0, null);
							if (errCode == 0) {
								results = Lua.lua_gettop(state.luaState) - results;
								var arr:Array = new Array(results);
								for (i = 0; i < results; i++){
									arr[i] = state.getAS3(i-results);
								}
								Lua.lua_pop(state.luaState, results);
								return arr;
							} else{
								var str:String = state.getAS3(-1);
								Lua.lua_pop(state.luaState, 1);
								trace(str);
								return str
							}
						};
				case (Lua.LUA_TUSERDATA):
					var ptr:int = Lua.lua_touserdata(this.luaState,ind);
					return __lua_objrefs[ptr];
				case (Lua.LUA_TTHREAD):
					return null; // These are not trivially convertible so not gonna bother
				case (Lua.LUA_TLIGHTUSERDATA):
					return Lua.lua_touserdata(this.luaState,ind); // Pointer to whatever tf it's pointing to
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
			pushAS3(obj);
			Lua.lua_setglobal(this.luaState, name);
		}
		
		public function getGlobal(name:String) : Object
		{
			checkNull();
			Lua.lua_getglobal(this.luaState,name);
			var obj:Object = getAS3(-1);
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
				this.pushAS3(args[i]);
			}
			var errCode:int = Lua.lua_pcallk(this.luaState, args.length, Lua.LUA_MULTRET, 0, 0, null); // lua_call is defined as a macro, so its not available to us. this is equivalent.
			if (errCode == 0) {
				var results:int = Lua.lua_gettop(this.luaState);
				var arr:Array = new Array(results + 1);
				for (i = 1; i <= results; i++){
					arr[i] = this.getAS3(i);
				}
				Lua.lua_pop(this.luaState, results);
				return arr;
			} else {
				var str:String = this.getAS3(-1);
				Lua.lua_pop(this.luaState, 1);
				trace("Lua error: " + str);
				return [null,str]; // stupid ass shit.
			}
		}
		
		public function doString(chunk:String) : void
		{
			checkNull();
			var errCode:int = 0;
			errCode = Lua.luaL_loadstring(this.luaState, chunk);
			if (errCode != 0) {
				var str:String = this.getAS3(-1);
				Lua.lua_pop(this.luaState, 1);
				trace("Lua error (doString, load):" + str);
				throw new Error();
			}
			errCode = Lua.lua_pcallk(this.luaState, 0, 0, 0, 0, null);
			if (errCode != 0) {
				var str:String = this.getAS3(-1);
				Lua.lua_pop(this.luaState, 1);
				trace("Lua error (doString, call):" + str);
				throw new Error();
			}
		}
			
	}
	
}