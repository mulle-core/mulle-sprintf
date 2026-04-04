# mulle-sprintf Library Documentation for AI
<!-- Keywords: sprintf, formatting, vararg, buffer, asprintf, utf, extensible -->
## 1. Introduction & Purpose

- mulle-sprintf is an extensible, stdlib-compatible sprintf replacement written in C. It supports both C varargs (va_list) and mulle_vararg style arguments, custom conversion registration, and UTF8/16/32 string handling.
- Solves: portable, extensible formatting (including Objective-C style %@ / BOOL as YES/NO via registration) and safe allocation variants (asprintf, allocator-backed asprintf, buffer-based printing).
- Key features: mulle_snprintf/mulle_sprintf/mulle_asprintf, buffer-oriented printing, conversion-table extensibility, optional mulle-dtoa for FP.

## 2. Key Concepts & Design Philosophy

- Extensible conversion table: conversion characters map to function vectors and modifier tables (struct mulle_sprintf_conversion).
- Argument handling: parses format into mulle_sprintf_formatconversioninfo, then fills an argument array (union mulle_sprintf_argumentvalue) to decouple parsing from conversion.
- Safety/allocators: fixed-buffer helpers use an internal mulle_buffer; asprintf variants let caller choose the allocator.
- Thread-awareness: configuration holds a TSS key for per-thread storage; registration of global defaults happens via provided register-on-load helpers.

## 3. Core API & Data Structures

This section highlights the public headers and the main symbols an AI should reference.

### 3.1. src/mulle-sprintf.h

#### Overview
- Primary entry points: buffer-based printing, fixed-buffer convenience wrappers, asprintf/allocator-asprintf variants, and helper macros.

#### Important symbols
- Buffer APIs
  - int mulle_buffer_sprintf(struct mulle_buffer *buffer, char *format, ...);
  - int mulle_buffer_vsprintf(struct mulle_buffer *buffer, char *format, va_list va);
  - int _mulle_buffer_vsprintf(struct mulle_buffer *buffer, char *format, va_list va, struct mulle_sprintf_conversion *table);
  - int mulle_buffer_mvsprintf(struct mulle_buffer *buffer, char *format, mulle_vararg_list va);
  - int _mulle_buffer_mvsprintf(..., struct mulle_sprintf_conversion *table);

- Fixed-buffer / snprintf family
  - int mulle_snprintf(char *buf, size_t size, char *format, ...);
  - int mulle_vsnprintf(...);
  - int mulle_mvsnprintf(...);
  - Note: these return -1 on overflow and always append a '\0'.

- Unsafe convenience
  - int mulle_sprintf(char *buf, char *format, ...);
  - static inline int mulle_vsprintf(char *buf, char *format, va_list va);

- asprintf family
  - int mulle_asprintf(char **strp, char *format, ...);
  - int mulle_vasprintf(char **strp, char *format, va_list ap);
  - int mulle_mvasprintf(char **strp, char *format, mulle_vararg_list arguments);
  - allocator variants: mulle_allocator_asprintf(...)
  - Strings are allocated with the chosen allocator; default requires mulle_free to free.

- Configuration & helpers
  - struct mulle_sprintf_config (tss key, storage callbacks, defaultconversion)
  - mulle_sprintf_get_config(), mulle_sprintf_get_defaultconversion(), mulle_sprintf_free_storage()
  - Macro: mulle_sprintf_do(string, format, ...) — creates a temporary buffer-scoped string for the enclosing block.

### 3.2. src/mulle-sprintf-function.h

#### Key data structures
- struct mulle_sprintf_formatconversionflags
  - Bitflags parsed from the format (zero_found, minus_found, space_found, plus_found, hash_found, bool_found, width_found, precision_found, etc.).

- struct mulle_sprintf_formatconversioninfo
  - Parsed conversion: width, precision, argv_index[], modifier[], conversion char, separator, argument index info.

- union mulle_sprintf_argumentvalue
  - Union of all supported argument representations (int, char, char*, double, intmax_t, long, long double, ptrdiff_t, object pointer, size_t, pointers, wchar_t*, etc.).

- struct mulle_sprintf_argumentarray
  - values + types + size. Helpers fill these from va_list or mulle_vararg_list via:
    - mulle_mvsprintf_set_values(..., mulle_vararg_list va)
    - mulle_vsprintf_set_values(..., va_list va)

- struct mulle_sprintf_function
  - determine_argument_type(info) -> argument type
  - convert_argument(buffer, info, arguments, i) -> performs conversion into buffer

