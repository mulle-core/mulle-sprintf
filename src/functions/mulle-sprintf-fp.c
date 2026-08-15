//
//  mulle-sprintf-fp.c
//  mulle-sprintf
//
//  Copyright (c) 2018 Nat! - Mulle kybernetiK.
//  Copyright (c) 2011 Codeon GmbH.
//  All rights reserved.
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
#include "mulle-sprintf-fp.h"

#include "mulle-sprintf.h"
#include "include-private.h"
#include <float.h>
#include <stdio.h>
#include <math.h>

#ifndef NO_MULLE__DTOSTR
# include <mulle-dtostr/mulle-dtostr.h>
#endif
#ifdef NO_MULLE__DTOSTR
//
// first idea was to use dtostr or gcvt, but those use global variables
// in the end, it's just easier to use sprintf and produce a format
// string on the fly. (historic comment, with the new mulle-dtostr this
// has become moot)
static void  produce_format_string( char format[ 64],
                                    struct mulle_sprintf_formatconversioninfo *info,
                                    int is_long)
{
   char   prefix[ 2];

   prefix[ 0] = is_long ? 'L' : 0;
   prefix[ 1] = 0;

   if( info->memory.width_found)
   {
      if( info->memory.precision_found)
         snprintf( format, 64, "%%%ld.%d%s%c", (long) info->width, info->precision, prefix, info->conversion);
      else
         snprintf( format, 64, "%%%ld%s%c", (long) info->width, prefix, info->conversion);

      return;
   }

   if( info->memory.precision_found)
      snprintf( format, 64, "%%.%d%s%c", info->precision, prefix, info->conversion);
   else
      snprintf( format, 64, "%%%s%c", prefix, info->conversion);
}
#endif
// #include <float.h>
//
// static inline int isnan(double x) {
//     union {
//         double d;
//         uint64_t u;
//     } u;
//
//     u.d = x;
//     return (u.u & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL;
// }

//
// We used to normalize -0.0 to 0.0 here for "portability across platforms".
// This was removed because:
//
// 1. The sign of -0.0 carries real information (1/-0.0 == -inf,
//    atan2(-0.0,-1) == -pi). Suppressing it hides useful debugging data.
//
// 2. It doesn't actually help with test diffs. The common case that produces
//    -0.0 in output is NOT IEEE negative zero but a tiny negative value like
//    -4e-15 that rounds to "-0.0" at the given precision. Normalization of
//    the exact bit pattern doesn't catch that case, so tests still break
//    non-deterministically depending on libm/platform.
//
// 3. Every mainstream C library (glibc, musl, macOS, MSVC) prints -0.0 for
//    negative zero. Deviating silently from the C standard is surprising for
//    a "sprintf replacement".
//
// -nan is still normalized to nan because the sign of a NaN has no defined
// semantics in IEEE 754 and platforms genuinely disagree on it.
//
#ifdef NO_MULLE__DTOSTR
static inline int   is_negative_nan_double( double x)
{
   if( ! isnan( x))
     return( 0);

   // Use union to access the bit representation of the double
   union
   {
       double d;
       uint64_t i;
   } u = { .d = x };

   // Check the sign bit (most significant bit)
   return( (u.i >> 63) != 0);
}
static inline int   is_negative_nan_long_double( long double x)
{
   if( ! isnan( x))
     return( 0);

    // Use union to access the bytes of the long double
   union
   {
      long double     ld;
      unsigned char   bytes[ sizeof( long double)];
   } u;

    memcpy( &u.ld, &x, sizeof( long double));

#if __LITTLE_ENDIAN__
        // For little-endian /first, check the last byte
     return( (u.bytes[ sizeof( long double) - 1] & 0x80) != 0);
#else
     return( (u.bytes[ 0] & 0x80) != 0);
#endif
}
#endif
static mulle_sprintf_argumenttype_t  _mulle_sprintf_get_fp_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   assert( info->width < 64);
   assert( info->precision < 64);

   if( info->modifier[ 0] == 'L')
      return( mulle_sprintf_long_double_argumenttype);
   assert( info->modifier[ 0] == '\0');
   return( mulle_sprintf_double_argumenttype);
}


