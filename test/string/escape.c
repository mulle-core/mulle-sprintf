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

   len = mulle_buffer_sprintf( buffer, "|%%|%03%|% 2%|");
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);

   mulle_buffer_destroy( buffer);

   return( 0);
}
