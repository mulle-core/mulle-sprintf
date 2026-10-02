#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>



static void   simple_string_test()
{
   char           buf[ 32];
   int            rval;
   unsigned int   size;

   for( size = 1; size <= sizeof( buf); size *= 2)
   {
      rval = mulle_snprintf( buf, size, "VfL Bochum %d", 1848);
      if( rval == -1)
         printf( "Buffer size %u exhausted\n", size);
      else
         printf( "%u: %s\n", size, buf);
   }
}


int  main()
{

   simple_string_test();

   mulle_sprintf_free_storage();

   return( 0);
}