#ifndef NO_MULLE__DTOSTR

static int   _count_decimal_digits( uint64_t n)
{
   int   count;

   if( n == 0)
      return( 1);
   
   count = 0;
   while( n > 0)
   {
      n /= 10;
      count++;
   }
   return( count);
}


static uint64_t   _round_significand( uint64_t sig, int keep_digits, int *exponent)
{
   int        current_digits;
   int        next_digit;
   int        last_digit;
   uint64_t   remainder;
   uint64_t   divisor;
   int        i;

   current_digits = _count_decimal_digits( sig);
   if( keep_digits >= current_digits)
      return( sig);
   
   divisor = 1;
   for( i = 0; i < current_digits - keep_digits - 1; i++)
      divisor *= 10;
   
   next_digit = (sig / divisor) % 10;
   remainder  = sig % divisor;
   last_digit = ((sig / divisor) / 10) % 10;
   divisor   *= 10;
   sig        = (sig / divisor) * divisor;
   
   // round half to even (glibc behaviour)
   if( next_digit > 5 || (next_digit == 5 && (remainder != 0 || (last_digit & 1))))
   {
      sig += divisor;
      if( _count_decimal_digits( sig) > keep_digits)
      {
         sig /= 10;
         (*exponent)++;
      }
   }
   return( sig);
}


static void   _mulle_sprintf_fp_justify( struct mulle_buffer *buffer,
                                         struct mulle_sprintf_formatconversioninfo *info,
                                         size_t before)
{
   size_t   used;
   size_t   width;
   size_t   pad;
   size_t   insert;
   char     width_char;
   char     *bytes;
   char     *s;
   int      is_special;

   used = mulle_buffer_get_length( buffer) - before;
   width = info->width;
   if( width <= used)
      return;

   pad = width - used;

   if( info->memory.left_justify)
   {
      mulle_buffer_memset( buffer, ' ', pad);
      return;
   }

   bytes = mulle_buffer_get_bytes( buffer) + before;
   s     = bytes;
   insert = 0;
   if( *s == '-' || *s == '+' || *s == ' ')
   {
      insert = 1;
      s++;
   }

   is_special = ! strncmp( s, "inf", 3) || ! strncmp( s, "nan", 3);
   if( is_special)
   {
      width_char = ' ';
      insert     = 0;
   }
   else
   {
      width_char = info->memory.zero_found ? '0' : ' ';
      if( width_char == '0' &&
          (info->conversion == 'a' || info->conversion == 'A') &&
          s[ 0] == '0' && (s[ 1] == 'x' || s[ 1] == 'X'))
         insert += 2;
   }

   if( width_char == ' ')
      insert = 0;

   mulle_buffer_set_length( buffer, mulle_buffer_get_length( buffer) + pad, 0);
   bytes = mulle_buffer_get_bytes( buffer) + before;
   memmove( bytes + insert + pad, bytes + insert, used - insert);
   memset( bytes + insert, width_char, pad);
}


