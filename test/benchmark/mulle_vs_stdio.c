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


static struct benchcase   cases[ 7] =
{
   { "int %%d",               1, mf_int,  gf_int},
   { "int %%d x3",            3, mf_int3, gf_int3},
   { "int %%d x16",          16, mf_intm, gf_intm},
   { "mixed 8 convs",         8, mf_mix,  gf_mix},
   { "float %.6f x3",         3, mf_float,gf_float},
   { "string %%s + int x2",   2, mf_str1, gf_str1},
   { "string %%s x8",         8, mf_strm, gf_strm}
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
      n     = 7;
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