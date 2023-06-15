# mulle-sprintf

#### 🔢 An extensible sprintf function supporting stdarg and mulle-vararg

The extensibility is used in **MulleObjCStandardFoundation** to add the
'@' conversion character for object conversion.

mulle-sprintf can handle **varargs** and **mulle_vararg** style variable
arguments. Because floating point to string conversion is hard, floating point
conversions are handed down to `sprintf`. So it is not a sprintf replacement.


| Release Version                                       | Release Notes
|-------------------------------------------------------|--------------
| ![Mulle kybernetiK tag](https://img.shields.io/github/tag/mulle-core/mulle-sprintf.svg?branch=release) [![Build Status](https://github.com/mulle-core/mulle-sprintf/workflows/CI/badge.svg?branch=release)](//github.com/mulle-core/mulle-sprintf/actions)| [RELEASENOTES](RELEASENOTES.md) |


## API

| File                                  | Description
|-------------------------------------- | -------------------------------------
| [`mulle_sprintf`](dox/API_SPRINTF.md) | The various sprintf like functions


### Format characters

For more detailed information on each characte consult a
[sprintf man page](https://manpages.org/sprintf).


### Flag Characters

Character | Description
----------|---------------
<tt>&nbsp;</tt> (SPC) | A blank should be left before a positive number.
`0`       | The value should be zero padded.
`#`       | The value should be converted to an "alternate form".
`-`       | The converted value is to be left adjusted on the field boundary.
`+`       | A sign  (+  or -) should always be placed before a number.
`'`       | Use thousands' grouping characters (UNUSED)
`b`       | BOOL, print as YES or NO (for Objective-C)


### Length modifier

Characters | Description
-----------|---------------
`h`        | `short`
`hh`       | `char`
`j`        | `intmax_t`
`l`        | `long`
`ll`       | `long long`
`L`        | `long double` (FP only)
`q`        | `int64.`
`t`        | `ptrdiff_t`
`z`        | `size_t`


### Conversion Specifiers (built in)

#### Integer

Character | Description
----------|---------------
`i`       | `int` as decimal
`d`       | `int` as decimal
`D`       | `long int`                   (compatibility)
`u`       | `unsigned int`
`o`       | `int` as octal
`x`       | `int` as lowercase hex
`U`       | `unsigned long int`          (compatibility)
`O`       | `unsigned long int` as octal (compatibility)
`X`       | `unsigned long int` as hex   (compatibility)

Used modifiers: all except `L`

#### Floating Point (FP)

Uppercase conversion specifiers output value such as 0e-20 or nan as
uppercase 0E-20 or NAN.

Character | Description
----------|---------------
`a`       | double as hex [-]0xh.hhhhp+-d
`e`       | double as [-]d.ddde+-dd
`f`       | double as [-]ddd.ddd  with lowercase for inf/nan
`g`       | double in `e` or `f`
`A`       | double as hex [-]0Xh.hhhhP+-d
`E`       | double as [-]d.dddE+-dd
`F`       | double as [-]d.ddde+-dd with uppercase for INF/NAN
`G`       | double in `E` or `F`

Used modifiers: `L`.

The actual conversion is done with the C-library `sprintf` function. This is
contrast with the other conversions, which are not using the C library.


#### Pointer / String / Other

Character | Description
----------|---------------
`n`       | return conversion
`p`       | void * as hex with 0x prefix
`s`       | char * as utf8
`S`       | utf16 / utf32 with `l` modifier


Used modifiers: `l`



## Usage

### Convenient

mulle-sprintf uses a dynamic loading scheme to add conversion routines. It is
important that the linker doesn't strip presumably "dead" code. This will happen
if you link mulle-sprintf as a static library without using `--all_load` or
some such. [Reference](//www.chrisgummer.com/llvm-load_all-and-force_load)


### Inconvenient

If you have or don't want to do it the convenient way, you need to add the
character conversion routines yourself. Add them before you call a
*mulle-sprintf* printing function:

``` c
struct mulle_sprintf_conversion   *conversion;

conversion = mulle_sprintf_get_defaultconversion();
mulle_sprintf_register_character_functions( conversion);
mulle_sprintf_register_decimal_functions( conversion);
mulle_sprintf_register_escape_functions( conversion);
mulle_sprintf_register_integer_functions( conversion);
mulle_sprintf_register_fp_functions( conversion);
mulle_sprintf_register_pointer_functions( conversion);
mulle_sprintf_register_return_functions( conversion);
mulle_sprintf_register_string_functions( conversion);

mulle_sprintf_register_standardmodifiers( conversion);
```





## Add

**This project is a component of the [mulle-core](//github.com/mulle-core/mulle-core) library. As such you usually will *not* add or install it
individually, unless you specifically do not want to link against
`mulle-core`.**


### Add as an individual component

Use [mulle-sde](//github.com/mulle-sde) to add mulle-sprintf to your project:

``` sh
mulle-sde add github:mulle-core/mulle-sprintf
```

To only add the sources of mulle-sprintf with dependency
sources use [clib](https://github.com/clibs/clib):


``` sh
clib install --out src/mulle-core mulle-core/mulle-sprintf
```

Add `-isystem src/mulle-core` to your `CFLAGS` and compile all the sources that were downloaded with your project.


## Install

### Install with mulle-sde

Use [mulle-sde](//github.com/mulle-sde) to build and install mulle-sprintf and all dependencies:

``` sh
mulle-sde install --prefix /usr/local \
   https://github.com/mulle-core/mulle-sprintf/archive/latest.tar.gz
```

### Manual Installation

Install the requirements:

| Requirements                                 | Description
|----------------------------------------------|-----------------------
| [mulle-buffer](https://github.com/mulle-c/mulle-buffer)             | ↗️ A growable C char array and also a stream
| [mulle-utf](https://github.com/mulle-c/mulle-utf)             | 🔤 UTF8-16-32 analysis and manipulation library
| [mulle-vararg](https://github.com/mulle-c/mulle-vararg)             |  ⏪ Access variable arguments in struct layout fashion in C
| [mulle-thread](https://github.com/mulle-concurrent/mulle-thread)             | 🔠 Cross-platform thread/mutex/tss/atomic operations in C

Install **mulle-sprintf** into `/usr/local` with [cmake](https://cmake.org):

``` sh
cmake -B build \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DCMAKE_PREFIX_PATH=/usr/local \
      -DCMAKE_BUILD_TYPE=Release &&
cmake --build build --config Release &&
cmake --install build --config Release
```

## Author

[Nat!](https://mulle-kybernetik.com/weblog) for Mulle kybernetiK


