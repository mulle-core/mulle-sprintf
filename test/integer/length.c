#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>
#include <stdint.h>


static void   length_modifier_test()
{
   struct mulle_buffer   *buffer;
   int                   len;

   buffer = mulle_buffer_create( NULL);

   len = mulle_sprintf( buffer, "%hd", (short) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_sprintf( buffer, "%qd", 1848LL);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_sprintf( buffer, "%td", (ptrdiff_t) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_sprintf( buffer, "%zd", (ssize_t) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_sprintf( buffer, "%jd", (intmax_t) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}




int  main()
{
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   length_modifier_test();

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();
}

