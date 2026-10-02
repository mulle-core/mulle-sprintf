#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <stdint.h>

// https://github.com/vlm/asn1c/issues/159
#if defined(_MSC_VER)
# include <windows.h>
typedef SSIZE_T ssize_t;
#endif


static void   length_modifier_test()
{
   struct mulle_buffer   *buffer;
   int                   len;

   buffer = mulle_buffer_create( NULL);

   len = mulle_buffer_sprintf( buffer, "%hd", (short) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "%qd", 1848LL);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "%td", (ptrdiff_t) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "%zd", (ssize_t) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "%jd", (intmax_t) 1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}




int  main()
{

   mulle_sprintf_free_storage();

   length_modifier_test();

   mulle_sprintf_free_storage();

   return( 0);
}

