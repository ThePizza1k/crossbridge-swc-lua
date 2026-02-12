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

	}
}