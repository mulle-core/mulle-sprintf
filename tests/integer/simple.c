#include <mulle_standalone_sprintf/mulle_standalone_sprintf.h>
#include <mulle_test_allocator/mulle_test_allocator.h>

#include <stdio.h>




static void   simple_int_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_sprintf( buffer, "%d", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%o", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%x", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_destroy( buffer);
}


static void   simple_long_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_sprintf( buffer, "%ld", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%lo", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%lx", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n", 1L, 2L, 3L, 4L, 5L, 6L, 7L, 8L, 9L, 10L, 11L, 12L, 13L, 14L, 15L, 16L, 17L, 18L, 19L, 20L, 21L, 22L, 23L, 24L);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo %lo\n", 1L, 2L, 3L, 4L, 5L, 6L, 7L, 8L, 9L, 10L, 11L, 12L, 13L, 14L, 15L, 16L, 17L, 18L, 19L, 20L, 21L, 22L, 23L, 24L);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx %lx\n", 1L, 2L, 3L, 4L, 5L, 6L, 7L, 8L, 9L, 10L, 11L, 12L, 13L, 14L, 15L, 16L, 17L, 18L, 19L, 20L, 21L, 22L, 23L, 24L);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_destroy( buffer);
}


static void   simple_width_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_sprintf( buffer, "%+10d", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "% 10o", 1848);
   printf( "%s\n", mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_sprintf( buffer, "%010x", 1848);
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

   simple_int_test();

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();

   simple_long_test();

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();

   simple_width_test();

   mulle_sprintf_free_storage();
   mulle_test_allocator_reset();
}

