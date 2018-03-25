#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-test-allocator/mulle-test-allocator.h>

#include <stdio.h>


static void   simple_sprintf_free_test()
{
   struct mulle_buffer   buffer;
   int                   len;

   mulle_buffer_init( &buffer, NULL);

   fprintf( stderr, " -- before sprintf\n");

   len = mulle_sprintf( &buffer, "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n"
                                 "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n"
                                 "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n"
                                 "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n"
                                 "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n"
                                 "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n"
                                 "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n"
                                 "%o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o %o\n",
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24);
   printf( "%.*s (%d)\n", len, mulle_buffer_get_bytes( &buffer), len);

   fprintf( stderr, " -- buffer done\n");
   mulle_buffer_done( &buffer);
}


int  main()
{
   mulle_test_allocator_set_tracelevel( 1);
   mulle_test_allocator_initialize();
   mulle_default_allocator = mulle_test_allocator;

   simple_sprintf_free_test();

   fprintf( stderr, " -- free storage\n");
   mulle_sprintf_free_storage();

   fprintf( stderr, " -- allocator reset\n");
   mulle_test_allocator_reset();
   return( 0);
}