static int _mulle_sprintf_fp_e_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           struct mulle_sprintf_argumentarray *arguments,
                                           int argc)
{
   union mulle_sprintf_argumentvalue   v;
   struct mulle_dtostr_decimal         decimal;
   uint64_t                            significand;
   uint64_t                            divisor;
   int                                 exponent;
   int                                 precision;
   int                                 num_digits;
   int                                 i;
   size_t                              before;

   v = arguments->values[ argc];
   if( arguments->types[ argc] != mulle_sprintf_double_argumenttype)
      v.d = (double) v.ld;
   decimal = mulle_dtostr_decompose( v.d);
   before  = mulle_buffer_get_length( buffer);
   
   if( decimal.special)
   {
      if( decimal.sign)
         mulle_buffer_add_byte( buffer, '-');
      else if( info->memory.plus_found)
         mulle_buffer_add_byte( buffer, '+');
      else if( info->memory.space_found)
         mulle_buffer_add_byte( buffer, ' ');

      if( decimal.special == 3)  // zero - format with precision
      {
         mulle_buffer_add_byte( buffer, '0');
         precision = info->memory.precision_found ? info->precision : 6;
         if( precision > 0 || info->memory.hash_found)
         {
            mulle_buffer_add_byte( buffer, '.');
            for( i = 0; i < precision; i++)
               mulle_buffer_add_byte( buffer, '0');
         }
         mulle_buffer_add_byte( buffer, (info->conversion == 'E' || info->conversion == 'G') ? 'E' : 'e');
         mulle_buffer_add_byte( buffer, '+');
         mulle_buffer_add_byte( buffer, '0');
         mulle_buffer_add_byte( buffer, '0');
      }
      else
      {
         const char *s = decimal.special == 1 ? "inf" : "nan";
         mulle_buffer_add_string( buffer, (char *) s);
      }
      _mulle_sprintf_fp_justify( buffer, info, before);
      return( 0);
   }

   significand = decimal.significand;
   exponent    = decimal.exponent;
   precision   = info->memory.precision_found ? info->precision : 6;
   num_digits  = _count_decimal_digits( significand);
   significand = _round_significand( significand, precision + 1, &exponent);
   num_digits  = _count_decimal_digits( significand);
   exponent   += num_digits - 1;
   
   if( decimal.sign)
      mulle_buffer_add_byte( buffer, '-');
   else if( info->memory.plus_found)
      mulle_buffer_add_byte( buffer, '+');
   else if( info->memory.space_found)
      mulle_buffer_add_byte( buffer, ' ');
   
   divisor = 1;
   for( i = 1; i < num_digits; i++)
      divisor *= 10;
   
   mulle_buffer_add_byte( buffer, '0' + (unsigned char) (significand / divisor));
   significand %= divisor;
   
   if( precision > 0 || info->memory.hash_found)
      mulle_buffer_add_byte( buffer, '.');
   
   for( i = 0; i < precision && i < num_digits - 1; i++)
   {
      divisor /= 10;
      mulle_buffer_add_byte( buffer, '0' + (unsigned char) (significand / divisor));
      significand %= divisor;
   }
   for( ; i < precision; i++)
      mulle_buffer_add_byte( buffer, '0');
   
   mulle_buffer_add_byte( buffer, (info->conversion == 'E' || info->conversion == 'G') ? 'E' : 'e');
   mulle_buffer_add_byte( buffer, exponent >= 0 ? '+' : '-');
   
   if( exponent < 0)
      exponent = -exponent;
   
   if( exponent >= 100)
      mulle_buffer_add_byte( buffer, '0' + (exponent / 100));
   mulle_buffer_add_byte( buffer, '0' + ((exponent / 10) % 10));
   mulle_buffer_add_byte( buffer, '0' + (exponent % 10));
   
   _mulle_sprintf_fp_justify( buffer, info, before);
   return( 0);
}


