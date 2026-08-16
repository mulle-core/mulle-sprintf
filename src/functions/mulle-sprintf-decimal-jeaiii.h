//
//  mulle-sprintf-decimal-jeaiii.h
//  mulle-sprintf
//
//  C adaptation of the jeaiii integer-to-string algorithm.
//  Original C++ by James Edward Anhalt III, MIT License:
//  https://github.com/jeaiii/itoa
//
//  The algorithm writes digits front to back (it knows the leading digits
//  first), so it plugs into the mulle-sprintf converter interface through
//  the *_convert_* wrappers at the bottom of this file.
//
//  Copyright (c) 2026 Nat! - Mulle kybernetiK.
//  MIT License (same as original).
//
#ifndef mulle_sprintf_decimal_jeaiii_h__
#define mulle_sprintf_decimal_jeaiii_h__

#include <stdint.h>


//
// The pair table: "00" "01" ... "99" stored as 200 chars.
// Index with value*2 to get a two-character decimal pair.
//
static char const   mulle_decimal_pairs__[] =
   "00010203040506070809"
   "10111213141516171819"
   "20212223242526272829"
   "30313233343536373839"
   "40414243444546474849"
   "50515253545556575859"
   "60616263646566676869"
   "70717273747576777879"
   "80818283848586878889"
   "90919293949596979899";


//
// Write a two-digit pair at position p (forward).
//
static inline char   *mulle_jeaiii_pair__( char *p, unsigned int value)
{
   p[ 0] = mulle_decimal_pairs__[ value * 2];
   p[ 1] = mulle_decimal_pairs__[ value * 2 + 1];
   return( p + 2);
}


//
// Write one or two leading digits (forward).
//
static inline char   *mulle_jeaiii_first__( char *p, unsigned int value)
{
   if( value < 10)
   {
      *p++ = (char) ('0' + value);
      return( p);
   }
   return( mulle_jeaiii_pair__( p, value));
}


//
// Write exactly 8 zero-padded digits using fixed-point reciprocal (forward).
//
static inline char   *mulle_jeaiii_fixed8__( char *p, uint32_t value)
{
   uint64_t   f0;
   uint64_t   f2;
   uint64_t   f4;
   uint64_t   f6;

   f0 = ((UINT64_C( 1) << 48) / UINT64_C( 1000000) + 1) * value;
   f0 = (f0 >> 16) + 1;
   p  = mulle_jeaiii_pair__( p, (unsigned int) (f0 >> 32));
   f2 = (f0 & UINT32_MAX) * 100;
   p  = mulle_jeaiii_pair__( p, (unsigned int) (f2 >> 32));
   f4 = (f2 & UINT32_MAX) * 100;
   p  = mulle_jeaiii_pair__( p, (unsigned int) (f4 >> 32));
   f6 = (f4 & UINT32_MAX) * 100;
   return( mulle_jeaiii_pair__( p, (unsigned int) (f6 >> 32)));
}


