#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



int  main()
{
   extern void  _mulle_sprintf_dump_available_defaultconversion_characters( void);

   _mulle_sprintf_dump_available_defaultconversion_characters();

   return( 0);
}

