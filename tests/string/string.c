#include <mulle_sprintf/mulle_sprintf.h>
#include <mulle_test_allocator/mulle_test_allocator.h>
#include <mulle_container/mulle_container.h>

#include <stdio.h>



static void   simple_string_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);


   mulle_sprintf( buffer, "%s\n", "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%.3s\n", "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "\"%.100s\"\n", "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "\"%100s\"\n", "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "\"%100.100s\"\n", "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "\"%.*s\"\n", 100, "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "\"%*s\"\n", 100, "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "\"%*.*s\"\n", 100, 100, "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%s", "");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}


int  main()
{
   mulle_test_allocator_set_tracelevel( 1);
   mulle_test_allocator_initialize();
   mulle_default_allocator = mulle_test_allocator;

   simple_string_test();

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();
}

