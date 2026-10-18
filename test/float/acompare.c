#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

//
// DBL_TRUE_MIN is C11. TinyCC's <float.h> does not declare it in any mode
// (-std=c11 included), so supply it here. tcc's float.h is otherwise the same
// as gcc's (DBL_MANT_DIG 53, DBL_MIN_EXP -1021, DBL_MAX_EXP 1024), and
// 0x1p-1074 is exactly the smallest denormal: verified equal to
// DBL_MIN * DBL_EPSILON and to nextafter( 0.0, 1.0) under both compilers.
//
#ifndef DBL_TRUE_MIN
# if DBL_MANT_DIG == 53 && DBL_MIN_EXP == -1021 && DBL_MAX_EXP == 1024
#  define DBL_TRUE_MIN   0x1p-1074
# endif
#endif

// Informational test: compares mulle-sprintf %a output against the C library
// and prints any differences to stderr. Does not write to stdout, so there is
// no golden .stdout file that could fail.

static void   compare( double value)
{
   char        expected[ 256];
   char        actual[ 256];
   static char formats[][ 8] = { "%a", "%.0a", "%#.0a", "%.1a", "%.3a", "%.13a", "%#.13a", "%A" };
   int         i;

   for( i = 0; i < (int)( sizeof( formats) / sizeof( formats[ 0])); i++)
   {
      snprintf( expected, sizeof( expected), formats[ i], value);
      mulle_snprintf( actual, sizeof( actual), formats[ i], value);
      if( strcmp( expected, actual) != 0)
         fprintf( stderr, "DIFF %a: %-24s glibc=[%s] mulle=[%s]\n",
                  value, formats[ i], expected, actual);
   }
}

int  main()
{
   double   values[] = {
      0.0, -0.0, 1.0, -1.0, 0.5, 1.5, 2.5, 3.0, 0.0625, 0.1, 1.0/3.0, 6.5,
      1e-10, 1e100, 123456.789, 2.675, 100000.0,
      DBL_MIN, DBL_MAX, DBL_TRUE_MIN, 2.0 * DBL_TRUE_MIN, 1e-308, 5e-324,
      0.9999999999999999, 1.7976931348623157e308, -0.001
   };
   int   i;

   for( i = 0; i < (int)( sizeof( values) / sizeof( values[ 0])); i++)
      compare( values[ i]);
   return( 0);
}
