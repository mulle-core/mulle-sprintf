#include <mulle_standalone_sprintf/mulle_standalone_sprintf.h>
#include <mulle_test_allocator/mulle_test_allocator.h>
#include <mulle_container/mulle_container.h>

#include <stdio.h>



static void   complex_width_tests()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_sprintf( buffer, "%2$*1$s/%.*s\n", 3, "VfL Bochum 1848", 5, "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));

   mulle_sprintf( buffer, "%*.*s/%*.*s\n", 3, 5, "VfL Bochum 1848", 5, 3, "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%*s/%.*s\n", 3, "VfL Bochum 1848", 5, "VfL Bochum 1848");
   printf( "%s", mulle_buffer_get_bytes( buffer));

   mulle_buffer_destroy( buffer);
}

int  main()
{
   mulle_test_allocator_set_tracelevel( 1);
   mulle_test_allocator_initialize();
   mulle_default_allocator = mulle_test_allocator;

   complex_width_tests();

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();
}

