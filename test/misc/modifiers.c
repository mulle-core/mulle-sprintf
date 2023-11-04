#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



int  main()
{
   // just coverage
   int   rc;

   rc = mulle_sprintf_register_modifiers( NULL, NULL);
   assert( rc == -1);
   rc = mulle_sprintf_register_modifiers( NULL, "\01");
   assert( rc == -1);

   rc = mulle_sprintf_register_default_modifiers( "(&)");
   assert( rc == 0);
   rc = mulle_sprintf_register_default_modifier( '^');
   assert( rc == 0);

   rc = mulle_sprintf_register_modifier( NULL, 'k');
   assert( rc == 0);

   // sharing modifiers is ok
   rc = mulle_sprintf_register_modifier( NULL, 'k');
   assert( rc == 0);

   // reuse of function not cool
   rc = mulle_sprintf_register_modifier( NULL, 'd');
   assert( rc == -1);

   // not cool
   rc = mulle_sprintf_register_modifier( NULL, '\n');
   assert( rc == -1);

   return( 0);
}

