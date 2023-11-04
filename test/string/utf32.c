#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-utf/mulle-utf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>


//
// There is an interesting dilemma:
// With %.3s do we want to see this
// p: |👣#|
// or this ?
// p: |👣#€|
// The emoji is twice as wide...
//
static mulle_utf32_t   emojimix[] =
{
      /* UTF32 feet, hash, euro */
      0x0001f463, 0x00000023, 0x000020ac,
      'V', 'f', 'L', ' ', 'B', 'o', 'c', 'h', 'u', 'm', ' ',
      0x0001f3b2, 0
      /* dice */
};

static mulle_utf32_t   plain[] =
{
      /* UTF16 feet, hash, euro */
      'V', 'f', 'L', ' ', 'B', 'o', 'c', 'h', 'u', 'm', ' ', '1', '8', '4', '8', 0
      /* dice */
};


static mulle_utf32_t   empty[] = { 0 };


static mulle_utf32_t   longish[] =
{
'T', 'H', 'I', 'S', ' ', 'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E', ' ', 'I', 'S',
' ', 'P', 'R', 'O', 'V', 'I', 'D', 'E', 'D', ' ', 'B', 'Y', ' ', 'T', 'H', 'E',
' ', 'C', 'O', 'P', 'Y', 'R', 'I', 'G', 'H', 'T', ' ', 'H', 'O', 'L', 'D', 'E',
'R', 'S', ' ', 'A', 'N', 'D', ' ', 'C', 'O', 'N', 'T', 'R', 'I', 'B', 'U', 'T',
'O', 'R', 'S', ' ', '"', 'A', 'S', ' ', 'I', 'S', '"', ' ',
'A', 'N', 'D', ' ', 'A', 'N', 'Y', ' ', 'E', 'X', 'P', 'R', 'E', 'S', 'S',
' ', 'O', 'R', ' ', 'I', 'M', 'P', 'L', 'I', 'E', 'D', ' ', 'W', 'A', 'R', 'R',
'A', 'N', 'T', 'I', 'E', 'S', ',', ' ', 'I', 'N', 'C', 'L', 'U', 'D', 'I', 'N',
'G', ',', ' ', 'B', 'U', 'T', ' ', 'N', 'O', 'T', ' ', 'L', 'I', 'M', 'I', 'T',
'E', 'D', ' ', 'T', 'O', ',', ' ', 'T', 'H', 'E', ' ',
'I', 'M', 'P', 'L', 'I', 'E', 'D', ' ', 'W', 'A', 'R', 'R', 'A', 'N', 'T',
'I', 'E', 'S', ' ', 'O', 'F', ' ', 'M', 'E', 'R', 'C', 'H', 'A', 'N', 'T', 'A',
'B', 'I', 'L', 'I', 'T', 'Y', ' ', 'A', 'N', 'D', ' ', 'F', 'I', 'T', 'N', 'E',
'S', 'S', ' ', 'F', 'O', 'R', ' ', 'A', ' ', 'P', 'A', 'R', 'T', 'I', 'C', 'U',
'L', 'A', 'R', ' ', 'P', 'U', 'R', 'P', 'O', 'S', 'E', ' ',
'A', 'R', 'E', ' ', 'D', 'I', 'S', 'C', 'L', 'A', 'I', 'M', 'E', 'D', '.',
' ', 'I', 'N', ' ', 'N', 'O', ' ', 'E', 'V', 'E', 'N', 'T', ' ', 'S', 'H', 'A',
'L', 'L', ' ', 'T', 'H', 'E', ' ', 'C', 'O', 'P', 'Y', 'R', 'I', 'G', 'H', 'T',
' ', 'H', 'O', 'L', 'D', 'E', 'R', ' ', 'O', 'R', ' ', 'C', 'O', 'N', 'T', 'R',
'I', 'B', 'U', 'T', 'O', 'R', 'S', ' ', 'B', 'E', ' ',
'L', 'I', 'A', 'B', 'L', 'E', ' ', 'F', 'O', 'R', ' ', 'A', 'N', 'Y', ' ',
'D', 'I', 'R', 'E', 'C', 'T', ',', ' ', 'I', 'N', 'D', 'I', 'R', 'E', 'C', 'T',
',', ' ', 'I', 'N', 'C', 'I', 'D', 'E', 'N', 'T', 'A', 'L', ',', ' ', 'S', 'P',
'E', 'C', 'I', 'A', 'L', ',', ' ', 'E', 'X', 'E', 'M', 'P', 'L', 'A', 'R', 'Y',
',', ' ', 'O', 'R', ' ',
'C', 'O', 'N', 'S', 'E', 'Q', 'U', 'E', 'N', 'T', 'I', 'A', 'L', ' ', 'D',
'A', 'M', 'A', 'G', 'E', 'S', ' ', '(', 'I', 'N', 'C', 'L', 'U', 'D', 'I', 'N',
'G', ',', ' ', 'B', 'U', 'T', ' ', 'N', 'O', 'T', ' ', 'L', 'I', 'M', 'I', 'T',
'E', 'D', ' ', 'T', 'O', ',', ' ', 'P', 'R', 'O', 'C', 'U', 'R', 'E', 'M', 'E',
'N', 'T', ' ', 'O', 'F', ' ',
'S', 'U', 'B', 'S', 'T', 'I', 'T', 'U', 'T', 'E', ' ', 'G', 'O', 'O', 'D',
'S', ' ', 'O', 'R', ' ', 'S', 'E', 'R', 'V', 'I', 'C', 'E', 'S', ';', ' ', 'L',
'O', 'S', 'S', ' ', 'O', 'F', ' ', 'U', 'S', 'E', ',', ' ', 'D', 'A', 'T', 'A',
',', ' ', 'O', 'R', ' ', 'P', 'R', 'O', 'F', 'I', 'T', 'S', ';', ' ', 'O', 'R',
' ', 'B', 'U', 'S', 'I', 'N', 'E', 'S', 'S', ' ',
'I', 'N', 'T', 'E', 'R', 'R', 'U', 'P', 'T', 'I', 'O', 'N', ')', ' ', 'H',
'O', 'W', 'E', 'V', 'E', 'R', ' ', 'C', 'A', 'U', 'S', 'E', 'D', ' ', 'A', 'N',
'D', ' ', 'O', 'N', ' ', 'A', 'N', 'Y', ' ', 'T', 'H', 'E', 'O', 'R', 'Y', ' ',
'O', 'F', ' ', 'L', 'I', 'A', 'B', 'I', 'L', 'I', 'T', 'Y', ',', ' ', 'W', 'H',
'E', 'T', 'H', 'E', 'R', ' ', 'I', 'N', ' ',
'C', 'O', 'N', 'T', 'R', 'A', 'C', 'T', ',', ' ', 'S', 'T', 'R', 'I', 'C',
'T', ' ', 'L', 'I', 'A', 'B', 'I', 'L', 'I', 'T', 'Y', ',', ' ', 'O', 'R', ' ',
'T', 'O', 'R', 'T', ' ', '(', 'I', 'N', 'C', 'L', 'U', 'D', 'I', 'N', 'G', ' ',
'N', 'E', 'G', 'L', 'I', 'G', 'E', 'N', 'C', 'E', ' ', 'O', 'R', ' ', 'O', 'T',
'H', 'E', 'R', 'W', 'I', 'S', 'E', ')', ' ',
'A', 'R', 'I', 'S', 'I', 'N', 'G', ' ', 'I', 'N', ' ', 'A', 'N', 'Y', ' ',
'W', 'A', 'Y', ' ', 'O', 'U', 'T', ' ', 'O', 'F', ' ', 'T', 'H', 'E', ' ', 'U',
'S', 'E', ' ', 'O', 'F', ' ', 'T', 'H', 'I', 'S', ' ', 'S', 'O', 'F', 'T', 'W',
'A', 'R', 'E', ',', ' ', 'E', 'V', 'E', 'N', ' ', 'I', 'F', ' ', 'A', 'D', 'V',
'I', 'S', 'E', 'D', ' ', 'O', 'F', ' ', 'T', 'H', 'E', ' ',
'P', 'O', 'S', 'S', 'I', 'B', 'I', 'L', 'I', 'T', 'Y', ' ', 'O', 'F', ' ',
'S', 'U', 'C', 'H', ' ', 'D', 'A', 'M', 'A', 'G', 'E', '.',
0x0001f463, 0x00000023, 0x000020ac,
0
};



static void   test( mulle_utf32_t *test_string)
{
   auto char   buf[ 1024];
   int         i;
   int         len;

   printf( "   |          1         2         3         4         |\n");
   printf( "   |01234567890123456789012345678901234567890123456789|\n");
   mulle_sprintf( buf, "%lS", test_string);
   printf( "p: |%s|\n", buf);

   len = mulle_utf32_strlen( test_string);
   for( i = 0; i <= len; i++)
   {
      mulle_sprintf( buf, "%.*lS", i, test_string);
      printf( "p: |%s|  # %%.%dlS\n", buf, i);
   }

   mulle_sprintf( buf, "%.50lS", test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%50lS", test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%50.50lS", test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%.*lS", 50, test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%*lS", 50, test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%*.*lS", 50, 50, test_string);
   printf( "p: |%s|\n", buf);

   printf( "\n");
}



int  main()
{
   mulle_testallocator_set_tracelevel( 1);
   mulle_testallocator_initialize();
   mulle_default_allocator = mulle_testallocator;

   test( empty);
   test( plain);
   test( emojimix);
   test( longish);
   test( NULL);

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   return( 0);
}

