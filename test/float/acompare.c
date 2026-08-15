#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>

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
