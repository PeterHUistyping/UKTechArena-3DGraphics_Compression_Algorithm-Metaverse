# Obj Serialization Specification

First, we define an optimization technique `3pack` that packs three 21-bit ints `a, b, c` into one 64-bit integer `s`.
```c++
s = (a << 42) || (b << 21) || c;
```

This way, we can store three ints (`-1048576 ~ 1048575`) in 8 bytes.

## General structure of the file

The first part is reserved for one-time flags and block length numbers. (Note: there are no line breaks. They are only here to aid readability.)

```
[Flags] (Size TBD, probably 8 bits or 16 bits)
Number of Blocks (Size: 1 byte)
Length of Block 1 (in number of entries, int32), Block 1
Length of Block 2 (in number of entries, int32), Block 2
...
```

## 4 Types of structures (Blocks)

### v

Each v has 3 elements, each of which have 6 d.p., and may be too large to 3pack. 

Therefore each v value is multiplied by 1e6, and then it would correspond to a 32-bit integer. (float32 is not enough)

### vt

Each vt has 2 elements, each of which is between 0 and 1 and has 6 sig figs (or in the case of 1, 7 sig figs). 

Therefore each vt value is multiplied by 1e6, and then it would correspond to a 32-bit integer. (int16 is not sufficient)

### vn

Each vn has 3 elements, each of which is between -1 and 1 and has 6 sig figs.

Therefore each vn value is multiplied by 1e6, and then 3packed into one 64-bit integer.

### f

Each f has three elements. Each element is in the form `v/[vt]/vn`. They can be 3packed into one 64-bit integer.

