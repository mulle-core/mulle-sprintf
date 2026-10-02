#include <mulle-sprintf/mulle-sprintf.h>
#include <stdio.h>



static void   printf_complex_width_tests()
{
// mixed: positional and sequential is undefined actually by the standard
//   printf( "printf: |%2$*1$s/%.*s|\n", 3, "VfL Bochum 1848", 5, "VfL Bochum 1848");
   printf( "printf: |%*.*s/%*.*s|\n", 3, 5, "VfL Bochum 1848", 5, 3, "VfL Bochum 1848");
   printf( "printf: |%*s/%.*s|\n", 3, "VfL Bochum 1848", 5, "VfL Bochum 1848");
}


static void   complex_width_tests()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_buffer_sprintf( buffer, "|%2$*1$s/%.*s|\n", 3, "VfL Bochum 1848", 5, "VfL Bochum 1848");
   printf( "mulle_sprintf: %.*s", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "|%*.*s/%*.*s|\n", 3, 5, "VfL Bochum 1848", 5, 3, "VfL Bochum 1848");
   printf( "mulle_sprintf: %.*s", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "|%*s/%.*s|\n", 3, "VfL Bochum 1848", 5, "VfL Bochum 1848");
   printf( "mulle_sprintf: %.*s", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));

   mulle_buffer_destroy( buffer);
}


int  main()
{

   complex_width_tests();
   printf_complex_width_tests();

   mulle_sprintf_free_storage();

   return( 0);
}

