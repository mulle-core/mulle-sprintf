#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mulle-time/mulle-time.h>


#define DATASET_N   4096
#define OUTPUT_N    32
#define N_CASES     4
#define N_ROUNDS    5

#if defined( __clang__)
# define NOINLINE  __attribute__(( noinline))
#elif defined( __GNUC__)
# define NOINLINE  __attribute__(( noinline, noipa))
#else
# define NOINLINE
#endif


struct conversion_result
{
   char   *start;
   char   *end;
};


typedef struct conversion_result
   (*conversion_function_t)( uint64_t, char *);


struct benchmark_case
{
   char                    *name;
   conversion_function_t   function;
};


static char const   decimal_pairs[] =
   "0001020304050607080910111213141516171819"
   "2021222324252627282930313233343536373839"
   "4041424344454647484950515253545556575859"
   "6061626364656667686970717273747576777879"
   "8081828384858687888990919293949596979899";

static volatile uint64_t   benchmark_sink;
static uint64_t            values[ DATASET_N];


static NOINLINE struct conversion_result
   convert_div10( uint64_t value, char *buffer)
{
   struct conversion_result   result;
   char                       *p;

   result.end = &buffer[ OUTPUT_N];
   p          = result.end;
   do
   {
      *--p   = (char) ('0' + value % 10);
      value /= 10;
   }
   while( value);

   result.start = p;
   return( result);
}


static inline char   *write_pairs( uint64_t value, char *p)
{
   uint64_t   quotient;
   unsigned   remainder;

   while( value >= 100)
   {
      quotient  = value / 100;
      remainder = (unsigned) (value - quotient * 100);
      p        -= 2;
      p[ 0]     = decimal_pairs[ remainder * 2];
      p[ 1]     = decimal_pairs[ remainder * 2 + 1];
      value     = quotient;
   }

   if( value < 10)
      *--p = (char) ('0' + value);
   else
   {
      p    -= 2;
      p[ 0] = decimal_pairs[ (unsigned) value * 2];
      p[ 1] = decimal_pairs[ (unsigned) value * 2 + 1];
   }

   return( p);
}


static NOINLINE struct conversion_result
   convert_div100( uint64_t value, char *buffer)
{
   struct conversion_result   result;

   result.end   = &buffer[ OUTPUT_N];
   result.start = write_pairs( value, result.end);
   return( result);
}


static inline char   *write_fixed4( uint32_t value, char *p)
{
   unsigned int   remainder;

   remainder = value % 100;
   value    /= 100;
   p        -= 2;
   p[ 0]     = decimal_pairs[ remainder * 2];
   p[ 1]     = decimal_pairs[ remainder * 2 + 1];

   remainder = value;
   p        -= 2;
   p[ 0]     = decimal_pairs[ remainder * 2];
   p[ 1]     = decimal_pairs[ remainder * 2 + 1];

   return( p);
}


static NOINLINE struct conversion_result
   convert_chunk10000( uint64_t value, char *buffer)
{
   struct conversion_result   result;
   char                       *p;
   uint64_t                   quotient;
   unsigned int               remainder;

   result.end = &buffer[ OUTPUT_N];
   p          = result.end;
   while( value >= UINT64_C( 10000))
   {
      quotient  = value / UINT64_C( 10000);
      remainder = (unsigned int) (value - quotient * UINT64_C( 10000));
      p         = write_fixed4( remainder, p);
      value     = quotient;
   }

   result.start = write_pairs( value, p);
   return( result);
}


static inline char   *write_fixed8( uint32_t value, char *p)
{
   unsigned int   remainder;

   remainder = value % 100;
   value    /= 100;
   p        -= 2;
   p[ 0]     = decimal_pairs[ remainder * 2];
   p[ 1]     = decimal_pairs[ remainder * 2 + 1];

   remainder = value % 100;
   value    /= 100;
   p        -= 2;
   p[ 0]     = decimal_pairs[ remainder * 2];
   p[ 1]     = decimal_pairs[ remainder * 2 + 1];

   remainder = value % 100;
   value    /= 100;
   p        -= 2;
   p[ 0]     = decimal_pairs[ remainder * 2];
   p[ 1]     = decimal_pairs[ remainder * 2 + 1];

   remainder = value;
   p        -= 2;
   p[ 0]     = decimal_pairs[ remainder * 2];
   p[ 1]     = decimal_pairs[ remainder * 2 + 1];

   return( p);
}


