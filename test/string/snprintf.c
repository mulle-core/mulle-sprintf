#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>
#include <errno.h>



static void   snprintf_string_test( char *buf, size_t len, char *text)
{
   int   rval;

   errno = 0;
   rval = snprintf( buf, len, "%s", text);
   printf( "snprintf       : %zd \"%s\" (%d-%d)\n", len, buf, rval, errno);
}


static void   mulle_snprintf_string_test( char *buf, size_t len, char *text)
{
   int   rval;

   errno = 0;
   rval  = mulle_snprintf( buf, len, "%s", text);
   printf( "mulle_snprintf : %zd \"%s\" (%d-%d)\n", len, buf, rval, errno);
}



int  main()
{
   char     buf[ 18];
   size_t   i;

   for( i = sizeof( buf); i;)
   {
      --i;

      memset( buf, '*', sizeof( buf) - 1);
      buf[ 15] = 0;
      snprintf_string_test( buf, i, "VfL Bochum 1848");

      memset( buf, '*', sizeof( buf) - 1);
      buf[ 15] = 0;
      mulle_snprintf_string_test( buf, i, "VfL Bochum 1848");
   }

   return( 0);
}

