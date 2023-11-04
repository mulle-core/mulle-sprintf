#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


int  main()
{
   mulle_buffer_do( buffer)
   {
      mulle_buffer_sprintf( buffer, "%f", 18.48);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%.1f", 18.48);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%3f", 18.48);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);

      mulle_buffer_sprintf( buffer, "%3.1f", 18.48);
      printf( "%.*s\n", (int) mulle_buffer_get_length( buffer), (char *) mulle_buffer_get_bytes( buffer));
      mulle_buffer_reset( buffer);
   }
  return( 0);

}



