#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>
#include <math.h>

static void   print( struct mulle_buffer *buffer, char *format, double value)
{
   mulle_buffer_sprintf( buffer, format, value);
   printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);
}

int  main()
{
   mulle_buffer_do( buffer)
   {
      // width / padding / left-justify / zero-pad for %e %f %g
      print( buffer, "[%12.2e]", 1.0);
      print( buffer, "[%012.2e]", 1.0);
      print( buffer, "[%+012.2e]", 1.0);
      print( buffer, "[%-012.2e]", 1.0);
      print( buffer, "[%012.3f]", -1234.5);
      print( buffer, "[%+012.3f]", 1234.5);
      print( buffer, "[%012g]", 0.00012345);
      print( buffer, "[% 012.2e]", 1.0);
      print( buffer, "[%020e]", INFINITY);
      print( buffer, "[%+020e]", INFINITY);
      print( buffer, "[%020f]", NAN);
      print( buffer, "[%020a]", 1.0);
      print( buffer, "[%+020a]", 1.0);

      // zero formatting
      print( buffer, "[%e]", 0.0);
      print( buffer, "[%E]", 0.0);
      print( buffer, "[%.0e]", 0.0);
      print( buffer, "[%#.0e]", 0.0);
      print( buffer, "[%e]", -0.0);
      print( buffer, "[%g]", 0.0);
      print( buffer, "[%#g]", 0.0);
      print( buffer, "[%#.0g]", 0.0);
      print( buffer, "[%020.1e]", 0.0);
      print( buffer, "[%020.2f]", 0.0);

      // round half to even
      print( buffer, "[%.0f]", 2.5);
      print( buffer, "[%.0f]", 3.5);
      print( buffer, "[%.0f]", 1234.5);
      print( buffer, "[%.3e]", 1234.5);
      print( buffer, "[%.0g]", 2.5);

      // %g trailing-zero removal must keep integer zeros
      print( buffer, "[%g]", 100000.0);
      print( buffer, "[%020g]", 100000.0);
   }
   return( 0);
}
