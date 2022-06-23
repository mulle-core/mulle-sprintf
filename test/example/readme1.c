#include <mulle-sprintf/mulle-sprintf.h>
#include <stdio.h>
#include <string.h>


int   main( void)
{
   auto char   storage[ 32];

   memset( storage, 'x', sizeof( storage));
   mulle_sprintf( storage, "%d", 1848);
   printf( "%s\n", storage);

   return( 0);
}