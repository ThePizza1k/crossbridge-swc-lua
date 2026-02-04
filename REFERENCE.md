Reference for Lua Extensions
==================================

Some extensions are provided for interaction with Flash, or because we wanted them for PR3.
Some base Lua libraries were extended.

## Flash Library

The flash library provides some tools for interop with AS3. Be aware that some of these functions are not safe, and should not be allowed for user code.

### flash.trace(str)
  - Equivalent to AS3 `trace(str);`.
  - Input will be converted to string.

### flash.gettimer()
  - Equivalent to AS3 `getTimer();`

### flash.new(className, ...)
  - Constructs and returns a new AS3 object of type className, with the arguments given.
  - e.g `flash.new("flash.display.BitmapData", 40, 40)`
  - This function is not safe for user code.

### flash.type(as3Obj)
  - Equivalent to AS3 `getQualifiedClassName(as3Obj);`
  - Example output: `"flash.display::BitmapData"`
  - While this function is safe, you may want to instead provide a wrapper, to provide more suitable type names.

### flash.toarray(t)
  - Converts a given table to an AS3 array.
  - Does not convert functions, threads, or non-convertible userdata.
  - Only converts numeric keys in range 1 to #t
  - Returned AS3 array uses 0 based indexing.
  - Tables within the given table are also converted to AS3 arrays.
  - Max recursive depth of 32.

### flash.toobject(t)
  - Converts a given table to an AS3 object.
  - Does not convert functions, threads, or non-convertible userdata.
  - Only converts string keys.
  - Tables within the given table are also converted to AS3 objects.
  - Max recursive depth of 32.

### flash.registerConversion(className,funct)
  - Registers a conversion function for a given type.
  - This conversion function will be called whenever an object of the given type is received.
  - This conversion function will receive an AS3 object and can return anything.
  - This function is not safe for user code.

### flash.getclass(className)
  - Equivalent to `getDefinitionByName(classname);`
  - The returned class bypasses the registerConversion system.
  - This function is not safe for user code.

## Random Library

stuff

## Library Extensions

stuff