- struct mulle_sprintf_conversion
  - jumps (vector indexed by printable ascii), modifiers table — the central conversion dispatch table.

#### Registration API
- mulle_sprintf_register_functions(table, functions, c)
- mulle_sprintf_register_default_functions(functions, c)
- mulle_sprintf_register_modifier(table, c)
- mulle_sprintf_register_modifiers(table, s)
- mulle_sprintf_register_default_modifier(c)
- mulle_sprintf_register_default_modifiers(s)
- mulle_sprintf_register_standardmodifiers(table)
- Debug: _mulle_sprintf_dump_available_conversion_characters(table), _mulle_sprintf_dump_available_defaultconversion_characters()

### 3.3. src/mulle-sscanf.h

- int mulle_sscanf(char const *str, char const *format, ...);
- int mulle_vsscanf(char const *str, char const *format, va_list args);
- sscanf-like parsing implemented to match the library's formatting conventions.

## 4. Performance Characteristics

- Output generation: O(n) in length of produced output.
- Format parsing: O(m) in length of format string; parsing is done once per formatted call.
- Dispatch: O(1) per conversion character via an indexed vector table.
- Memory: buffer-based APIs minimize reallocs (amortized O(1) append); asprintf does O(n) allocation.
- Thread-safety: per-thread storage exists (TSS), but registering/modifying global conversion tables at runtime is not inherently thread-safe — perform registration at startup.

## 5. AI Usage Recommendations & Patterns

- Best practices
  - Use mulle_buffer_sprintf for efficient, repeated or streaming formatting.
  - Use mulle_snprintf for bounded, stack-allocated buffers; check for negative return value (overflow).
  - Use mulle_asprintf when a heap-allocated string is needed; free with the matching allocator (mulle_free for default allocator).
  - For custom format specifiers, register conversion functions at init time via mulle_sprintf_register_functions and modifiers via mulle_sprintf_register_modifier.
  - Prefer the provided setter helpers (mulle_vsprintf_set_values / mulle_mvsprintf_set_values) to populate argument arrays.

- Common pitfalls
  - Do not access internal union or conversion vectors directly; use public API.
  - Beware that floating point formatting may use system sprintf unless mulle-dtoa is enabled; platform differences can appear in NaN/Inf formatting.
  - mulle_buffer_sprintf does not append '\0' to the buffer — use buffer string helpers when a C-string is needed.

## 6. Integration Examples

Style: 3-space indent, Allman braces, C89 var rules (one declaration per line), return( expr );

### Example 1: Creating and using a fixed buffer

```c
#include "mulle-sprintf.h"
#include <stdio.h>

int
main()
{
   char  buf[ 32];

   mulle_snprintf( buf, sizeof( buf), "%s %d", "items:", 42);
   puts( buf);
   return( 0);
}
```

### Example 2: Allocate formatted string and free with mulle_free

```c
#include "mulle-sprintf.h"

int
example_asprintf( void)
{
   char  *s;

   if( mulle_asprintf( &s, "%s %d", "VfL", 1848) == 0)
   {
      puts( s);
      mulle_free( s);
      return( 0);
   }
   return( -1);
}
```

### Example 3: Registering a custom conversion (skeleton)

```c
#include "mulle-sprintf-function.h"

static mulle_sprintf_argumenttype_t
my_determine_type( struct mulle_sprintf_formatconversioninfo *info)
{
   (void) info;
   return( mulle_sprintf_void_pointer_argumenttype);
}

static int
my_convert( struct mulle_buffer *buffer,
            struct mulle_sprintf_formatconversioninfo *info,
            struct mulle_sprintf_argumentarray *arguments,
            int i)
{
   (void) buffer; (void) info; (void) arguments; (void) i;
   /* implement conversion, append to buffer */
   return( 0);
}

void
register_my_conversion( struct mulle_sprintf_conversion *table)
{
   struct mulle_sprintf_function  fns[ 1];

   fns[ 0].determine_argument_type = my_determine_type;
   fns[ 0].convert_argument       = my_convert;

   mulle_sprintf_register_functions( table, fns, (mulle_sprintf_conversioncharacter_t) 'z');
}
```

## 7. Dependencies

- mulle-buffer
- mulle-utf
- mulle-vararg
- mulle-allocator (for allocator-backed asprintf)
- Optional: mulle-dtoa / mulle-dtostr for improved floating-point formatting

---

Notes for an AI assistant: prefer the headers in src/ as authoritative. Use test/ and asset/dox examples for usage patterns and edge cases. When generating code snippets, follow the style rules above and favour buffer or allocator variants for safer memory semantics.