static int _mulle_sprintf_fp_f_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           struct mulle_sprintf_argumentarray *arguments,
                                           int argc)
{
   union mulle_sprintf_argumentvalue   v;
   struct mulle_dtostr_decimal         decimal;
   uint64_t                            significand;
   uint64_t                            divisor;
   int                                 exponent;
   int                                 precision;
   int                                 num_digits;
   int                                 int_digits;
   int                                 i;
   int                                 j;
   size_t                              before;

   v = arguments->values[ argc];
   if( arguments->types[ argc] != mulle_sprintf_double_argumenttype)
      v.d = (double) v.ld;
   decimal = mulle_dtostr_decompose( v.d);
   before  = mulle_buffer_get_length( buffer);
   
   if( decimal.special)
   {
      if( decimal.special == 3)  // zero - format with precision
      {
         if( decimal.sign)
            mulle_buffer_add_byte( buffer, '-');
         else if( info->memory.plus_found)
            mulle_buffer_add_byte( buffer, '+');
         else if( info->memory.space_found)
            mulle_buffer_add_byte( buffer, ' ');
         
         mulle_buffer_add_byte( buffer, '0');
         precision = info->memory.precision_found ? info->precision : 6;
         if( precision > 0 || info->memory.hash_found)
         {
            mulle_buffer_add_byte( buffer, '.');
            for( i = 0; i < precision; i++)
               mulle_buffer_add_byte( buffer, '0');
         }
         _mulle_sprintf_fp_justify( buffer, info, before);
         return( 0);
      }
      
      const char *s = decimal.special == 1 ? (decimal.sign ? "-inf" : "inf") :
                      decimal.sign ? "-nan" : "nan";
      mulle_buffer_add_string( buffer, (char *) s);
      _mulle_sprintf_fp_justify( buffer, info, before);
      return( 0);
   }

   significand = decimal.significand;
   exponent    = decimal.exponent;
   precision   = info->memory.precision_found ? info->precision : 6;
   num_digits  = _count_decimal_digits( significand);
   int_digits  = num_digits + exponent;
   
   if( int_digits > 0)
      significand = _round_significand( significand, int_digits + precision, &exponent);
   else
      significand = _round_significand( significand, precision - exponent, &exponent);
   
   num_digits = _count_decimal_digits( significand);
   int_digits = num_digits + exponent;
   
   if( decimal.sign)
      mulle_buffer_add_byte( buffer, '-');
   else if( info->memory.plus_found)
      mulle_buffer_add_byte( buffer, '+');
   else if( info->memory.space_found)
      mulle_buffer_add_byte( buffer, ' ');
   
   if( int_digits <= 0)
   {
      mulle_buffer_add_byte( buffer, '0');
      if( precision > 0 || info->memory.hash_found)
      {
         mulle_buffer_add_byte( buffer, '.');
         for( i = 0; i < -int_digits && i < precision; i++)
            mulle_buffer_add_byte( buffer, '0');
         
         divisor = 1;
         for( j = 1; j < num_digits; j++)
            divisor *= 10;
         
         for( ; i < precision && num_digits > 0; i++)
         {
            mulle_buffer_add_byte( buffer, '0' + (unsigned char) (significand / divisor));
            significand %= divisor;
            divisor     /= 10;
            num_digits--;
         }
         for( ; i < precision; i++)
            mulle_buffer_add_byte( buffer, '0');
      }
   }
   else
   {
      divisor = 1;
      for( i = 1; i < num_digits; i++)
         divisor *= 10;
      
      for( i = 0; i < int_digits && num_digits > 0; i++)
      {
         mulle_buffer_add_byte( buffer, '0' + (unsigned char) (significand / divisor));
         significand %= divisor;
         divisor     /= 10;
         num_digits--;
      }
      for( ; i < int_digits; i++)
         mulle_buffer_add_byte( buffer, '0');
      
      if( precision > 0 || info->memory.hash_found)
      {
         mulle_buffer_add_byte( buffer, '.');
         for( i = 0; i < precision && num_digits > 0; i++)
         {
            mulle_buffer_add_byte( buffer, '0' + (unsigned char) (significand / divisor));
            significand %= divisor;
            divisor     /= 10;
            num_digits--;
         }
         for( ; i < precision; i++)
            mulle_buffer_add_byte( buffer, '0');
      }
   }
   _mulle_sprintf_fp_justify( buffer, info, before);
   return( 0);
}