static NOINLINE struct conversion_result
   convert_chunk100000000( uint64_t value, char *buffer)
{
   struct conversion_result   result;
   char                       *p;
   uint64_t                   quotient;
   unsigned int               remainder;

   result.end = &buffer[ OUTPUT_N];
   p          = result.end;
   while( value >= UINT64_C( 100000000))
   {
      quotient  = value / UINT64_C( 100000000);
      remainder = (unsigned int) (value - quotient * UINT64_C( 100000000));
      p         = write_fixed8( remainder, p);
      value     = quotient;
   }

   result.start = write_pairs( value, p);
   return( result);
}


// C adaptation of James Edward Anhalt III's MIT-licensed jeaiii algorithm.
// It uses reciprocal multiplication and emits one or two leading digits,
// followed by pairs. Large uint64_t values are split into 8-digit blocks.
static inline char   *write_pair_forward( char *p, unsigned int value)
{
   p[ 0] = decimal_pairs[ value * 2];
   p[ 1] = decimal_pairs[ value * 2 + 1];
   return( p + 2);
}


static inline char   *write_first_forward( char *p, unsigned int value)
{
   if( value < 10)
   {
      *p++ = (char) ('0' + value);
      return( p);
   }
   return( write_pair_forward( p, value));
}


static inline char   *jeaiii_write_u32( char *p, uint32_t value)
{
   uint32_t   f0_32;
   uint32_t   f2_32;
   uint64_t   f0;
   uint64_t   f2;
   uint64_t   f4;
   uint64_t   f6;
   uint64_t   f8;

   if( value < UINT32_C( 100))
   {
      if( value < 10)
         *p++ = (char) ('0' + value);
      else
         p = write_pair_forward( p, value);
      return( p);
   }

   if( value < UINT32_C( 10000))
   {
      f0_32 = (UINT32_C( 10) * (UINT32_C( 1) << 24) /
               UINT32_C( 1000) + 1) * value;
      p     = write_first_forward( p, f0_32 >> 24);
      f2_32 = (f0_32 & ((UINT32_C( 1) << 24) - 1)) * 100;
      return( write_pair_forward( p, f2_32 >> 24));
   }

   if( value < UINT32_C( 1000000))
   {
      f0 = (UINT64_C( 10) * (UINT64_C( 1) << 32) /
            UINT64_C( 100000) + 1) * value;
      p  = write_first_forward( p, (unsigned int) (f0 >> 32));
      f2 = (f0 & UINT32_MAX) * 100;
      p  = write_pair_forward( p, (unsigned int) (f2 >> 32));
      f4 = (f2 & UINT32_MAX) * 100;
      return( write_pair_forward( p, (unsigned int) (f4 >> 32)));
   }

   if( value < UINT32_C( 100000000))
   {
      f0 = (UINT64_C( 10) * (UINT64_C( 1) << 48) /
            UINT64_C( 10000000) + 1) * value >> 16;
      p  = write_first_forward( p, (unsigned int) (f0 >> 32));
      f2 = (f0 & UINT32_MAX) * 100;
      p  = write_pair_forward( p, (unsigned int) (f2 >> 32));
      f4 = (f2 & UINT32_MAX) * 100;
      p  = write_pair_forward( p, (unsigned int) (f4 >> 32));
      f6 = (f4 & UINT32_MAX) * 100;
      return( write_pair_forward( p, (unsigned int) (f6 >> 32)));
   }

   f0 = (UINT64_C( 10) * (UINT64_C( 1) << 57) /
         UINT64_C( 1000000000) + 1) * value;
   p  = write_first_forward( p, (unsigned int) (f0 >> 57));
   f2 = (f0 & ((UINT64_C( 1) << 57) - 1)) * 100;
   p  = write_pair_forward( p, (unsigned int) (f2 >> 57));
   f4 = (f2 & ((UINT64_C( 1) << 57) - 1)) * 100;
   p  = write_pair_forward( p, (unsigned int) (f4 >> 57));
   f6 = (f4 & ((UINT64_C( 1) << 57) - 1)) * 100;
   p  = write_pair_forward( p, (unsigned int) (f6 >> 57));
   f8 = (f6 & ((UINT64_C( 1) << 57) - 1)) * 100;
   return( write_pair_forward( p, (unsigned int) (f8 >> 57)));
}


