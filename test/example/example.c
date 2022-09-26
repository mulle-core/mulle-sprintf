#include <mulle-sprintf/mulle-sprintf.h>
#include <stdio.h>


int   main( void)
{
   struct mulle_buffer   buffer;
   auto char             storage[ 32];

   mulle_buffer_init_inflexible_with_static_bytes( &buffer, storage, sizeof( storage));

   mulle_buffer_sprintf( &buffer, "%d", 1848);
   if( mulle_buffer_get_length( &buffer) == 0)
   {
      fprintf( stderr, "mulle-atinit apparently failed to initialize mulle-sprintf ?\n"
                       "Ensure that all-load mark is set (no-all-load is unset)\n");
      abort();
   }
   printf( "%.*s\n", (int) mulle_buffer_get_length( &buffer), (char *) mulle_buffer_get_bytes( &buffer));
   mulle_buffer_done( &buffer);

   return( 0);
}
