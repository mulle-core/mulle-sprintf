#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void   printf_string_test()
{
   auto char  buf[ 256];

   mulle_snprintf( buf, sizeof( buf), "001: |%#s|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "002: |%#.3s|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "003: |%#.4s|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "004: |%#.5s|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "005: |\"%#.100s\"|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "006: |\"%#100s\"|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "007: |\"%#100.100s\"|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "010: |\"%#100.5s\"|", "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "011: |\"%#.*s\"|", 100, "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "012: |\"%#*s\"|", 100, "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "013: |\"%#*.*s\"|", 100, 100, "VfL\tBochum\t1848");
   printf( "%s\n", buf);
   mulle_snprintf( buf, sizeof( buf), "014: |%#s|", "");
   printf( "%s\n", buf);
}



int  main()
{
   printf_string_test();

   return( 0);
}

