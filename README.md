# Mortonlib

[![Build Status](https://travis-ci.org/aavenel/mortonlib.svg?branch=master)](https://travis-ci.org/aavenel/mortonlib)

A lib for basic arithmetic with morton code.


## Encoding/Decoding

If BMI2 instructions set is available on your processor (intel i5, i7 and some Xeon), 
mortonlib will use pext and pdep instructions to encode/decode morton code.
Else, we rely on a precomputed look-up table. This last method is slower but works on all processors.
You can set the flag USE_BMI2 to define which strategy you want to use.

If you don't have BMI2 instructions and you don't have to encode coordinates greater than (256,256,256), you can use the morton3d_256(x, y, z) function which is a bit faster than the generic one.

```c++

//Encode 2d morton code
morton2 m0 = morton2(x, y);

//Encode 3d morton code
morton3 m1 = morton3(x, y, z);

//if x, y and z < 256 and USE_BMI2 not set
morton3 m2 = morton3d_256(x, y, z);

//Decode 3d morton code
int x, y, z;
m1.decode(x, y, z);

```

## Additions, substractions, increments

You can add and substract morton codes without decoding and re-encoding them.

```c++

//Add two morton keys
morton3(0,0,0) + morton3(1,2,3) == morton3(1,2,3);

//Substract two morton keys
morton3(4,5,6) - morton3(1,2,3) == morton3(3,3,3);

```

If you want to increment or decrement a single coordinate, you can use inc() and dec() function, which are a little bit faster than generic add.
```c++

//Increment Y part of morton key
morton3(4,5,6).incX() == morton3(4,5,6) + morton3(1,0,0) == morton3(5,5,6);

//Decrement Y part of morton key
morton3(4,5,6).decY() == morton3(4,5,6) - morton3(0,1,0) == morton3(4,4,6);
```

## Benchmarks


## References 

* https://fgiesen.wordpress.com/2011/01/17/texture-tiling-and-swizzling/

* http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/

* http://en.wikipedia.org/wiki/Z-order_curve

* http://bitmath.blogspot.fr/2012/11/tesseral-arithmetic-useful-snippets.html

* http://asgerhoedt.dk/?p=276
