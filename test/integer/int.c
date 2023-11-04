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
   int                   len;

   buffer = mulle_buffer_create( NULL);

   len = mulle_buffer_sprintf( buffer, "|%d|%hhd|%hd|%ld|%lld|%jd|%td|%qd|%zd|",
                                        (int) 1845, (char) 15, (short) 1845, (long) 1845,
                                        (long long) 1845, (intmax_t) 1845,
                                        (ptrdiff_t) 1845, (long long) 1845,
                                        (ssize_t) 1845);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "|%u|%hhu|%hu|%lu|%llu|%ju|%tu|%qu|%zu|",
                                        (unsigned int) 1846, (unsigned char) 16, (unsigned short) 1846, (unsigned long) 1846,
                                        (unsigned long long) 1846, (uintmax_t) 1846,
                                        (ptrdiff_t) 1846, (unsigned long long) 1846,
                                        (size_t) 1846);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "|%o|%hho|%ho|%lo|%llo|%jo|%to|%qo|%zo|",
                                        (int) 01747, (char) 017, (short) 01747 , (long) 01747,
                                        (long long) 01747, (intmax_t) 01747,
                                        (ptrdiff_t) 01747, (long long) 01747,
                                        (ssize_t) 01747);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "|%x|%hhx|%hx|%lx|%llx|%jx|%tx|%qx|%zx|",
                                        (unsigned int) 0x1848, (unsigned char) 0x18, (unsigned short) 0x1848, (unsigned long) 0x1848,
                                        (unsigned long long) 0x1848, (uintmax_t) 0x1848,
                                        (ptrdiff_t) 0x1848, (unsigned long long) 0x1848,
                                        (size_t) 0x1848);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);
   mulle_buffer_reset( buffer);

   len = mulle_buffer_sprintf( buffer, "|%X|%hhX|%hX|%lX|%llX|%jX|%tX|%qX|%zX|",
                                        (unsigned int) 0x1849, (unsigned char) 0x19, (unsigned short) 0x1849, (unsigned long) 0x1849,
                                        (unsigned long long) 0x1849, (uintmax_t) 0x1849,
                                        (ptrdiff_t) 0x1849, (unsigned long long) 0x1849,
                                        (size_t) 0x1849);
   if( len)
      printf( "%.*s (%d)\n", len, (char *) mulle_buffer_get_bytes( buffer), len);

   mulle_buffer_destroy( buffer);

   return( 0);
}

