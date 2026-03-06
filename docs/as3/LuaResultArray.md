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
\
\
Note that the LuaResultArray constructor does not directly correspond to Array's constructor.
\
The constructor always pushes all given arguments to the array, even if there is only one argument.
