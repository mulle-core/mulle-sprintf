//
//  staticprefix.c
//  mulle-sprintf
//
//  Test the "static prefix" fast path: leading literal text and "%%"
//  escapes are copied directly into the buffer, only the remainder gets
//  the full conversion treatment. This must not change output, return
//  values, or %n counts.
//
//  Copyright (c) 2026 Nat! - Mulle kybernetiK.
//  All rights reserved.
//
#include <mulle-sprintf/mulle-sprintf.h>

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


// %n cases need the count visible, so they get explicit checks instead of
// going through check() (which cannot see the %n target). Callers pass
// &n as `np` and again at the position of %n in the varargs.
static void   check_n( int *np, int expected_len, int expected_n, char *format, ...)
{
   va_list   args;
   char      m[ 512];
   int       len;

   *np = -1;
   va_start( args, format);
   len = mulle_vsnprintf( m, sizeof( m), format, args);
   va_end( args);

   if( len == expected_len && *np == expected_n)
      printf( "%-26s -> '%s' len %d n %d\n", format, m, len, *np);
   else
   {
      ++failures;
      printf( "%-26s -> FAIL len %d (want %d) n %d (want %d)\n",
              format, len, expected_len, *np, expected_n);
   }
}


int  main( void)
{
   int   n;

   // no conversions at all: whole format is a static prefix
   check( "just literal");
   check( "a longer literal that has no conversions at all");

   // "%%" escapes are static too
   check( "percent %% only");
   check( "100%%");
   check( "%%");
   check( "%%d");                      // escape + literal 'd'
   check( "a%%%%b");                   // two escapes

   // static prefix followed by real conversions
   check( "a%%%d", 42);
   check( "hello world %d %s", 42, "y'all");
   check( "x%%%%y %x", 0x1F);

   // a stray '%' at the end of the format is printed literally (mulle
   // extension; glibc drops it, so no comparison here)
   {
      char   m[ 512];
      int    len;

      len = mulle_snprintf( m, sizeof( m), "trailing %");
      if( len == 10 && ! strcmp( m, "trailing %"))
         printf( "%-26s -> '%s' len %d\n", "trailing %", m, len);
      else
      {
         ++failures;
         printf( "%-26s -> FAIL len %d\n", "trailing %", len);
      }

      len = mulle_snprintf( m, sizeof( m), "%%%");
      if( len == 2 && ! strcmp( m, "%%"))
         printf( "%-26s -> '%s' len %d\n", "%%%", m, len);
      else
      {
         ++failures;
         printf( "%-26s -> FAIL len %d\n", "%%%", len);
      }
   }

   // %n must count the copied prefix
   check_n( &n, 8, 6, "hello %n%d", &n, 42);
   check_n( &n, 5, 4, "a%%b %n%s", &n, "x");
   check_n( &n, 10, 6, "lit %d %n%s", 7, &n, "tail");
   check_n( &n, 59, 57,
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit. %n%d",
            &n, 42);

   if( failures)
   {
      fprintf( stderr, "%d failures\n", failures);
      return( 1);
   }
   return( 0);
}
