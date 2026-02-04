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

### AS3 objects in Lua code.
  - `tostring(as3obj)` calls the AS3 `.toString()` method.
  - `type(as3obj)` returns `"flash"`.
  - The `__index` metamethod indexes the AS3 object, and converts the returned value to a Lua value if possible.
  - The `__newindex` metamethod attempts to set the given property. Any error that occurs is silenced.
  - The `__call` metamethod will call the AS3 object with the given arguments, if it is a Function. Any error that occurs is thrown as a Lua error.

## Random Library

The random library allows the creation of RNG objects, which can be used to generate random numbers.
Each RNG object has its own state, independent of all others.
In this section, `RNG` is used as a standin for any arbitrary RNG object.

### random.new(seed?)
  - Returns a new RNG object.
  - If a seed is provided, seeds the RNG object with the given seed. All 64 bits of the seed are used.
  - If a seed is not provided, one is generated using AS3 `Math.random();`

### random.swap(rng1, rng2)
  - Swaps the internal states of the two RNG objects.

### random.clone(rng)
  - Returns a new RNG object with an identical state to the provided RNG object.

### RNG.random(a?, b?)
  - Equivalent to math.random(a?, b?), but the returned value is based on the RNG object's internal state.
  - This function is implemented as a closure.

### RNG.randomseed(seed?)
  - Seeds the RNG object, using the same semantics as `random.new`

### RNG.getseed()
  - Returns the last seed used to seed the RNG object.

### RNG objects
  - `tostring(RNG)` returns `"(RNG : {ptr})"`
  - `type(RNG)` returns `"random"`
  - RNG objects are internally `userdata`

## Library Extensions

stuff
