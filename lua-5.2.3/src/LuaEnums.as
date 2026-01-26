package crossbridge.lua{

	public class LuaEnums {
	
		public static const LUA_REGISTRYINDEX:int = -1001000;
		
		public static const LUA_REFNIL:int = -1;
		public static const LUA_NOREF:int = -2;
		
		public static const LUA_TNONE:int = -1;
		
		public static const LUA_TNIL:int = 0;
		public static const LUA_TBOOLEAN:int = 1;
		public static const LUA_TLIGHTUSERDATA:int = 2;
		public static const LUA_TNUMBER:int = 3;
		public static const LUA_TSTRING:int = 4;
		public static const LUA_TTABLE:int = 5;
		public static const LUA_TFUNCTION:int = 6;
		public static const LUA_TUSERDATA:int = 7;
		public static const LUA_TTHREAD:int = 8;
		
		public static const LUA_NUMTAGS:int = 9;
		
		public static const LUA_TYPEMAP:Vector.<String> = new <String>["nil","boolean", "userdata", "number", "string", "table", "function", "userdata", "thread"];
		
		public static const LUA_RIDX_MAINTHREAD:int = 1;
		public static const LUA_RIDX_GLOBALS:int = 2;
		
		public static const LUA_MULTRET:int = -1;
		
		public static const LUA_OK:int = 0;
		public static const LUA_YIELD:int = 1;
		public static const LUA_ERRRUN:int = 2;
		public static const LUA_ERRSYNTAX:int = 3;
		public static const LUA_ERRMEM:int = 4;
		public static const LUA_ERRGCMM:int = 5;
		public static const LUA_ERRERR:int = 6;
		
		public static const LUA_OPADD:int = 0;
		public static const LUA_OPSUB:int = 1;
		public static const LUA_OPMUL:int = 2;
		public static const LUA_OPDIV:int = 3;
		public static const LUA_OPMOD:int = 4;
		public static const LUA_OPPOW:int = 5;
		public static const LUA_OPUNM:int = 6;
		
		public static const LUA_OPEQ:int = 0;
		public static const LUA_OPLT:int = 1;
		public static const LUA_OPLE:int = 2;
		
		public static const LUA_GCSTOP:int = 0;
		public static const LUA_GCRESTART:int = 1;
		public static const LUA_GCCOLLECT:int = 2;
		public static const LUA_GCCOUNT:int = 3;
		public static const LUA_GCCOUNTB:int = 4;
		public static const LUA_GCSTEP:int = 5;
		public static const LUA_GCSETPAUSE:int = 6;
		public static const LUA_GCSETSTEPMUL:int = 7;
		public static const LUA_GCSETMAJORINC:int = 8;
		public static const LUA_GCISRUNNING:int = 9;
		public static const LUA_GCGEN:int = 10;
		public static const LUA_GCINC:int = 11;
		
		public static const LUA_HOOKCALL:int = 0;
		public static const LUA_HOOKRET:int = 1;
		public static const LUA_HOOKLINE:int = 2;
		public static const LUA_HOOKCOUNT:int = 3;
		public static const LUA_HOOKTAILCALL:int = 4;
		
		
	}
	
}