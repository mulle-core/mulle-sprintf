#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>

// just a copy from snprintf mindlessly changed to sprintf

static void   sprintf_string_test( char *buf, size_t len, char *text)
{
   memset( buf, '*', len);
   sprintf( buf, "%s", text);
   printf( "sprintf: %zd \"%s\"\n", len, text);

   memset( buf, '*', len);
   mulle_sprintf( buf, "%s", text);
   printf( "mulle_sprintf: %zd \"%s\"\n", len, buf);
}



int  main()
{
   char     buf[ 16];
   size_t   i;

   for( i = 0; i < sizeof( buf); i++)
      sprintf_string_test( buf, i, "VfL Bochum 1848");

   return( 0);
}

