LuaResultArray class
==========================
LuaResultArray is a class that extends Array.
\
\
When Lua receives a LuaResultArray as a function result, it converts it into multiple results.
\
This allows AS3 to return multiple results to Lua in the same way that a C or Lua function can.
\
\
See src/main/actionscript/Test.as for an example (the getValues() method)
