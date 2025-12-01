# mulle-sprintf

## Functions

### Buffer-based Functions

#### `mulle_buffer_sprintf`

```
int   mulle_buffer_sprintf( struct mulle_buffer *buffer,
                            char *format,
                            ...);
```

The 'C' version of mulle_sprintf with variable arguments. `buffer` will be used
to hold the output string. This protects against buffer overflows. The contents
of `buffer` will not be zero terminated. This allows you to easily to the
buffer with maybe another `mulle_buffer_sprintf` call. 

Returns the length of the produced buffer in bytes.
Returns -1 on failure and sets `errno` to

Errorcode  | Description
-----------|----------------------
EDOM       | Format is wrong
ENOMEM     | Supplied buffer is too small
EINVAL     | Parameters are wrong (NULL ?)

Example:

```
struct mulle_buffer   buffer;

mulle_buffer_init( &buffer, NULL);

mulle_buffer_sprintf( &buffer, "%s %s %d", "VfL", "Bochum", 1848);
// either use length to access the contents
printf( "%.*s\n", mulle_buffer_get_length( &buffer), mulle_buffer_get_bytes( &buffer));

// or explicitly zero terminate
mulle_buffer_add_byte( &buffer, 0);
printf( "%s\n", mulle_buffer_get_bytes( &buffer));

mulle_buffer_done( &buffer);
```

#### `mulle_buffer_vsprintf`

```
int   mulle_buffer_vsprintf( struct mulle_buffer *buffer,
                             char *format,
                             va_list va);
```

The `<stdarg.h>` `va_list` variant of `mulle_buffer_sprintf`.

#### `mulle_buffer_mvsprintf`

```
int   mulle_buffer_mvsprintf( struct mulle_buffer *buffer,
                              char *format,
                              mulle_vararg_list va);
```

The `<mulle_vararg/mulle_vararh.h>` `mulle_vararg_list` variant of `mulle_buffer_sprintf`.

### Fixed-size Buffer Functions

#### `mulle_snprintf`

```
int   mulle_snprintf( char *buf, size_t size, char *format, ...);
```

Safe sprintf variant that writes to a fixed-size buffer. Always appends a null
terminator even if the buffer overflows. Returns -1 on overflow with `errno`
set to ENOMEM.

Example:
```
char buf[ 32];
mulle_snprintf( buf, sizeof( buf), "%d", 1848);
```

#### `mulle_vsnprintf`

```
int   mulle_vsnprintf( char *buf, size_t size, char *format, va_list va);
```

The `<stdarg.h>` `va_list` variant of `mulle_snprintf`.

#### `mulle_mvsnprintf`

```
int   mulle_mvsnprintf( char *buf, size_t size, char *format, mulle_vararg_list arguments);
```

The `<mulle_vararg/mulle_vararh.h>` `mulle_vararg_list` variant of `mulle_snprintf`.

### Unsafe Buffer Functions

#### `mulle_sprintf`

```
int   mulle_sprintf( char *buf, char *format, ...);
```

Unsafe sprintf variant - use `mulle_snprintf` or `mulle_buffer_sprintf` instead.

#### `mulle_vsprintf`

```
int   mulle_vsprintf( char *buf, char *format, va_list va);
```

The `<stdarg.h>` `va_list` variant of `mulle_sprintf`.

#### `mulle_mvsprintf`

```
int   mulle_mvsprintf( char *buf, char *format, mulle_vararg_list arguments);
```

The `<mulle_vararg/mulle_vararh.h>` `mulle_vararg_list` variant of `mulle_sprintf`.

### Dynamic Allocation Functions

#### `mulle_asprintf`

```
int   mulle_asprintf( char **strp, char *format, ...);
```

Allocates a string using the mulle_default_allocator. The caller must free the
result with `mulle_free()`. Returns -1 on failure.

Example:
```
char *str;
mulle_asprintf( &str, "%s %d", "VfL", 1848);
// use str...
mulle_free( str);
```