static inline char   *jeaiii_write_fixed8( char *p, uint32_t value)
{
   uint64_t   f0;
   uint64_t   f2;
   uint64_t   f4;
   uint64_t   f6;

   f0 = ((UINT64_C( 1) << 48) / UINT64_C( 1000000) + 1) * value;
   f0 = (f0 >> 16) + 1;
   p  = write_pair_forward( p, (unsigned int) (f0 >> 32));
   f2 = (f0 & UINT32_MAX) * 100;
   p  = write_pair_forward( p, (unsigned int) (f2 >> 32));
   f4 = (f2 & UINT32_MAX) * 100;
   p  = write_pair_forward( p, (unsigned int) (f4 >> 32));
   f6 = (f4 & UINT32_MAX) * 100;
   return( write_pair_forward( p, (unsigned int) (f6 >> 32)));
}


static inline char   *jeaiii_write_u64( char *p, uint64_t value)
{
   uint32_t   low;
   uint32_t   middle;
   uint64_t   high;

   if( value <= UINT32_MAX)
      return( jeaiii_write_u32( p, (uint32_t) value));

   low  = (uint32_t) (value % UINT64_C( 100000000));
   high = value / UINT64_C( 100000000);
   if( high > UINT32_MAX)
   {
      middle = (uint32_t) (high % UINT64_C( 100000000));
      high  /= UINT64_C( 100000000);
      p      = jeaiii_write_u32( p, (uint32_t) high);
      p      = jeaiii_write_fixed8( p, middle);
   }
   else
      p = jeaiii_write_u32( p, (uint32_t) high);

   return( jeaiii_write_fixed8( p, low));
}


static NOINLINE struct conversion_result
   convert_jeaiii( uint64_t value, char *buffer)
{
   struct conversion_result   result;

   result.start = buffer;
   result.end   = jeaiii_write_u64( buffer, value);
   return( result);
}


static uint64_t   random_next( uint64_t *state)
{
   uint64_t   value;

   value  = *state;
   value ^= value >> 12;
   value ^= value << 25;
   value ^= value >> 27;
   *state  = value;
   return( value * UINT64_C( 2685821657736338717));
}


static void   fill_values( int dataset)
{
   uint64_t   random;
   uint64_t   state;
   int        i;

   state = UINT64_C( 0x1848a5a5f00dcafe);
   for( i = 0; i < DATASET_N; i++)
   {
      random = random_next( &state);
      switch( dataset)
      {
      case 0 :
         values[ i] = ((uint64_t) i * UINT64_C( 2441)) % UINT64_C( 10000000);
         break;

      case 1 :
         values[ i] = (uint32_t) random;
         break;

      case 2 :
         values[ i] = random;
         break;

      default :
         values[ i] = UINT64_MAX - (random & UINT64_C( 0x00000000000fffff));
         break;
      }
   }
}


static uint64_t   nanoseconds_now( void)
{
   mulle_timeinterval_t   now;

   now = mulle_timeinterval_now_monotonic();
   return( (uint64_t) (now * 1000000000.0));
}


static NOINLINE uint64_t   run_conversion( conversion_function_t function,
                                           size_t iterations)
{
   char                       buffer[ OUTPUT_N];
   struct conversion_result   result;
   size_t                     i;
   size_t                     length;
   uint64_t                   checksum;

   checksum = 0;
   for( i = 0; i < iterations; i++)
   {
      result   = (*function)( values[ i & (DATASET_N - 1)], buffer);
      length   = (size_t) (result.end - result.start);
      checksum = checksum * UINT64_C( 33) + length;
      checksum = checksum * UINT64_C( 33) + (unsigned char) result.start[ 0];
      checksum = checksum * UINT64_C( 33) + (unsigned char) result.start[ length - 1];
   }

   benchmark_sink = benchmark_sink * UINT64_C( 33) + checksum;
   return( checksum);
}