static int _mulle_sprintf_fp_g_conversion(struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc)
{
   union mulle_sprintf_argumentvalue           v;
   struct mulle_dtostr_decimal                 decimal;
   struct mulle_sprintf_formatconversioninfo   info_copy;
   int                                         precision;
   int                                         exponent;
   int                                         num_digits;
   size_t                                      start;
   size_t                                      before;
   char                                        *str;
   char                                        *p;
   char                                        *end;
   char                                        *dot;

   v = arguments->values[ argc];
   if( arguments->types[ argc] != mulle_sprintf_double_argumenttype)
      v.d = (double) v.ld;
   decimal = mulle_dtostr_decompose( v.d);
   start   = mulle_buffer_get_length( buffer);
   
   if( decimal.special == 1 || decimal.special == 2)
   {
      if( decimal.sign)
         mulle_buffer_add_byte( buffer, '-');
      else if( info->memory.plus_found)
         mulle_buffer_add_byte( buffer, '+');
      else if( info->memory.space_found)
         mulle_buffer_add_byte( buffer, ' ');

      mulle_buffer_add_string( buffer, (char *) (decimal.special == 1 ? "inf" : "nan"));
      _mulle_sprintf_fp_justify( buffer, info, start);
      return( 0);
   }

   precision  = info->memory.precision_found ? info->precision : 6;
   if( precision == 0)
      precision = 1;
   
   num_digits = _count_decimal_digits( decimal.significand);
   exponent   = decimal.exponent + num_digits - 1;
   
   info_copy = *info;
   info_copy.width                 = 0;
   info_copy.memory.width_found    = 0;
   info_copy.memory.left_justify   = 0;
   info_copy.memory.zero_found     = 0;
   if( exponent < -4 || exponent >= precision)
   {
      info_copy.precision              = precision - 1;
      info_copy.memory.precision_found = 1;
      before                           = mulle_buffer_get_length( buffer);
      
      _mulle_sprintf_fp_e_conversion( buffer, &info_copy, arguments, argc);
      
      if( ! info->memory.hash_found)
      {
         str = mulle_buffer_get_bytes( buffer);
         p   = str + before;
         end = str + mulle_buffer_get_length( buffer);
         
         while( p < end && *p != 'e' && *p != 'E')
            p++;
         
         dot = p - 1;
         while( dot > str + before && *dot == '0')
            dot--;
         if( *dot == '.')
            dot--;
         
         if( dot + 1 < p)
         {
            memmove( dot + 1, p, end - p);
            mulle_buffer_set_length( buffer, before + (dot + 1 - (str + before)) + (end - p), 0);
         }
      }
   }
   else
   {
      info_copy.precision              = precision - (exponent + 1);
      info_copy.memory.precision_found = 1;
      before                           = mulle_buffer_get_length( buffer);
      
      _mulle_sprintf_fp_f_conversion( buffer, &info_copy, arguments, argc);
      
      if( ! info->memory.hash_found)
      {
         str = mulle_buffer_get_bytes( buffer);
         end = str + mulle_buffer_get_length( buffer);
         p   = end - 1;

         dot = str + before;
         while( dot < end && *dot != '.')
            dot++;

         if( dot < end)
         {
            while( p > dot && *p == '0')
               p--;
            if( p == dot)
               p--;

            if( p + 1 < end)
               mulle_buffer_set_length( buffer, before + (p + 1 - (str + before)), 0);
         }
      }
   }
   _mulle_sprintf_fp_justify( buffer, info, start);
   return( 0);
}


