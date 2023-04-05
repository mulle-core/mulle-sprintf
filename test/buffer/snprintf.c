#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

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
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   simple_string_test();

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   return( 0);
}

