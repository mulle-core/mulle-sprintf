#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


int  main( void)
{
   char   buf[ 14];
   int    rval;

   rval = mulle_snprintf( buf, sizeof( buf), "VfL Bochum 1848");
   if( rval == -1 && errno == ENOMEM)
   {
      printf( "%s", buf);
      return( 0);
   }

   rval = mulle_snprintf( buf, sizeof( buf), "%s", "VfL Bochum 1848");
   if( rval == -1 && errno == ENOMEM)
   {
      printf( "%s", buf);
      return( 0);
   }

   return( 1);
}

