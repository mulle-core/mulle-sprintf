#include <mulle_sprintf/mulle_sprintf.h>
#include <mulle_test_allocator/mulle_test_allocator.h>

#include <stdio.h>


static void   simple_none_test()
{
   struct mulle_buffer   *buffer;
   va_list               va;
   mulle_vararg_list     args;
   int                   len;

   buffer = mulle_buffer_create( NULL);

   len = mulle_sprintf( buffer, "none");
   printf( "%s (%d)\n", mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_vsprintf( buffer, "none", va);
   printf( "%s (%d)\n", mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_mvsprintf( buffer, "none", args);
   printf( "%s (%d)\n", mulle_buffer_get_bytes( buffer), len);

   mulle_buffer_destroy( buffer);
}


int  main()
{
   mulle_test_allocator_set_tracelevel( 1);
   mulle_test_allocator_initialize();
   mulle_default_allocator = mulle_test_allocator;

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();

   simple_none_test();

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();
}