static int _mulle_sprintf_fp_a_conversion(struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc)
{
   union mulle_sprintf_argumentvalue   v;
   union { double d; uint64_t u; }     bits;
   uint64_t                            mantissa;
   int                                 exponent;
   int                                 precision;
   int                                 nibble;
   int                                 leading;
   int                                 out_digits;
   int                                 round_up;
   int                                 i;
   size_t                              before;
   char                                digits[ 13];
   char                                hex_char;

   v = arguments->values[ argc];
   if( arguments->types[ argc] != mulle_sprintf_double_argumenttype)
      v.d = (double) v.ld;
   bits.d   = v.d;
   mantissa = bits.u & 0xFFFFFFFFFFFFFULL;
   exponent = (bits.u >> 52) & 0x7FF;
   before   = mulle_buffer_get_length( buffer);
   hex_char = info->conversion == 'A' ? 'A' : 'a';
   
   if( bits.u >> 63)
      mulle_buffer_add_byte( buffer, '-');
   else if( info->memory.plus_found)
      mulle_buffer_add_byte( buffer, '+');
   else if( info->memory.space_found)
      mulle_buffer_add_byte( buffer, ' ');
   
   if( exponent == 0x7FF)
   {
      if( mantissa)
         mulle_buffer_add_string( buffer, "nan");
      else
         mulle_buffer_add_string( buffer, bits.u >> 63 ? "-inf" : "inf");
      _mulle_sprintf_fp_justify( buffer, info, before);
      return( 0);
   }
   
   mulle_buffer_add_byte( buffer, '0');
   mulle_buffer_add_byte( buffer, hex_char == 'A' ? 'X' : 'x');
   
   if( exponent == 0 && mantissa == 0)
   {
      mulle_buffer_add_byte( buffer, '0');
      if( info->memory.precision_found)
      {
         precision = info->precision;
         if( precision > 0 || info->memory.hash_found)
         {
            mulle_buffer_add_byte( buffer, '.');
            for( i = 0; i < precision; i++)
               mulle_buffer_add_byte( buffer, '0');
         }
      }
      mulle_buffer_add_byte( buffer, hex_char == 'A' ? 'P' : 'p');
      mulle_buffer_add_byte( buffer, '+');
      mulle_buffer_add_byte( buffer, '0');
   }
   else
   {
      leading = 1;
      if( exponent == 0)
      {
         leading  = 0;
         exponent = 1;
      }
      else
         mantissa |= 0x10000000000000ULL;

      for( i = 0; i < 13; i++)
         digits[ i] = (mantissa >> (48 - i * 4)) & 0xF;

      round_up = 0;
      if( info->memory.precision_found)
      {
         precision  = info->precision;
         out_digits = precision < 13 ? precision : 13;
         // round to nearest, ties to even, using the first dropped nibble
         if( precision < 13)
         {
            nibble = digits[ precision];
            if( nibble > 8)
               round_up = 1;
            else if( nibble == 8)
            {
               for( i = precision + 1; i < 13; i++)
               {
                  if( digits[ i])
                  {
                     round_up = 1;
                     break;
                  }
               }
               if( ! round_up)   // exact tie: keep last retained digit even
               {
                  if( precision == 0)
                  {
                     if( leading & 1)
                        round_up = 1;
                  }
                  else if( digits[ precision - 1] & 1)
                     round_up = 1;
               }
            }
         }
      }
      else
      {
         // No precision specified: output minimum digits (no trailing zeros)
         out_digits = 0;
         for( i = 12; i >= 0; i--)
         {
            if( digits[ i])
            {
               out_digits = i + 1;
               break;
            }
         }
      }

      if( round_up)
      {
         // increment the last retained digit, carrying into the leading digit
         for( i = out_digits - 1; i >= 0; i--)
         {
            if( ++digits[ i] < 16)
            {
               round_up = 0;
               break;
            }
            digits[ i] = 0;
         }
         if( round_up)
            leading += 1;
      }

      mulle_buffer_add_byte( buffer, '0' + leading);

      if( info->memory.precision_found)
      {
         precision  = info->precision;
         if( precision > 0 || info->memory.hash_found)
         {
            mulle_buffer_add_byte( buffer, '.');
            for( i = 0; i < precision; i++)
            {
               nibble = i < out_digits ? digits[ i] : 0;
               mulle_buffer_add_byte( buffer, nibble < 10 ? '0' + nibble : hex_char + nibble - 10);
            }
         }
      }
      else if( out_digits > 0 || info->memory.hash_found)
      {
         mulle_buffer_add_byte( buffer, '.');
         for( i = 0; i < out_digits; i++)
         {
            nibble = digits[ i];
            mulle_buffer_add_byte( buffer, nibble < 10 ? '0' + nibble : hex_char + nibble - 10);
         }
      }
      
      mulle_buffer_add_byte( buffer, hex_char == 'A' ? 'P' : 'p');
      exponent -= 1023;
      mulle_buffer_add_byte( buffer, exponent >= 0 ? '+' : '-');
      
      if( exponent < 0)
         exponent = -exponent;
      
      if( exponent >= 1000)
         mulle_buffer_add_byte( buffer, '0' + (exponent / 1000));
      if( exponent >= 100)
         mulle_buffer_add_byte( buffer, '0' + ((exponent / 100) % 10));
      if( exponent >= 10)
         mulle_buffer_add_byte( buffer, '0' + ((exponent / 10) % 10));
      mulle_buffer_add_byte( buffer, '0' + (exponent % 10));
   }
   _mulle_sprintf_fp_justify( buffer, info, before);
   return( 0);
}

