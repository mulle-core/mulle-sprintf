### 3.2.1

* fix `mulle_snprintf` in truncation case

## 3.2.0

* unified printing of nan across platforms


### 3.1.3

* fix return (%n) bugs for 32 bit arch

### 3.1.2

* Various small improvements

### 3.1.1

* fix for cosmopolitan

## 3.1.0

* mulle-sprintf now supports the alternate flag for %s, therefore %#s will output C escaped text instead without the double quotes
* Added more tests, only some `wchar_t` branch and some obscure error conditions not hit are outside of the coverage
* remove deprecated and never used O D U conversion functions, unless ``HAVE_DEPRECATED_INT_LONG_CONVERSIONS`` is defined
* fix X conversion to properly output uppercase
* fix %n conversion for unsigned char * and unsigned short *
* you can now override already registered conversion characters, if you think you an do a better job
* fix for positional parameters
* `mulle_utf8_t` is no more, it is now just char
* added UTF16 and UTF32 output modifiers


# 3.0.0

* remove package.json as it conflicts with clib.json
* for purposes of the amalgamated mulle-core, mulle-sprintf no longer uses constructor calls to piece internal things together


## 2.2.0

* added `mulle_allocator_asprintf` and variants


## 2.1.0

* do not add trailing \0 byte in vsprintf functions
* prevent crash for unknown modifiers
* add b flag to integer conversions to output BOOL strings YES/NO (e.g. printf( "%bd", YES));


# 2.0.0

* provides the full list of sprintf like functions without `<stdio.h>` now (stdio is in `mulle_fprintf`)
* unknown conversion characters are now an error and not silently ignored


### 1.0.20

* Various small improvements

### 1.0.19

* move from travis to github actions, upgrade mulle-sde

### 1.0.18

* new mulle-sde project structure

### 1.0.17

* fix missing function

### 1.0.16

* updated mulle-sde and travis

### 1.0.15

* improved `find_library` code

### 1.0.14

* fix some test compile warnings

### 1.0.13

* modernized to mulle-sde with .mulle folder

### 1.0.12

* fix travis

### 1.0.11

* migrated to mulle-test version 4.0.0

### 1.0.10

* fix outdated and buggy sde

### 1.0.9

* adapt to mulle-c11 3.0.0 and fix `PTR_DIFF` problem on windows

### 1.0.8

* fix mingw, update sde

### 1.0.7

* modernized mulle-sde

### 1.0.6

* Various small improvements

### 1.0.5

* Various small improvements

### 1.0.4

* Various small improvements

### 1.0.3

* Various small improvements

### 1.0.2

* Various small improvements

### 1.0.1

* fix travis.yml

# 1.0.0

* migrated to mulle-sde
* made headernames hyphenated
* no longer distributed as a homebrew package

### 0.9.11

* Various small improvements

### 0.9.9

* support new mulle-tests

### 0.9.7

* fixed scion wrapper command

### 0.9.5

* follow mulle-configuration 3.1 changes and move .travis.yml to trusty

### 0.9.3

* modernized to mulle-configuration 3.0

### 0.8.5

* make it a cmake "C" project

### 0.8.3

* modernize mulle-homebrew script

## 0.8.1

### 0.7.11

* fix dox, add example

### 0.7.9

* don't troll stdlib in tests

### 0.7.7

* fix tests to use mulle_buffer

### 0.7.5

* fix general bug that manifested on linux

### 0.7.3

* improve documentation

## 0.7.1

* do not append a '\0' any more for the sake of flushable buffers

## 0.6
  * fixed an offset bug

## 0.5
   * added mulle_utf to dependencies
   * wrote the wide character routines

## 0.4
   * fix bug when no conversion is needed

## 0.3
   * fix FP output

## 0.2
   * added versioning
