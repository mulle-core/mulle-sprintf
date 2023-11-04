#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


int  main()
{
   mulle_buffer_do( buffer)
   {
       int     i = 1,
               j = 2,
               k = 3;
       double  x = 0.1,
               y = 2.22,
               z = 333.3333;
       char    *s1 = "abc",
               *s2 = "def",
               *s3 = "ghi";

      // taken from: https://learn.microsoft.com/en-us/cpp/c-runtime-library/printf-p-positional-parameters?view=msvc-170
      // If positional arguments are unspecified,
      // normal input order is used.
      mulle_buffer_sprintf( buffer, "%d %d %d", i, j, k);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Positional arguments are numbers followed by a $ character.
      mulle_buffer_sprintf( buffer, "%3$d %1$d %2$d", i, j, k);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // The same positional argument may be reused.
      mulle_buffer_sprintf( buffer, "%1$d %2$d %1$d", i, j);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // The positional arguments may appear in any order.
      mulle_buffer_sprintf( buffer, "%1$s %2$s %3$s", s1, s2, s3);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%3$s %1$s %2$s", s1, s2, s3);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Precision and width specifiers must be int types.
      mulle_buffer_sprintf( buffer, "%3$*5$f %2$.*4$f %1$*4$.*5$f", x, y, z, j, k);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
   }
   return( 0);
}



