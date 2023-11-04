#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>

static mulle_sprintf_argumenttype_t
   determine_argument_type( struct mulle_sprintf_formatconversioninfo *info)
{
   return( 0);
}


static int   convert_argument( struct mulle_buffer *buffer,
                               struct mulle_sprintf_formatconversioninfo *info,
                               struct mulle_sprintf_argumentarray *arguments,
                               int i)
{
   return( 0);
}


struct mulle_sprintf_function    functions =
{
   determine_argument_type,
   convert_argument
};


int  main()
{
   // just coverage
   int   rc;

   struct mulle_sprintf_conversion   table = { 0 };

   rc = mulle_sprintf_register_default_functions( &functions, -1);
   assert( rc == -1);

   rc = mulle_sprintf_register_default_functions( &functions, 'a');
   assert( rc == 0);

   // overwrite is ok
   rc = mulle_sprintf_register_functions( NULL, &functions, 'd');
   assert( rc == 0);

   rc = mulle_sprintf_register_functions( &table, &functions, 'a');
   assert( rc == 0);

   rc = mulle_sprintf_register_functions( &table, &functions, -1);
   assert( rc == -1);

   rc = mulle_sprintf_register_modifier( &table, 'k');
   assert( rc == 0);
   // overwrite modifier ? no good
   rc = mulle_sprintf_register_functions( &table, &functions, 'k');
   assert( rc == -1);

   rc = mulle_sprintf_register_functions( NULL, NULL, 'x');
   assert( rc == -1);

   return( 0);
}

