#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <stdint.h>


int  main( void)
{
   struct mulle_buffer   *buffer;
   unsigned long long    big;
   int                   len;

   buffer = mulle_buffer_create( NULL);

   // value ranges straddling every digit boundary (u32 and u64 chunks)
   len = mulle_buffer_sprintf( buffer,
      "|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|%u|",
      (unsigned) 0, (unsigned) 1, (unsigned) 9, (unsigned) 10,
      (unsigned) 99, (unsigned) 100, (unsigned) 999, (unsigned) 1000,
      (unsigned) 9999, (unsigned) 10000, (unsigned) 999999, (unsigned) 1000000,
      (unsigned) 99999999, (unsigned) 100000000, (unsigned) 4294967295u,
      (unsigned) 0xFFFFFFFFu);
   if( len)
      printf( "uint: %.*s\n", len, (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   big = 18446744073709551615ULL;
   len = mulle_buffer_sprintf( buffer,
      "|%llu|%llu|%llu|%llu|%llu|%llu|%llu|%llu|",
      (unsigned long long) 0, (unsigned long long) 9999999999ULL,
      (unsigned long long) 1000000000000000ULL,
      (unsigned long long) 10000000000000000ULL,
      (unsigned long long) 100000000000000000ULL,
      (unsigned long long) 9999999999999999999ULL, big,
      (unsigned long long) 9223372036854775807ULL);
   if( len)
      printf( "ull:  %.*s\n", len, (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   // signed: negative + boundaries
   len = mulle_buffer_sprintf( buffer,
      "|%d|%d|%d|%d|%d|%lld|%lld|",
      (int) 0, (int) 5, (int) -5, (int) 2147483647, (int) -2147483647 - 1,
      (long long) -9223372036854775807LL - 1, (long long) 9223372036854775807LL);
   if( len)
      printf( "signed: %.*s\n", len, (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   // zero with precision/width: the caller formats zero, not the converter
   len = mulle_buffer_sprintf( buffer, "|%.0d|%5.0d|%05d|%d|",
                                     (int) 0, (int) 0, (int) 0, (int) 0);
   if( len)
      printf( "zero:  %.*s\n", len, (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "|%.0d|%5.0d|%.3d|%.12u|",
                                     (int) 5, (int) 42, (int) 42, (unsigned) 7);
   if( len)
      printf( "prec:  %.*s\n", len, (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   // octal / hex across ranges (also zero)
   len = mulle_buffer_sprintf( buffer, "|%o|%o|%o|%#o|%x|%x|%#x|",
                                     (unsigned) 0, (unsigned) 8, (unsigned) 0174700,
                                     (unsigned) 0, (unsigned) 0, (unsigned) 0xDEF0, (unsigned) 0x1E90);
   if( len)
      printf( "ox:    %.*s\n", len, (char *) mulle_buffer_get_bytes( buffer));

   mulle_buffer_destroy( buffer);
   return( 0);
}