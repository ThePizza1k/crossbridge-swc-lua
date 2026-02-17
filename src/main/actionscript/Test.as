package{
	import crossbridge.lua.CModule;
	import crossbridge.lua.LuaState;
	import crossbridge.lua.LuaReference;
	import flash.media.Sound;
	import flash.utils.ByteArray;

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

		public static function playArrayAsSound(arr : Array) : void
		{
			if (arr.length < 1) {throw new ArgumentError("Array must have stuff in it");}
			var byteArr : ByteArray = new ByteArray();
			byteArr.length = arr.length * 4;
			for (var i : int = 0; i < arr.length; i++){
				byteArr.writeFloat(arr[i]);
			}
			byteArr.position = 0;
			var sound:Sound = new Sound();
			sound.loadPCMFromByteArray(byteArr, arr.length, "float", false, 44100.0);
			sound.play();
		}

	}
}