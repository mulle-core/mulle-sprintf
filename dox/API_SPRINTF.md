# mulle-sprintf

## Functions

### `mulle_sprintf`

int   mulle_sprintf( struct mulle_buffer *buffer,
                     char *format,
                     ...);

The 'C' version of mulle_sprintf with variable arguments.  `buffer` will be used
to hold the output string. This protects against buffer overflows.

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
for( i = 0; i < 10; i++)
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


## Function Ponters


mulle-sprintf uses a thread local storage system. If want to supply your own
storage mechanism write your functions into :


```
void   *(*mulle_sprintf_get_storage)( struct mulle_allocator *);
void   (*mulle_sprintf_free_storage)( void);
```

It may be useful to clear the thread local cache before running the leak
checker with:

```
mulle_sprintf_free_storage();
```
