#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>


static void   simple_none_test()
{
   struct mulle_buffer   *buffer;
   va_list               va;
   mulle_vararg_list     args;
   int                   len;

   buffer = mulle_buffer_create( NULL);

   len = mulle_buffer_sprintf( buffer, "none");

   printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_vsprintf( buffer, "none", va);
   printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_mvsprintf( buffer, "none", args);
   printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);

   mulle_buffer_destroy( buffer);
}


int  main()
{

   mulle_sprintf_free_storage();

   simple_none_test();

   mulle_sprintf_free_storage();

   return( 0);
}

