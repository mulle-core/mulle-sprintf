#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <stdint.h>

// https://github.com/vlm/asn1c/issues/159
#if defined(_MSC_VER)
# include <windows.h>
typedef SSIZE_T ssize_t;
#endif



int  main( void)
{
   int   len;

   mulle_buffer_do( buffer)
   {
      len = mulle_buffer_sprintf( buffer, "%#d %#o %#x %#X", 1, 2, 0xa, 0xb);
      if( len)
         printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   }

   return( 0);
}

