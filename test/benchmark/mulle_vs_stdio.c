#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-time/mulle-time.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static char   buffer[ 16384];


struct benchcase
{
   char                        *name;
   int                         conversions;
   size_t                      (*mulle)( char *, size_t, int);
   size_t                      (*glibc)( char *, size_t, int);
};


static void   bench( struct benchcase *bc, int iterations, int print)
{
   mulle_relativetime_t   a;
   mulle_relativetime_t   b;
   char                   *pbuf;
   size_t                 glibcbytes;
   size_t                 i;
   size_t                 mullebytes;
   double                 mratio;
   double                 msecs_glibc;
   double                 msecs_mulle;

   pbuf = buffer;

   a = mulle_relativetime_now();
   for( i = 0; i < iterations; i++)
      mullebytes = (*bc->mulle)( pbuf, sizeof( buffer), i);
   b = mulle_relativetime_now();
   msecs_mulle = (b - a) * 1000.0;

   a = mulle_relativetime_now();
   for( i = 0; i < iterations; i++)
      glibcbytes = (*bc->glibc)( pbuf, sizeof( buffer), i);
   b = mulle_relativetime_now();
   msecs_glibc = (b - a) * 1000.0;

   if( mullebytes != glibcbytes)
      fprintf( stderr, "MISMATCH %s: mulle=%zu glibc=%zu\n",
                   bc->name, mullebytes, glibcbytes);

   if( print)
   {
      mratio = msecs_mulle / ( msecs_glibc ? msecs_glibc : 1.0);
      fprintf( stdout,
         "%-28s conv=%2d  mulle=%9.3fms  glibc=%9.3fms  ratio=%6.2fx%s\n",
         bc->name, bc->conversions, msecs_mulle, msecs_glibc, mratio,
         mratio < 1.0 ? "  (mulle faster)" : "");
   }
}


static size_t   mf_int( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, "%d", v));
}


static size_t   gf_int( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, "%d", v));
}


static size_t   mf_int3( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, "%d %d %d", v, v + 1, v + 2));
}


static size_t   gf_int3( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, "%d %d %d", v, v + 1, v + 2));
}


static size_t   mf_intm( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      v, v + 1, v + 2, v + 3, v + 4, v + 5, v + 6, v + 7,
      v + 8, v + 9, v + 10, v + 11, v + 12, v + 13, v + 14, v + 15));
}


static size_t   gf_intm( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      v, v + 1, v + 2, v + 3, v + 4, v + 5, v + 6, v + 7,
      v + 8, v + 9, v + 10, v + 11, v + 12, v + 13, v + 14, v + 15));
}


static size_t   mf_mix( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%d %s %f %c %x %ld %g %u", v, "hello", 3.5, 'A', v, (long) v,
            (double) v / 10.0, (unsigned) v));
}


static size_t   gf_mix( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%d %s %f %c %x %ld %g %u", v, "hello", 3.5, 'A', v, (long) v,
            (double) v / 10.0, (unsigned) v));
}


static size_t   mf_float( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, "%.6f %.6f %.6f",
                  (double) v / 1000.0, (double) v / 7.0, 3.1415926535));
}


static size_t   gf_float( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, "%.6f %.6f %.6f",
                  (double) v / 1000.0, (double) v / 7.0, 3.1415926535));
}


static size_t   mf_str1( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, "[%s] %d", "abcdefghij", v));
}


static size_t   gf_str1( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, "[%s] %d", "abcdefghij", v));
}


static size_t   mf_strm( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%s %s %s %s %s %s %s %s", "one", "two", "three", "four",
      "five", "six", "seven", "eight"));
}


static size_t   gf_strm( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%s %s %s %s %s %s %s %s", "one", "two", "three", "four",
      "five", "six", "seven", "eight"));
}


static size_t   mf_int24( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      v, v + 1, v + 2, v + 3, v + 4, v + 5, v + 6, v + 7,
      v + 8, v + 9, v + 10, v + 11, v + 12, v + 13, v + 14, v + 15,
      v + 16, v + 17, v + 18, v + 19, v + 20, v + 21, v + 22, v + 23));
}


static size_t   gf_int24( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      v, v + 1, v + 2, v + 3, v + 4, v + 5, v + 6, v + 7,
      v + 8, v + 9, v + 10, v + 11, v + 12, v + 13, v + 14, v + 15,
      v + 16, v + 17, v + 18, v + 19, v + 20, v + 21, v + 22, v + 23));
}


static size_t   mf_str24( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
      "one", "two", "three", "four", "five", "six", "seven", "eight",
      "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen",
      "seventeen", "eighteen", "nineteen", "twenty", "twentyone", "twentytwo", "twentythree", "twentyfour"));
}


static size_t   gf_str24( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
      "one", "two", "three", "four", "five", "six", "seven", "eight",
      "nine", "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen",
      "seventeen", "eighteen", "nineteen", "twenty", "twentyone", "twentytwo", "twentythree", "twentyfour"));
}


static size_t   mf_float16( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f",
      v / 1000.0, 1.25, v / 7.0, 3.1415926535, v / 999.0, 0.01, v / 3.0, 123.456,
      v / 1000.0, 1.25, v / 7.0, 3.1415926535, v / 999.0, 0.01, v / 3.0, 123.456));
}


static size_t   gf_float16( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f",
      v / 1000.0,  1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456,
      v / 1000.0,  1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456));
}


