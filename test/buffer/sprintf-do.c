#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>



int  main()
{

   mulle_sprintf_do( s, "%s Bochum %d", "VfL", 1848)
   {
      puts( s);
   }

   mulle_sprintf_free_storage();

   return( 0);
}