//
// Convert a uint32_t to decimal (forward into buffer, return end pointer).
//
static inline char   *mulle_jeaiii_u32__( char *p, uint32_t value)
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
         p = mulle_jeaiii_pair__( p, value);
      return( p);
   }

   if( value < UINT32_C( 10000))
   {
      // 3-4 digits
      f0_32 = (UINT32_C( 10) * (UINT32_C( 1) << 24) /
               UINT32_C( 1000) + 1) * value;
      p     = mulle_jeaiii_first__( p, f0_32 >> 24);
      f2_32 = (f0_32 & ((UINT32_C( 1) << 24) - 1)) * 100;
      return( mulle_jeaiii_pair__( p, f2_32 >> 24));
   }

   if( value < UINT32_C( 1000000))
   {
      // 5-6 digits
      f0 = (UINT64_C( 10) * (UINT64_C( 1) << 32) /
            UINT64_C( 100000) + 1) * value;
      p  = mulle_jeaiii_first__( p, (unsigned int) (f0 >> 32));
      f2 = (f0 & UINT32_MAX) * 100;
      p  = mulle_jeaiii_pair__( p, (unsigned int) (f2 >> 32));
      f4 = (f2 & UINT32_MAX) * 100;
      return( mulle_jeaiii_pair__( p, (unsigned int) (f4 >> 32)));
   }

   if( value < UINT32_C( 100000000))
   {
      // 7-8 digits
      f0 = (UINT64_C( 10) * (UINT64_C( 1) << 48) /
            UINT64_C( 10000000) + 1) * value >> 16;
      p  = mulle_jeaiii_first__( p, (unsigned int) (f0 >> 32));
      f2 = (f0 & UINT32_MAX) * 100;
      p  = mulle_jeaiii_pair__( p, (unsigned int) (f2 >> 32));
      f4 = (f2 & UINT32_MAX) * 100;
      p  = mulle_jeaiii_pair__( p, (unsigned int) (f4 >> 32));
      f6 = (f4 & UINT32_MAX) * 100;
      return( mulle_jeaiii_pair__( p, (unsigned int) (f6 >> 32)));
   }

   // 9-10 digits
   f0 = (UINT64_C( 10) * (UINT64_C( 1) << 57) /
         UINT64_C( 1000000000) + 1) * value;
   p  = mulle_jeaiii_first__( p, (unsigned int) (f0 >> 57));
   f2 = (f0 & ((UINT64_C( 1) << 57) - 1)) * 100;
   p  = mulle_jeaiii_pair__( p, (unsigned int) (f2 >> 57));
   f4 = (f2 & ((UINT64_C( 1) << 57) - 1)) * 100;
   p  = mulle_jeaiii_pair__( p, (unsigned int) (f4 >> 57));
   f6 = (f4 & ((UINT64_C( 1) << 57) - 1)) * 100;
   p  = mulle_jeaiii_pair__( p, (unsigned int) (f6 >> 57));
   f8 = (f6 & ((UINT64_C( 1) << 57) - 1)) * 100;
   return( mulle_jeaiii_pair__( p, (unsigned int) (f8 >> 57)));
}


//
// Convert a uint64_t to decimal (forward into buffer, return end pointer).
// Splits into at most three 8-digit chunks.
//
static inline char   *mulle_jeaiii_u64__( char *p, uint64_t value)
{
   uint32_t   low;
   uint32_t   middle;
   uint64_t   high;

   if( value <= UINT32_MAX)
      return( mulle_jeaiii_u32__( p, (uint32_t) value));

   low  = (uint32_t) (value % UINT64_C( 100000000));
   high = value / UINT64_C( 100000000);
   if( high > UINT32_MAX)
   {
      middle = (uint32_t) (high % UINT64_C( 100000000));
      high  /= UINT64_C( 100000000);
      p      = mulle_jeaiii_u32__( p, (uint32_t) high);
      p      = mulle_jeaiii_fixed8__( p, middle);
   }
   else
      p = mulle_jeaiii_u32__( p, (uint32_t) high);

   return( mulle_jeaiii_fixed8__( p, low));
}


//
// mulle-sprintf interface for unsigned int. p points at the front of the
// digit area; *length is i/o: the available capacity on entry, the digit
// count on exit. Digits are written front to back, the start pointer is
// returned. Zero emits no digits: the caller formats it (precision/
// padding).
//
static inline char   *mulle_jeaiii_convert_unsigned_int( unsigned int value,
                                                         char *p,
                                                         size_t *length)
{
   char     *end;

   if( ! value)
   {
      *length = 0;
      return( p);
   }

   end     = mulle_jeaiii_u32__( p, value);
   *length = (size_t)( end - p);
   return( p);
}


//
// mulle-sprintf interface for unsigned long long. Same conventions as
// mulle_jeaiii_convert_unsigned_int above.
//
static inline char   *mulle_jeaiii_convert_unsigned_long_long( unsigned long long value,
                                                               char *p,
                                                               size_t *length)
{
   char     *end;

   if( ! value)
   {
      *length = 0;
      return( p);
   }

   end     = mulle_jeaiii_u64__( p, (uint64_t) value);
   *length = (size_t)( end - p);
   return( p);
}


#endif
