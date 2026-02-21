Reference for Lua Extensions
==================================

Some extensions are provided for interaction with Flash, or because we wanted them for PR3.
Some base Lua libraries were extended.

## Flash Library

The flash library provides some tools for interop with AS3. Be aware that some of these functions are not safe, and should not be allowed for user code.

<details>

<summary>Functions</summary>

### flash.trace(str : string|number) : void
  - Equivalent to AS3 `trace(str);`.
  - Input will be converted to string.

### flash.gettimer() : number
  - Equivalent to AS3 `getTimer();`

### flash.new(className : String, args : ...) : flash
  - Constructs and returns a new AS3 object of type className, with the arguments given.
  - e.g `flash.new("flash.display.BitmapData", 40, 40)`
  - This function is not safe for user code.

### flash.type(as3Obj : flash) : string
  - Equivalent to AS3 `getQualifiedClassName(as3Obj);`
  - Example output: `"flash.display::BitmapData"`
  - While this function is safe, you may want to instead provide a wrapper, to provide more suitable type names.

### flash.toarray(t : table) : flash
  - Converts a given table to an AS3 array.
  - Does not convert functions, threads, or non-convertible userdata.
  - Only converts numeric keys in range 1 to #t
  - Returned AS3 array uses 0 based indexing.
  - Tables within the given table are also converted to AS3 arrays.
  - Max recursive depth of 32.

### flash.toobject(t : table) : flash
  - Converts a given table to an AS3 object.
  - Does not convert functions, threads, or non-convertible userdata.
  - Only converts string keys.
  - Tables within the given table are also converted to AS3 objects.
  - Max recursive depth of 32.

### flash.registerConversion(className : string, f : function) : none
  - Registers a conversion function for a given type.
  - This conversion function will be called whenever an object of the given type is received.
  - This conversion function will receive an AS3 object and can return anything.
  - This function is not safe for user code.

### flash.getclass(className : string) : flash
  - Equivalent to `getDefinitionByName(classname);`
  - The returned class bypasses the registerConversion system.
  - This function is not safe for user code.

### AS3 objects in Lua code.
  - `tostring(as3obj)` calls the AS3 `.toString()` method.
  - `type(as3obj)` returns `"flash"`.
  - The `__index` metamethod indexes the AS3 object, and converts the returned value to a Lua value if possible.
    - Functions will be converted to closures, which follow the same semantics given below for the `__call` metamethod when called.
    - If a conversion function is registered for the object's type, then the corresponding conversion function will be called, and the value returned by that will be returned instead.
  - The `__newindex` metamethod attempts to set the given property. Any error that occurs is silenced.
  - The `__call` metamethod will call the AS3 object with the given arguments, if it is a `Function`, and return the result. Any error that occurs is thrown as a Lua error.
    - Follows the same semantics for returns as `__index`.
   
</details>

## Random Library

The random library allows the creation of RNG objects, which can be used to generate random numbers.
Each RNG object has its own state, independent of all others.
In this section, `RNG` is used as a standin for any arbitrary RNG object.

<details>

<summary>Functions</summary>

### random.new(seed : number?) : RNG
  - Returns a new RNG object.
  - If a seed is provided, seeds the RNG object with the given seed. All 64 bits of the seed are used.
  - If a seed is not provided, one is generated using AS3 `Math.random();`

### random.swap(rng1 : RNG, rng2 : RNG) : none
  - Swaps the internal states of the two RNG objects.

### random.clone(rng : RNG) : RNG
  - Returns a new RNG object with an identical state to the provided RNG object.

### RNG.random(a : number?, b : number?) : number
  - Equivalent to math.random(a?, b?), but the returned value is based on the RNG object's internal state.
  - Return value is generated with 52 random bits.
  - This function is implemented as a closure.

### RNG.randomseed(seed : number?) : none
  - Seeds the RNG object, using the same semantics as `random.new`
  - This function is implemented as a closure.

### RNG.getseed() : number
  - Returns the last seed used to seed the RNG object.
  - This function is implemented as a closure.

### RNG objects
  - `tostring(RNG)` returns `"(RNG : {ptr})"`
  - `type(RNG)` returns `"random"`
  - RNG objects are internally `userdata`
  - RNG objects currently use xoshiro256+ to generate random numbers.

</details>

## Buffer Library

The buffer library allows the creation and usage of fixed size, mutable blocks of memory.
Buffer objects are capped at a length of 16777216 bytes.
Read and write operations for numeric values use big endian byte order.

Buffers are automatically converted to/from ByteArrays when going between AS3 and Lua.

todo: document everything

## Library Extensions

<details>
<summary>Math extensions</summary>

### math.clamp(m : number, min : number, max : number): number
  - Returns n if min < n < max.
  - Returns min if n < min.
  - Returns max if n > max.
  - Errors if min > max.
  - If n is NaN, returns NaN.

### math.sign(m : number): number
  - Returns -1 if n is negative, 1 if n is positive, or 0 if n is zero or NaN.

### math.round(m : number): number
  - Rounds m to nearest integer.
  - If m is halfway between 2 integers, rounds away from 0.

</details>

<details>
<summary>Table extensions</summary>

### table.find(t : table, v : any, n : number?) : number?
  - Returns the index of first element in t equal to v.
  - If not found, returns nil.
  - Starts search at n if specified, or at 1 otherwise. Stops at first nil.

### table.create(narr : number? = 0, nrec : number? = 0) : table
  - Creates an empty table.
  - narr is a hint for how many elements the table will have as a sequence.
  - nrec is a hint for how many other elements the table will have.
  - Lua may use these hints to preallocate space for the table, providing a performance boost in some situations.
  - Errors if narr or nrec are negative, or if memory allocation fails.
</details>
