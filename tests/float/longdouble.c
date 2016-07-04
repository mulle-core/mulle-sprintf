#include <mulle_sprintf/mulle_sprintf.h>
#include <mulle_test_allocator/mulle_test_allocator.h>

#include <stdio.h>


static void   simple_float_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_sprintf( buffer, "%Lf", (long double) 18.48);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%.1Lf", (long double) 18.48);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%3.1Lf", (long double) 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
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

