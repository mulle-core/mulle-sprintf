# mulle-sprintf

an extensible variant of sprintf written in C (C11).

The extensibility is used in **MulleObjCFoundation** to add the '@' conversion
character for object conversion.

mulle-sprintf can handle **varargs** and **mulle_vararg** style variable
arguments. Because floating point to string conversion is hard, floating point 
conversions are handed down to `sprintf`. So it is not a sprintf replacement.


Fork      |  Build Status | Release Version
----------|---------------|-----------------------------------
[Mulle kybernetiK](//github.com/mulle-nat/mulle-sprintf) | [![Build Status](https://travis-ci.org/mulle-nat/mulle-sprintf.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-sprintf) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-nat/mulle-sprintf.svg) [![Build Status](https://travis-ci.org/mulle-nat/mulle-sprintf.svg?branch=release)](https://travis-ci.org/mulle-nat/mulle-sprintf)

## Example


Here is an example of using `mulle_sprintf` together with [`mulle-buffer`](//github.com/mulle-nat/mulle-buffer) to print an integer into a char array safely:

```
#include <mulle_sprintf/mulle_sprintf.h>
#include <mulle_buffer/mulle_buffer.h>
#include <stdio.h>


int   main( void)
{
   struct mulle_buffer   buffer;
   auto char             storage[ 32];

   mulle_buffer_init_inflexible_with_static_bytes( &buffer, storage, sizeof( storage));

   mulle_sprintf( &buffer, "%d", 1848);
   printf( "%.*s\n", (int) mulle_buffer_get_length( &buffer), mulle_buffer_get_bytes( &buffer));
   mulle_buffer_done( &buffer);

   return( 0);
}
```


## API

File                                  | Description
------------------------------------- | -------------------------------------
[`mulle_sprintf`](dox/API_SPRINTF.md) | The various sprintf like functions


## Install

On OS X and Linux you can use [homebrew](//brew.sh), respectively
[linuxbrew](//linuxbrew.sh) to install the library:

```
brew tap mulle-kybernetik/software/mulle-sprintf
```

On other platforms you can use **mulle-install** from
[mulle-build](//github.com/mulle-nat/mulle-build) to install the library:

```
mulle-install --prefix /usr/local --branch release https://github.com/mulle-nat/mulle-sprintf
```

Otherwise read:

* [How to Build](dox/BUILD.md)


### Platforms and Compilers

All platforms and compilers supported by
[mulle-c11](//www.mulle-kybernetik.com/software/git/mulle-c11/)

## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
