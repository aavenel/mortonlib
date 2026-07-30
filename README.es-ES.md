

# Mortonlib

[![Build Status](https://travis-ci.org/aavenel/mortonlib.svg?branch=master)](https://travis-ci.org/aavenel/mortonlib)

Una biblioteca para realizar aritmética básica con códigos Morton.


## Codificación/Decodificación

Si el conjunto de instrucciones BMI2 está disponible en su procesador (Intel i5, i7 y algunos Xeon), 
mortonlib utilizará las instrucciones `pext` y `pdep` para codificar y decodificar códigos Morton.
De lo contrario, nos basamos en una tabla de búsqueda precomputada. Este último método es más lento, pero funciona en todos los procesadores.
Puede establecer la bandera `USE_BMI2` para definir qué estrategia desea utilizar.

Si no dispone de instrucciones BMI2 y no necesita codificar coordenadas mayores a (256,256,256), puede utilizar la función `morton3d_256(x, y, z)`, que es un poco más rápida que la genérica.

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

## Sumas, restas e incrementos

Puede sumar y restar códigos Morton sin necesidad de decodificarlos y volver a codificarlos.

```c++

//Add two morton keys
morton3(0,0,0) + morton3(1,2,3) == morton3(1,2,3);

//Substract two morton keys
morton3(4,5,6) - morton3(1,2,3) == morton3(3,3,3);

```

Si desea incrementar o decrementar una única coordenada, puede utilizar las funciones `inc()` y `dec()`, que son un poco más rápidas que la suma genérica.
```c++

//Increment Y part of morton key
morton3(4,5,6).incX() == morton3(4,5,6) + morton3(1,0,0) == morton3(5,5,6);

//Decrement Y part of morton key
morton3(4,5,6).decY() == morton3(4,5,6) - morton3(0,1,0) == morton3(4,4,6);
```

## Benchmarks


## Referencias 

* https://fgiesen.wordpress.com/2011/01/17/texture-tiling-and-swizzling/

* http://www.forceflow.be/2013/10/07/morton-encodingdecoding-through-bit-interleaving-implementations/

* http://en.wikipedia.org/wiki/Z-order_curve

* http://bitmath.blogspot.fr/2012/11/tesseral-arithmetic-useful-snippets.html

* http://asgerhoedt.dk/?p=276
