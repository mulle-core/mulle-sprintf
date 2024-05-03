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
   uint16_t              canary1 = 0x1EAC;
   unsigned int          n0 = 0xFD;  // random junk values
   uint16_t              canary2 = 0x2EAC;
   unsigned char         n1 = 0xFE;
   uint16_t              canary3 = 0x3EAC;
   unsigned short        n2 = 0xFF;
   uint16_t              canary4 = 0x4EAC;
   unsigned long         n3 = 0xFC;
   uint16_t              canary5 = 0x5EAC;
   unsigned long long    n4 = 0xFB;
   uint16_t              canary6 = 0x6EAC;
   uintmax_t             n5 = 0xFA;
   uint16_t              canary7 = 0x7EAC;
   ptrdiff_t             n6 = 0xF9;
   uint16_t              canary8 = 0x8EAC;
   uint64_t              n7 = 0xF8;
   uint16_t              canary9 = 0x9EAC;
   ssize_t               n8 = 0xF7;
   uint16_t              canary10 = 0xAEAC;

   buffer = mulle_buffer_create( NULL);

   len1 = mulle_buffer_sprintf( buffer, "|%n|%hhn|%hn|%ln|%lln|%jn|%tn|%qn|%zn|",
                                        &n0, &n1, &n2, &n3, &n4, &n5, &n6, &n7,
                                        &n8);
   assert( canary1  == 0x1EAC);
   assert( canary2  == 0x2EAC);
   assert( canary3  == 0x3EAC);
   assert( canary4  == 0x4EAC);
   assert( canary5  == 0x5EAC);
   assert( canary6  == 0x6EAC);
   assert( canary7  == 0x7EAC);
   assert( canary8  == 0x8EAC);
   assert( canary9  == 0x9EAC);
   assert( canary10 == 0xAEAC);

   len2 = mulle_buffer_sprintf( buffer, "={%u,%hhu,%hu,%lu,%llu,%ju,%tu,%qu,%zu}",
                                        n0, n1, n2, n3, n4, n5, n6, n7, n8);

   assert( canary1  == 0x1EAC);
   assert( canary2  == 0x2EAC);
   assert( canary3  == 0x3EAC);
   assert( canary4  == 0x4EAC);
   assert( canary5  == 0x5EAC);
   assert( canary6  == 0x6EAC);
   assert( canary7  == 0x7EAC);
   assert( canary8  == 0x8EAC);
   assert( canary9  == 0x9EAC);
   assert( canary10 == 0xAEAC);

   if( len1)
      printf( "%.*s (%d+%d)\n", len1 + len2, (char *) mulle_buffer_get_bytes( buffer), len1, len2);

   assert( canary1  == 0x1EAC);
   assert( canary2  == 0x2EAC);
   assert( canary3  == 0x3EAC);
   assert( canary4  == 0x4EAC);
   assert( canary5  == 0x5EAC);
   assert( canary6  == 0x6EAC);
   assert( canary7  == 0x7EAC);
   assert( canary8  == 0x8EAC);
   assert( canary9  == 0x9EAC);
   assert( canary10 == 0xAEAC);

   mulle_buffer_reset( buffer);

   assert( canary1  == 0x1EAC);
   assert( canary2  == 0x2EAC);
   assert( canary3  == 0x3EAC);
   assert( canary4  == 0x4EAC);
   assert( canary5  == 0x5EAC);
   assert( canary6  == 0x6EAC);
   assert( canary7  == 0x7EAC);
   assert( canary8  == 0x8EAC);
   assert( canary9  == 0x9EAC);
   assert( canary10 == 0xAEAC);

   mulle_buffer_destroy( buffer);

   return( 0);
}

