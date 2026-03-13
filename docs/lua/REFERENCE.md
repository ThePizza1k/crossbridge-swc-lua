Reference for Lua Extensions
==================================

Some extensions are provided for interaction with Flash, or because we wanted them for PR3.
Some base Lua libraries were extended.

## Flash Library

The flash library provides some tools for interop with AS3. Be aware that some of these functions are not safe, and should not be allowed for user code.
All metamethods on flash objects should be safe, so long as you limit what you provide.
(You may want to restrict access to Class objects via registerConversion, as you can access them via obj.constructor)

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
  - The `__pairs` metamethod allows use of `pairs` to loop over all dynamic properties of an Object, the same way one would with a Lua table.
   
</details>

## Random Library

The random library allows the creation of RNG objects, which can be used to generate random numbers.
Each RNG object has its own state, independent of all others.

<details>

<summary>Functions</summary>

In this section, `RNG` is used as a standin for any arbitrary RNG object.

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
This library is modeled after luau's buffer library, but has some differences. (Most notably, this uses big-endian order)

<details>
<summary>Buffer functions</summary>

In this section, `buf` is used as a standin for any arbitrary buffer object.

### buffer.new(size : number) : buffer
  - Creates a new buffer of the given size.
  - Size cannot exceed 16777216 bytes.

### buffer.fromstring(str : string) : buffer
  - Creates a new buffer from the contents of the string.
  - The buffer will have the same size as the string.
  - Buffer size cannot exceed 16777216 bytes.

### buffer.len(b : buffer) : number
  - Returns the length of the buffer.
  - `#buf` is equivalent to this.

### buffer.readu8(b : buffer, offset : number) : number
### buffer.readu16(b : buffer, offset : number) : number
### buffer.readu32(b : buffer, offset : number) : number
  - Reads the bytes at the given offset as an unsigned integer, and returns it.
  - u8 reads 1 byte, u16 reads 2 bytes, and u32 reads 4 bytes.
<br/>

### buffer.readi8(b : buffer, offset : number) : number
### buffer.readi16(b : buffer, offset : number) : number
### buffer.readi32(b : buffer, offset : number) : number
  - Reads the bytes at the given offset as a signed integer, and returns it.
  - i8 reads 1 byte, i16 reads 2 bytes, and i32 reads 4 bytes.
<br/>

### buffer.readf32(b : buffer, offset : number) : number
### buffer.readf64(b : buffer, offset : number) : number
  - Reads the bytes at the given offset as a floating point number, and returns it.
  - f32 reads 4 bytes, and f64 reads 8 bytes.
<br/>

### buffer.writeu8(b : buffer, offset : number, value : number) : void
### buffer.writeu16(b : buffer, offset : number, value : number) : void
### buffer.writeu32(b : buffer, offset : number, value : number) : void
  - Casts the given value to an unsigned integer, and writes it to the buffer at the specified offset.
  - u8 writes 1 byte, u16 writes 2 bytes, and u32 writes 4 bytes.
  - Larger data types have larger range
<br/>

### buffer.writei8(b : buffer, offset : number, value : number) : void
### buffer.writei16(b : buffer, offset : number, value : number) : void
### buffer.writei32(b : buffer, offset : number, value : number) : void
  - Casts the given value to a signed integer, and writes it to the buffer at the specified offset.
  - i8 writes 1 byte, i16 writes 2 bytes, and i32 writes 4 bytes.
  - Larger data types have larger range.
<br/>

### buffer.writef32(b : buffer, offset : number, value : number) : void
### buffer.writef64(b : buffer, offset : number, value : number) : void
  - Writes the given number to the buffer at the specified offset.
  - f32 writes 4 bytes, and f64 writes 8 bytes.
  - f64 preserves all precision of the number, while f32 reduces precision somewhat.
<br/>

### buffer.readbits(b : buffer, bitOffset : number, bitCount : number) : number
  - Reads bitCount bits from the buffer, starting at bitOffset, as an unsigned integer.
  - bitOffset is an offset in bits, rather than bytes like the other read/write functions.
  - bitCount must be in range [0, 48]

### buffer.writebits(b : buffer, bitOffset : number, bitCount : number, value : number) : void
  - Writes bitCount bits into the buffer, starting at bitOffset, from an unsigned integer.
  - bitOffset is an offset in bits, rather than bytes like the other read/write functions.
  - bitCount must be in range [0, 48]

### buffer.readstring(b : buffer, offset : number, count : number) : string
  - Reads a string of length 'count' from the buffer, starting from the specified offset.
  - Not null terminated.

### buffer.writestring(b : buffer, offset : number, value : string, count : number? = #value) : void
  - Writes a string to the buffer, starting at the specified offset.
  - If a count is specified, only writes the given amount from the string.
  - Not null terminated.

### buffer.fill(b : buffer, offset : number, value : number, count : number?) : void
  - Fills a buffer with a given byte value, starting from the offset.
  - If a count is specified, only writes that many bytes. Otherwise, it writes to the end of the buffer.

### buffer.copy(dest: buffer, destOffset: number, source: buffer, sourceOffset: number? = 0, count: number? = #sourceOffset): void
  - Copies data from source to dest.
  - Data is copied from source starting at sourceOffset (or 0 if not provided)
  - Data is copied to dest starting at destOffset.
  - If a count is not specified, copies the entire source buffer. Otherwise, only copies the specified number of bytes.

### Buffer objects.
  - `tostring(buf)` returns `"buffer: {ptr}"`
  - `type(buf)` returns `"buffer"`
  - Buffer objects are capped at a length of 16777216 bytes.
  - Read and write operations for numeric values use big endian byte order.
  - Buffer offset values are 0-based. (The first byte is at 0, and the last byte is at #b - 1)
  - Any operation that would exceed the bounds of the buffer throws an error.
  - Buffers are automatically converted to/from ByteArrays when going between AS3 and Lua.
  - Buffers index to the buffer table, so you can write `buf:writef64(0, math.pi)`
  
</details>

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
