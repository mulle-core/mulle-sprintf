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

      // Test %g format (automatic %e or %f selection)
      // %g in sprintf (C99/POSIX):
      // - Chooses automatically between %f and %e (or %F/%E) whichever is shorter/more compact.
      // - Removes trailing zeros after decimal point.
      // - If exponent would be < -4 or >= precision (default 6), uses scientific %e; else uses fixed %f.
      // - Precision = number of significant digits (not decimal places).
      // - Always keeps at least 1 digit left of decimal (no .000001 -> 1e-6 unless forced).
      // - Trailing decimal point is removed (e.g., 1 not 1.).
      // - Special case: if result is exactly 0, prints 0 (not 0e+00 unless # flag).
      // Flags behavior:
      // - # forces decimal point and keeps trailing zeros in fixed mode.
      // - Never prints trailing zeros in scientific mode.
      mulle_buffer_sprintf( buffer, "%g", 123.456);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 0.00123);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 1234567.89);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 0.0001);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 0.000001);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%.2g", 123.456);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%.4g", 0.00123456);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 1.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 10.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 100.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 1000000.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 0.1);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 0.01);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 0.001);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test # flag (keeps trailing zeros and decimal point)
      mulle_buffer_sprintf( buffer, "%#g", 1.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%#g", 100.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test exponent threshold (exponent < -4 uses %e)
      mulle_buffer_sprintf( buffer, "%g", 0.00001);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test exponent threshold (exponent >= precision uses %e, default precision=6)
      mulle_buffer_sprintf( buffer, "%g", 999999.0);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%g", 1000000.1);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      // Test zero
      mulle_buffer_sprintf( buffer, "%g", 0.0);
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