static size_t   mf_float24( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f",
      v / 1000.0, 1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456,
      v / 1000.0, 1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456,
      v / 1000.0, 1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456));
}


static size_t   gf_float24( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f",
      v / 1000.0, 1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456,
      v / 1000.0, 1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456,
      v / 1000.0, 1.25, v / 7.0, 3.141592653589, v / 999.0, 0.01, v / 3.0, 123.456));
}


static size_t   mf_pad( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%10d %-5d %08d %.2f %-12.3f %12s %-12s %.4e %8x %5ld %c %s",
      v, v, v, v / 2.5, v / 7.0, "hello", "world", v / 3.0,
            v, (long) v, 'K', "end"));
}


static size_t   gf_pad( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%10d %-5d %08d %.2f %-12.3f %12s %-12s %.4e %8x %5ld %c %s",
      v, v, v, v / 2.5, v / 7.0, "hello", "world", v / 3.0,
            v, (long) v, 'K', "end"));
}


static size_t   mf_hex( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, "%x", (unsigned) v));
}


static size_t   gf_hex( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, "%x", (unsigned) v));
}


static size_t   mf_hex8( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%x %x %x %x %x %x %x %x",
      (unsigned) v, (unsigned) v + 1, (unsigned) v + 2, (unsigned) v + 3,
      (unsigned) v + 4, (unsigned) v + 5, (unsigned) v + 6, (unsigned) v + 7));
}


static size_t   gf_hex8( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%x %x %x %x %x %x %x %x",
      (unsigned) v, (unsigned) v + 1, (unsigned) v + 2, (unsigned) v + 3,
      (unsigned) v + 4, (unsigned) v + 5, (unsigned) v + 6, (unsigned) v + 7));
}


static size_t   mf_oct( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, "%o", (unsigned) v));
}


static size_t   gf_oct( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, "%o", (unsigned) v));
}


static size_t   mf_oct8( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size,
      "%o %o %o %o %o %o %o %o",
      (unsigned) v, (unsigned) v + 1, (unsigned) v + 2, (unsigned) v + 3,
      (unsigned) v + 4, (unsigned) v + 5, (unsigned) v + 6, (unsigned) v + 7));
}


static size_t   gf_oct8( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size,
      "%o %o %o %o %o %o %o %o",
      (unsigned) v, (unsigned) v + 1, (unsigned) v + 2, (unsigned) v + 3,
      (unsigned) v + 4, (unsigned) v + 5, (unsigned) v + 6, (unsigned) v + 7));
}


#define LONG_TEXT \
   "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "


static size_t   mf_longlit( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, LONG_TEXT "%%d", v));
}


static size_t   gf_longlit( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, LONG_TEXT "%%d", v));
}


#define SWAR_TAIL \
   "0123456789abcdef0123456789abcdef"


static size_t   mf_swar( char *buf, size_t size, int v)
{
   return( mulle_snprintf( buf, size, LONG_TEXT "%%" SWAR_TAIL "%d", v));
}


static size_t   gf_swar( char *buf, size_t size, int v)
{
   return( (size_t) snprintf( buf, size, LONG_TEXT "%%" SWAR_TAIL "%d", v));
}


static struct benchcase   cases[] =
{
   { "int %%d",               1, mf_int,  gf_int},
   { "int %%d x3",            3, mf_int3, gf_int3},
   { "int %%d x16",          16, mf_intm, gf_intm},
   { "int %%d x24",          24, mf_int24,gf_int24},
   { "long lit + %%d",        1, mf_longlit,gf_longlit},
   { "literal %% + SWAR + %%d", 1, mf_swar, gf_swar},
   { "hex %%x",               1, mf_hex,  gf_hex},
   { "hex %%x x8",            8, mf_hex8, gf_hex8},
   { "oct %%o",               1, mf_oct,  gf_oct},
   { "oct %%o x8",            8, mf_oct8, gf_oct8},
   { "mixed 8 convs",         8, mf_mix,  gf_mix},
   { "float %.6f x3",         3, mf_float,gf_float},
   { "float %.6f x16",       16, mf_float16,gf_float16},
   { "float %.6f x24",       24, mf_float24,gf_float24},
   { "string %%s + int x2",   2, mf_str1, gf_str1},
   { "string %%s x8",         8, mf_strm, gf_strm},
   { "string %%s x24",       24, mf_str24,gf_str24},
   { "pad mix (12 convs)",   12, mf_pad,  gf_pad}
};


int  main( int argc, char *argv[])
{
   double             budget;
   double             pass;
   int                first;
   int                i;
   int                iterations;
   int                n;
   mulle_relativetime_t   a;
   mulle_relativetime_t   b;

   if( argc > 3)
      first = atoi( argv[ 3]);
   else
      first = -1;

   if( first < 0)
   {
      first = 0;
      n     = (int) (sizeof( cases) / sizeof( cases[ 0]));
   }
   else
   {
      n = 1;
   }

   if( argc > 2)
      budget = atof( argv[ 2]);
   else
      budget = 1.0;

   if( argc > 1)
      iterations = atoi( argv[ 1]);
   else
      iterations = 100000;

   for(;;)
   {
      a = mulle_relativetime_now();
      for( i = 0; i < n; i++)
         bench( &cases[ first + i], iterations, 0);
      b = mulle_relativetime_now();
      pass = b - a;

      if( pass >= budget)
      {
         for( i = 0; i < n; i++)
            bench( &cases[ first + i], iterations, 1);
         break;
      }
      iterations *= 2;
   }

   return( 0);
}