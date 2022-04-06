#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void   printf_string_test()
{
   printf( "p: |%s|\n", "VfL Bochum 1848");
   printf( "p: |%.3s|\n", "VfL Bochum 1848");
   printf( "p: |\"%.100s\"|\n", "VfL Bochum 1848");
   printf( "p: |\"%100s\"|\n", "VfL Bochum 1848");
   printf( "p: |\"%100.100s\"|\n", "VfL Bochum 1848");
   printf( "p: |\"%.*s\"|\n", 100, "VfL Bochum 1848");
   printf( "p: |\"%*s\"|\n", 100, "VfL Bochum 1848");
   printf( "p: |\"%*.*s\"|\n", 100, 100, "VfL Bochum 1848");
   printf( "p: |%s|\n", "");
}


static void   simple_string_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_buffer_sprintf( buffer, "m: |%s|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |%.3s|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%.100s\"|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%100s\"|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%100.100s\"|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%.*s\"|\n", 100, "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%*s\"|\n", 100, "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%*.*s\"|\n", 100, 100, "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |%s|\n", "");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}


int  main()
{
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   simple_string_test();
   printf_string_test();

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   return( 0);
}

