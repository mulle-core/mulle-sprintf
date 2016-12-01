# mulle-sprintf

## Functions

### `mulle_sprintf`

int   mulle_sprintf( struct mulle_buffer *buffer,
                     char *format,
                     ...);

The 'C' version of mulle_sprintf with variable arguments.  `buffer` will be used
to hold the output string. This protects against buffer overflows. The contents
of `buffer` will not be zero terminated. This allows you to easily to the
buffer with maybe another `mulle_sprintf` call. See the example how to properly
treat the buffer contents.

Example
```


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

mulle_sprintf( &buffer, "%s %s %d", "VfL", "Bochum", 1848);
// either use length to access the contents
printf( "%.*s\n", mulle_buffer_get_length( &buffer), mulle_buffer_get_bytes( &buffer));

// or explicitly zero terminate
mulle_buffer_add_byte( &buffer, 0);
printf( "%s\n", mulle_buffer_get_bytes( &buffer));

mulle_buffer_done( &buffer);
```


### `mulle_vsprintf`

```
int   mulle_vsprintf( struct mulle_buffer *buffer,
                      char *format,
                      va_list va);
```

The `<stdarg.h> va_list`  variant of `mulle_sprintf`.


### `mulle_mvsprintf`

```
int   mulle_mvsprintf( struct mulle_buffer *buffer,
                       char *format,
                       mulle_vararg_list va);
```

The `<mulle_vararg/mulle_vararh.h> mulle_vararg_list` variant of `mulle_sprintf`.


### `mulle_sprintf_free_storage`

```
mulle_sprintf_free_storage();
```

mulle-sprintf uses a thread local storage system to maintain a memory cache.
It may be useful to clear the thread local cache before running the leak
checker with `mulle_sprintf_free_storage`.



## Extending mulle-sprintf


Vou can augment the available "conversion specifiers"
like `%d` or `%s` with your own characters. This can be
used by an Objective-C Foundation for the `%@` conversion for instance.

Here is an example that registers a conversion rotine for a BOOL
type (based on `int`) to be used as '%b', that outputs "YES" or "NO"


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
```

