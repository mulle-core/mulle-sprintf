#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>


static void   simple_float_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_buffer_sprintf( buffer, "%Lf", (long double) 18.48);
   printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "%.1Lf", (long double) 18.48);
   printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "%3.1Lf", (long double) 1848);
   printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}



int  main()
{

   mulle_sprintf_free_storage();

   simple_float_test();

   mulle_sprintf_free_storage();

   return( 0);
}

