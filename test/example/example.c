#include <mulle-sprintf/mulle-sprintf.h>
#include <stdio.h>


int   main( void)
{
   struct mulle_buffer   buffer;
   auto char             storage[ 32];

   mulle_buffer_init_inflexible_with_static_bytes( &buffer, storage, sizeof( storage));

   mulle_sprintf( &buffer, "%d", 1848);
   printf( "%.*s\n", (int) mulle_buffer_get_length( &buffer), (char *) mulle_buffer_get_bytes( &buffer));
   mulle_buffer_done( &buffer);

   return( 0);
}
