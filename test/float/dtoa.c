#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


int  main()
{
   mulle_buffer_do( buffer)
   {
      // Test %e format
      mulle_buffer_sprintf( buffer, "%e", 3.14159);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%.2e", 3.14159);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test %f format
      mulle_buffer_sprintf( buffer, "%f", 123.456);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%.2f", 123.456);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test %g format
      mulle_buffer_sprintf( buffer, "%g", 123.456);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 0.00123);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test %a format
      mulle_buffer_sprintf( buffer, "%a", 1.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test special values
      mulle_buffer_sprintf( buffer, "%f", 0.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%e", 1.0/0.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%f", 0.0/0.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);
   }
   return( 0);
}
