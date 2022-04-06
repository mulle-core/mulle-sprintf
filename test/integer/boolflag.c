#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <stdint.h>



int  main( void)
{
   struct mulle_buffer   *buffer;
   int                   len;

   buffer = mulle_buffer_create( NULL);

   len = mulle_buffer_sprintf( buffer, "%bhd", (short) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);

   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "%bllu", (unsigned long long) 0);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_destroy( buffer);
   return( 0);
}

