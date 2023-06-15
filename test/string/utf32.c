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

   mulle_sprintf_free_storage();
   mulle_testallocator_reset();

   return( 0);
}

