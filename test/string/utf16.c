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
static mulle_utf16_t   emojimix[] =
{
      /* UTF16 feet, hash, euro */
      0xd83d, 0xdc63, 0x0023, 0x20ac,
      'V', 'f', 'L', ' ', 'B', 'o', 'c', 'h', 'u', 'm', ' ',
      0xd83c, 0xdfb2, 0
      /* dice */
};

static mulle_utf16_t   plain[] =
{
      /* UTF16 feet, hash, euro */
      'V', 'f', 'L', ' ', 'B', 'o', 'c', 'h', 'u', 'm', ' ', '1', '8', '4', '8', 0
      /* dice */
};


static mulle_utf16_t   empty[] =
{
   0
};



static void   test( mulle_utf16_t *test_string)
{
   auto char   buf[ 1024];
   int         i;
   int         len;

   printf( "   |          1         2         3         4         |\n");
   printf( "   |01234567890123456789012345678901234567890123456789|\n");
   mulle_sprintf( buf, "%hS", test_string);
   printf( "p: |%s|\n", buf);

   len = mulle_utf16_utf32length( test_string, -1);
   for( i = 0; i <= len; i++)
   {
      mulle_sprintf( buf, "%.*hS", i, test_string);
      printf( "p: |%s|  # %%.%dlS\n", buf, i);
   }

   mulle_sprintf( buf, "%.50hS", test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%50hS", test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%50.50hS", test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%.*hS", 50, test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%*hS", 50, test_string);
   printf( "p: |%s|\n", buf);

   mulle_sprintf( buf, "%*.*hS", 50, 50, test_string);
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