#### `mulle_vasprintf`

```
int   mulle_vasprintf( char **strp, char *format, va_list ap);
```

The `<stdarg.h>` `va_list` variant of `mulle_asprintf`.

#### `mulle_mvasprintf`

```
int   mulle_mvasprintf( char **strp, char *format, mulle_vararg_list arguments);
```

The `<mulle_vararg/mulle_vararh.h>` `mulle_vararg_list` variant of `mulle_asprintf`.

### Custom Allocator Functions

#### `mulle_allocator_asprintf`

```
int   mulle_allocator_asprintf( struct mulle_allocator *allocator,
                                char **strp,
                                char *format, ...);
```

Allocates a string using the specified allocator. Returns -1 on failure.

#### `mulle_allocator_vasprintf`

```
int   mulle_allocator_vasprintf( struct mulle_allocator *allocator,
                                 char **strp,
                                 char *format,
                                 va_list ap);
```

The `<stdarg.h>` `va_list` variant of `mulle_allocator_asprintf`.

#### `mulle_allocator_mvasprintf`

```
int   mulle_allocator_mvasprintf( struct mulle_allocator *allocator,
                                  char **strp,
                                  char *format,
                                  mulle_vararg_list arguments);
```

The `<mulle_vararg/mulle_vararh.h>` `mulle_vararg_list` variant of `mulle_allocator_asprintf`.

### Legacy Buffer Functions

#### `mulle_sprintf` (legacy)

```
int   mulle_sprintf( struct mulle_buffer *buffer,
                     char *format,
                     ...);
```

The 'C' version of mulle_sprintf with variable arguments. **Note**: This is the
legacy signature that uses a buffer. The newer unsafe variant uses `char *buf`
directly.

Returns the length of the produced buffer in bytes.
Returns -1 on failure and sets `errno` to

Errorcode  | Description
-----------|----------------------
EDOM       | Format is wrong
ENOMEM     | Supplied buffer is too small
EINVAL     | Parameters are wrong (NULL ?)

#### `mulle_vsprintf` (legacy)

```
int   mulle_vsprintf( struct mulle_buffer *buffer,
                      char *format,
                      va_list va);
```

The `<stdarg.h>` `va_list` variant of the legacy `mulle_sprintf`.

#### `mulle_mvsprintf` (legacy)

```
int   mulle_mvsprintf( struct mulle_buffer *buffer,
                       char *format,
                       mulle_vararg_list va);
```

The `<mulle_vararg/mulle_vararh.h>` `mulle_vararg_list` variant of the legacy `mulle_sprintf`.

### Storage Management

#### `mulle_sprintf_free_storage`

```
mulle_sprintf_free_storage();
```

**mulle-sprintf** uses a thread local storage system to maintain a memory cache.
It may be useful to clear the thread local cache before running the leak
checker with `mulle_sprintf_free_storage`.

## Extending mulle-sprintf

Vou can augment the available "conversion specifiers"
like `%d` or `%s` with your own characters. This can be
used by an Objective-C Foundation for the `%@` conversion for instance.

Here is an example that of a conversion routine for `%b`, to be used as a BOOL
type (based on `int`) to output "YES" or "NO":


```
static mulle_sprintf_argumenttype_t  get_bool_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   return( mulle_sprintf_int_argumenttype);
}


static int   _bool_conversion( struct mulle_buffer *buffer,
                               struct mulle_sprintf_formatconversioninfo *info,
                               struct mulle_sprintf_argumentarray *arguments,
                               int argc)
{
   union mulle_sprintf_argumentvalue   v;

   v = arguments->values[ argc];

   mulle_buffer_add_string( buffer, v.i ? "YES" : "NO");
   return( 0);
}



static struct mulle_sprintf_function   bool_functions =
{
   get_bool_argumenttype,
   _bool_conversion
};


void  register_bool_sprintf_functions( void)
{
   mulle_sprintf_register_functions( NULL, &bool_functions, 'b');
}
