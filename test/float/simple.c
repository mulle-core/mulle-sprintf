#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-test-allocator/mulle-test-allocator.h>

#include <stdio.h>


static void   simple_float_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_sprintf( buffer, "%f", 18.48);
   printf( "%.*s\n", mulle_buffer_get_length( buffer), mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%.1f", 18.48);
   printf( "%.*s\n", mulle_buffer_get_length( buffer), mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%3.1f", 18.48);
   printf( "%.*s\n", mulle_buffer_get_length( buffer), mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}



int  main()
{
   mulle_test_allocator_set_tracelevel( 1);
   mulle_test_allocator_initialize();
   mulle_default_allocator = mulle_test_allocator;

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();

   simple_float_test();

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();
}

