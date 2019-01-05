# mulle-sprintf

🔢 An extensible sprintf function supporting stdarg and mulle-vararg

The extensibility is used in **MulleObjCStandardFoundation** to add the
'@' conversion character for object conversion.

mulle-sprintf can handle **varargs** and **mulle_vararg** style variable
arguments. Because floating point to string conversion is hard, floating point
conversions are handed down to `sprintf`. So it is not a sprintf replacement.

> #### Note
>
> Because mulle-sprintf uses a dynamic loading scheme, it is important
> that the linker doesn't strip presumably "dead" code. This will happen
> if you link mulle-sprintf as a static library without using --all_load or
> some such.
>
> [Reference](//www.chrisgummer.com/llvm-load_all-and-force_load)
>


Build Status | Release Version
-------------|-----------------------------------
[![Build Status](https://travis-ci.org/mulle-core/mulle-sprintf.svg?branch=release)](https://travis-ci.org/mulle-core/mulle-sprintf) | ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-core/mulle-sprintf.svg) [![Build Status](https://travis-ci.org/mulle-core/mulle-sprintf.svg?branch=release)](https://travis-ci.org/mulle-core/mulle-sprintf)


## Install

Install the prerequisites first:

| Prerequisites                                              |
|------------------------------------------------------------|
| [mulle-thread](//github.com/mulle-concurrent/mulle-thread) |
| [mulle-buffer](//github.com/mulle-c/mulle-buffer)          |
| [mulle-utf](//github.com/mulle-c/mulle-utf)                |
| [mulle-vararg](//github.com/mulle-c/mulle-vararg)          |

Then build and install

```
mkdir build 2> /dev/null
(
   cd build ;
   cmake .. ;
   make install
)
```

Or let [mulle-sde](//github.com/mulle-sde) do it all for you.


## Example


Here is an example of using `mulle_sprintf` together with [`mulle-buffer`](//github.com/mulle-c/mulle-buffer) to print an integer into a char array safely:

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


### Platforms and Compilers

All platforms and compilers supported by
[mulle-c11](//github.com/mulle-c/mulle-c11) and
[mulle-thread](//github.com/mulle-concurrent/mulle-thread).


## Author

[Nat!](//www.mulle-kybernetik.com/weblog) for
[Mulle kybernetiK](//www.mulle-kybernetik.com) and
[Codeon GmbH](//www.codeon.de)
