package{
	import crossbridge.lua.CModule;
	import crossbridge.lua.LuaState;
	import crossbridge.lua.LuaReference;

	public class Test {

		public static function getTable() : LuaReference
		{
			var tab:LuaReference = CModule.rootSprite.luastate.newTable();
			for (var i:int = 0; i < 5; i++) {
				tab.setField(i+1, i*i);
			}
			tab.setField("value", 51);
			tab.setField(true, "Freaky!");
			return tab;
		}

		public static function preAllocGetTable() : LuaReference
		{
			var tab:LuaReference = CModule.rootSprite.luastate.newTable(5, 2);
			for (var i:int = 0; i < 5; i++) {
				tab.setField(i+1, i*i);
			}
			tab.setField("value", 51);
			tab.setField(true, "Freaky!");
			return tab;
		}

		public static function setTableField(l : LuaReference, k : Object, v : Object) : void
		{
			l.setField(k, v);
		}

		public static function execute(f : LuaReference) : void
		{
			f.execute();
		}
	}
}