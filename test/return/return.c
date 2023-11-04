#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <stdint.h>

// https://github.com/vlm/asn1c/issues/159
#if defined(_MSC_VER)
# include <windows.h>
typedef SSIZE_T ssize_t;
#endif



int  main( void)
{
   struct mulle_buffer   *buffer;
   int                   len1;
   int                   len2;
   unsigned int          n0 = 0xFD;  // random junk values
   unsigned char         n1 = 0xFE;
   unsigned short        n2 = 0xFF;
   unsigned long         n3 = 0xFC;
   unsigned long long    n4 = 0xFB;
   uintmax_t             n5 = 0xFA;
   ptrdiff_t             n6 = 0xF9;
   unsigned long long    n7 = 0xF8;
   ssize_t               n8 = 0xF7;

   buffer = mulle_buffer_create( NULL);

   len1 = mulle_buffer_sprintf( buffer, "|%n|%hhn|%hn|%ln|%lln|%jn|%tn|%qn|%zn|",
                                        &n0, &n1, &n2, &n3, &n4,&n5,&n6, &n7,
                                        &n8);
   len2 = mulle_buffer_sprintf( buffer, "={%u,%hhu,%hu,%lu,%llu,%ju,%tu,%qu,%zu}",
                                        n0, n1, n2, n3, n4, n5, n6, n7, n8);
   if( len1)
      printf( "%.*s (%d+%d)\n", len1 + len2, (char *) mulle_buffer_get_bytes( buffer), len1, len2);
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);

   return( 0);
}