static double   benchmark( conversion_function_t function,
                           size_t iterations)
{
   double     best;
   double     elapsed;
   int        round;
   uint64_t   start;
   uint64_t   stop;

   run_conversion( function, DATASET_N);

   best = 1.0e100;
   for( round = 0; round < N_ROUNDS; round++)
   {
      start   = nanoseconds_now();
      run_conversion( function, iterations);
      stop    = nanoseconds_now();
      elapsed = (double) (stop - start) / (double) iterations;
      if( elapsed < best)
         best = elapsed;
   }

   return( best);
}


static int   check_function( struct benchmark_case *entry)
{
   static uint64_t const   edge_values[] =
   {
      UINT64_C( 0), UINT64_C( 1), UINT64_C( 9), UINT64_C( 10),
      UINT64_C( 11), UINT64_C( 99), UINT64_C( 100), UINT64_C( 101),
      UINT64_C( 9999), UINT64_C( 10000), UINT64_C( 10001),
      UINT64_C( 99999999), UINT64_C( 100000000), UINT64_C( 100000001),
      UINT32_MAX, UINT64_MAX
   };
   char                       actual[ OUTPUT_N];
   char                       expected[ OUTPUT_N];
   int                        length;
   size_t                     i;
   size_t                     n;
   struct conversion_result   result;
   uint64_t                   state;
   uint64_t                   value;

   n     = sizeof( edge_values) / sizeof( edge_values[ 0]);
   state = UINT64_C( 0xc001d00d1848a5a5);
   for( i = 0; i < n + 100000; i++)
   {
      value = i < n ? edge_values[ i] : random_next( &state);
      result = (*entry->function)( value, actual);
      length = snprintf( expected, sizeof( expected), "%" PRIu64, value);
      if( length < 0 ||
          (size_t) length != (size_t) (result.end - result.start) ||
          memcmp( result.start, expected, (size_t) length))
      {
         fprintf( stderr,
                  "%s failed for %" PRIu64 ": got '%.*s', expected '%s'\n",
                  entry->name,
                  value,
                  (int) (result.end - result.start),
                  result.start,
                  expected);
         return( -1);
      }
   }

   return( 0);
}


int   main( int argc, char *argv[])
{
   static char   *dataset_names[ N_CASES] =
   {
      "small (< 10^7)",
      "random uint32",
      "random uint64",
      "near UINT64_MAX"
   };
   static struct benchmark_case   cases[] =
   {
      { "div10",       convert_div10 },
      { "div100",      convert_div100 },
      { "chunk 10^4",  convert_chunk10000 },
      { "chunk 10^8",  convert_chunk100000000 },
      { "jeaiii",      convert_jeaiii }
   };
   char     *endptr;
   double   baseline;
   double   results[ sizeof( cases) / sizeof( cases[ 0])];
   int      dataset;
   size_t   i;
   size_t   iterations;
   size_t   n;

   iterations = 5000000;
#ifdef MULLE_TEST_VALGRIND
   iterations = 1000;
#endif
   if( argc > 1)
   {
      iterations = (size_t) strtoull( argv[ 1], &endptr, 10);
      if( ! iterations || *endptr)
      {
         fprintf( stderr, "usage: %s [iterations]\n", argv[ 0]);
         return( 1);
      }
   }

   n = sizeof( cases) / sizeof( cases[ 0]);
   for( i = 0; i < n; i++)
      if( check_function( &cases[ i]))
         return( 1);

   printf( "uint64 to decimal, -O3, best of %d rounds, %zu iterations\n",
           N_ROUNDS,
           iterations);
   printf( "The ratio is relative to the div10 loop; lower is better.\n");

   for( dataset = 0; dataset < N_CASES; dataset++)
   {
      fill_values( dataset);
      printf( "\n%s:\n", dataset_names[ dataset]);

      for( i = 0; i < n; i++)
         results[ i] = benchmark( cases[ i].function, iterations);

      baseline = results[ 0];
      for( i = 0; i < n; i++)
         printf( "  %-12s %8.3f ns/value  ratio %6.3f\n",
                 cases[ i].name,
                 results[ i],
                 results[ i] / baseline);
   }

   printf( "\nobservable checksum: %" PRIu64 "\n", benchmark_sink);
   return( 0);
}

