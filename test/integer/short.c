//
//  short.c
//  mulle-sprintf
//
//  %hhu/%hu conversions with values above the signed range: the union
//  members are read as signed, so this exercises the sign-extension
//  handling in integer_conversion.
//
//  Copyright (c) 2026 Nat! - Mulle kybernetiK.
//  All rights reserved.
//
#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>
#include <string.h>


static int   failures;


static void   check( char *format, ...)
{
   va_list   args;
   va_list   args2;
   char      m[ 512];
   char      g[ 512];
   int       mlen;
   int       glen;

   va_start( args, format);
   mlen = mulle_vsnprintf( m, sizeof( m), format, args);
   va_end( args);

   va_start( args2, format);
   glen = vsnprintf( g, sizeof( g), format, args2);
   va_end( args2);

   if( mlen == glen && ! strcmp( m, g))
      printf( "%-26s -> '%s' len %d\n", format, m, mlen);
   else
   {
      ++failures;
      printf( "%-26s -> MISMATCH mulle('%s',%d) glibc('%s',%d)\n",
              format, m, mlen, g, glen);
   }
}


int  main( void)
{
   check( "%hhu", (unsigned char) 200);
   check( "%hhu", (unsigned char) 255);
   check( "%hhd", (signed char) -128);
   check( "%hhd", (signed char) 127);
   check( "%hu", (unsigned short) 40000);
   check( "%hu", (unsigned short) 65535);
   check( "%hd", (short) -32768);
   check( "%hd", (short) 32767);
   check( "%hu %hu %hu", (unsigned short) 1, (unsigned short) 32768, (unsigned short) 65535);
   check( "%hhu %hhu", (unsigned char) 0, (unsigned char) 255);

   if( failures)
   {
      fprintf( stderr, "%d failures\n", failures);
      return( 1);
   }
   return( 0);
}
