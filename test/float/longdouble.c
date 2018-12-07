#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


static void   simple_float_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_sprintf( buffer, "%Lf", (long double) 18.48);
   printf( "%.*s\n", mulle_buffer_get_length( buffer), mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%.1Lf", (long double) 18.48);
   printf( "%.*s\n", mulle_buffer_get_length( buffer), mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%3.1Lf", (long double) 1848);
   printf( "%.*s\n", mulle_buffer_get_length( buffer), mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}



int  main()
{
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   simple_float_test();

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();
}