static struct mulle_sprintf_function mulle_sprintf_fp_dtostr_functions =
{
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_e_conversion
};

static struct mulle_sprintf_function mulle_sprintf_fp_dtostr_f_functions =
{
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_f_conversion
};

static struct mulle_sprintf_function mulle_sprintf_fp_dtostr_g_functions =
{
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_g_conversion
};

static struct mulle_sprintf_function mulle_sprintf_fp_dtostr_a_functions =
{
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_a_conversion
};

#endif


#ifdef NO_MULLE__DTOSTR
static int   _mulle_sprintf_fp_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           struct mulle_sprintf_argumentarray *arguments,
                                           int argc)
{
   union mulle_sprintf_argumentvalue   v;
   mulle_sprintf_argumenttype_t        t;
   auto char                           format[ 64];
   auto char                           result[ 128];

   assert( buffer);
   assert( info);
   assert( arguments);

   v = arguments->values[ argc];
   t = arguments->types[ argc];

   if( t == mulle_sprintf_double_argumenttype)
   {
      // On MacOSX -nan is never printed, but on Linux
      // it is, which is inconsistent (do not want)
      if( is_negative_nan_double( v.d))
         snprintf( result, sizeof( result), "-nan");
      else
      {
         produce_format_string( format, info, 0);
         snprintf( result, sizeof( result), format, v.d);
      }
   }
   else
   {
      if( is_negative_nan_long_double( v.ld))
         snprintf( result, sizeof( result), "-nan");
      else
      {
         produce_format_string( format, info, 1);

         snprintf( result, sizeof( result), format, v.ld);
      }
   }
   mulle_buffer_add_string_with_maxlength( buffer, result, sizeof( result));
   return( 0);
}

static struct mulle_sprintf_function     mulle_sprintf_fp_functions =
{
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_conversion
};
#endif

void  mulle_sprintf_register_fp_functions( struct mulle_sprintf_conversion *tables)
{
#ifndef NO_MULLE__DTOSTR
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_a_functions, 'a');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_functions,   'e');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_f_functions, 'f');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_g_functions, 'g');

   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_a_functions, 'A');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_functions,   'E');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_f_functions, 'F');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtostr_g_functions, 'G');
#else
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'a');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'e');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'f');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'g');

   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'A');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'E');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'F');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'G');
#endif

   mulle_sprintf_register_modifier( tables, 'L');
}
// MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_fp_functions)
// static void  mulle_sprintf_register_default_fp_functions()
// {
//    mulle_sprintf_register_fp_functions( mulle_sprintf_get_defaultconversion());
// }

