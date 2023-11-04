#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <stdint.h>

// https://github.com/vlm/asn1c/issues/159
#if defined(_MSC_VER)
# include <windows.h>
typedef SSIZE_T ssize_t;
#endif



int  main( void)
{
   struct mulle_buffer   *buffer;
   int                   len;

   buffer = mulle_buffer_create( NULL);

   len = mulle_buffer_sprintf( buffer, "|%c|%lc|%03c|% c|% 2c|%C|%hC|%lC|",
                                        'c', 'l', 'x', 'y', 'z',
                                        0x1F642, 0x2117, 0x1F61C);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);

   mulle_buffer_destroy( buffer);

   return( 0);
}

