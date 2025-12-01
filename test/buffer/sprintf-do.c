#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



int  main()
{
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   mulle_sprintf_do( s, "%s Bochum %d", "VfL", 1848)
   {
      puts( s);
   }

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   return( 0);
}

