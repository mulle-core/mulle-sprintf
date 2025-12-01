//
//  mulle_sprintf_fp.c
//  mulle-sprintf
//
//  Created by Nat!
//  Copyright (c) 2011 Nat! - Mulle kybernetiK.
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

#ifndef NO_MULLE__DTOA
# include "../mulle-dtoa/mulle-dtoa.h"
#endif


//
// first idea was to use dtoa or gcvt, but those use global variables
// in the end, it's just easier to use sprintf and produce a format
// string on the fly.
//
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

#define MASK_TOP_UINT64_BIT_ZERO(x) ((x) & (UINT64_MAX >> 1))


static inline double   make_non_negative_zero_double_if_zero( double x)
{
    if( x != 0.0)
      return( x);

    // Use union to access the bit representation of the double
    union
    {
        double d;
        uint64_t i;
    } u = { .d = x };

    u.i = MASK_TOP_UINT64_BIT_ZERO( u.i);
    return( u.d);
}



static inline long double   make_non_negative_zero_long_double_if_zero( long double x)
{
    if( x != 0.0)
      return( x);

    // Use union to access the bytes of the long double
   union
   {
      long double     ld;
      unsigned char   bytes[ sizeof( long double)];
   } u;

    memcpy( &u.ld, &x, sizeof( long double));

#if __LITTLE_ENDIAN__
        // For little-endian /first, check the last byte
     u.bytes[ sizeof( long double) - 1] &= 0x7F;
#else
     u.bytes[ 0] &= 0x7F;
#endif
     return( u.ld);
}


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


#ifndef NO_MULLE__DTOA

static mulle_sprintf_argumenttype_t _mulle_sprintf_get_fp_argumenttype(struct mulle_sprintf_formatconversioninfo *info);

static int count_digits(uint64_t n) {
   int count = 0;
   if (n == 0) return 1;
   while (n > 0) { n /= 10; count++; }
   return count;
}

static uint64_t round_significand(uint64_t sig, int keep_digits, int *exponent) {
   int current_digits = count_digits(sig);
   if (keep_digits >= current_digits) return sig;
   uint64_t divisor = 1;
   for (int i = 0; i < current_digits - keep_digits - 1; i++) divisor *= 10;
   int next_digit = (sig / divisor) % 10;
   divisor *= 10;
   sig = (sig / divisor) * divisor;
   if (next_digit >= 5) {
      sig += divisor;
      if (count_digits(sig) > keep_digits) { sig /= 10; (*exponent)++; }
   }
   return sig;
}

static int _mulle_sprintf_fp_e_conversion(struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc) {
   union mulle_sprintf_argumentvalue v;
   mulle_sprintf_argumenttype_t t;
   struct mulle_dtoa_decimal dec;
   char result[128], *p;
   uint64_t sig, divisor;
   int exp, precision, num_digits, i;

   v = arguments->values[argc];
   t = arguments->types[argc];
   if (t != mulle_sprintf_double_argumenttype) {
      auto char format[64], res[128];
      produce_format_string(format, info, 1);
      v.ld = make_non_negative_zero_long_double_if_zero(v.ld);
      snprintf(res, 128, format, v.ld);
      mulle_buffer_add_string(buffer, res);
      return 0;
   }

   v.d = make_non_negative_zero_double_if_zero(v.d);
   dec = mulle_dtoa_decompose(v.d);
   if (dec.special) {
      const char *s = dec.special == 1 ? (dec.sign ? "-inf" : "inf") :
                      dec.special == 2 ? (dec.sign ? "-nan" : "nan") :
                      dec.sign ? "-0" : "0";
      mulle_buffer_add_string(buffer, (char*)s);
      return 0;
   }

   p = result;
   sig = dec.significand;
   exp = dec.exponent;
   precision = info->memory.precision_found ? info->precision : 6;
   num_digits = count_digits(sig);
   sig = round_significand(sig, precision + 1, &exp);
   num_digits = count_digits(sig);
   exp += num_digits - 1;
   
   if (dec.sign) *p++ = '-';
   else if (info->memory.plus_found) *p++ = '+';
   else if (info->memory.space_found) *p++ = ' ';
   
   divisor = 1;
   for (i = 1; i < num_digits; i++) divisor *= 10;
   *p++ = '0' + (sig / divisor);
   sig %= divisor;
   if (precision > 0 || info->memory.hash_found) *p++ = '.';
   for (i = 0; i < precision && i < num_digits - 1; i++) {
      divisor /= 10;
      *p++ = '0' + (sig / divisor);
      sig %= divisor;
   }
   for (; i < precision; i++) *p++ = '0';
   *p++ = info->conversion;
   *p++ = exp >= 0 ? '+' : '-';
   if (exp < 0) exp = -exp;
   if (exp >= 100) *p++ = '0' + (exp / 100);
   *p++ = '0' + ((exp / 10) % 10);
   *p++ = '0' + (exp % 10);
   *p = '\0';
   mulle_buffer_add_string(buffer, result);
   return 0;
}

static int _mulle_sprintf_fp_f_conversion(struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc) {
   union mulle_sprintf_argumentvalue v;
   mulle_sprintf_argumenttype_t t;
   struct mulle_dtoa_decimal dec;
   char result[512], *p;
   uint64_t sig, divisor;
   int exp, precision, num_digits, i, int_digits;

   v = arguments->values[argc];
   t = arguments->types[argc];
   if (t != mulle_sprintf_double_argumenttype) {
      auto char format[64], res[512];
      produce_format_string(format, info, 1);
      v.ld = make_non_negative_zero_long_double_if_zero(v.ld);
      snprintf(res, 512, format, v.ld);
      mulle_buffer_add_string(buffer, res);
      return 0;
   }

   v.d = make_non_negative_zero_double_if_zero(v.d);
   dec = mulle_dtoa_decompose(v.d);
   if (dec.special) {
      const char *s = dec.special == 1 ? (dec.sign ? "-inf" : "inf") :
                      dec.special == 2 ? (dec.sign ? "-nan" : "nan") :
                      dec.sign ? "-0" : "0";
      mulle_buffer_add_string(buffer, (char*)s);
      return 0;
   }

   p = result;
   sig = dec.significand;
   exp = dec.exponent;
   precision = info->memory.precision_found ? info->precision : 6;
   num_digits = count_digits(sig);
   int_digits = num_digits + exp;
   
   if (int_digits > 0) sig = round_significand(sig, int_digits + precision, &exp);
   else sig = round_significand(sig, precision - exp, &exp);
   
   num_digits = count_digits(sig);
   int_digits = num_digits + exp;
   
   if (dec.sign) *p++ = '-';
   else if (info->memory.plus_found) *p++ = '+';
   else if (info->memory.space_found) *p++ = ' ';
   
   if (int_digits <= 0) {
      *p++ = '0';
      if (precision > 0 || info->memory.hash_found) {
         *p++ = '.';
         for (i = 0; i < -int_digits && i < precision; i++) *p++ = '0';
         divisor = 1;
         for (int j = 1; j < num_digits; j++) divisor *= 10;
         for (; i < precision && num_digits > 0; i++) {
            *p++ = '0' + (sig / divisor);
            sig %= divisor;
            divisor /= 10;
            num_digits--;
         }
         for (; i < precision; i++) *p++ = '0';
      }
   } else {
      divisor = 1;
      for (i = 1; i < num_digits; i++) divisor *= 10;
      for (i = 0; i < int_digits && num_digits > 0; i++) {
         *p++ = '0' + (sig / divisor);
         sig %= divisor;
         divisor /= 10;
         num_digits--;
      }
      for (; i < int_digits; i++) *p++ = '0';
      if (precision > 0 || info->memory.hash_found) {
         *p++ = '.';
         for (i = 0; i < precision && num_digits > 0; i++) {
            *p++ = '0' + (sig / divisor);
            sig %= divisor;
            divisor /= 10;
            num_digits--;
         }
         for (; i < precision; i++) *p++ = '0';
      }
   }
   *p = '\0';
   mulle_buffer_add_string(buffer, result);
   return 0;
}

static int _mulle_sprintf_fp_g_conversion(struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc) {
   union mulle_sprintf_argumentvalue v;
   mulle_sprintf_argumenttype_t t;
   struct mulle_dtoa_decimal dec;
   int precision, exp, num_digits;
   struct mulle_sprintf_formatconversioninfo info_copy;

   v = arguments->values[argc];
   t = arguments->types[argc];
   if (t != mulle_sprintf_double_argumenttype) {
      auto char format[64], res[512];
      produce_format_string(format, info, 1);
      v.ld = make_non_negative_zero_long_double_if_zero(v.ld);
      snprintf(res, 512, format, v.ld);
      mulle_buffer_add_string(buffer, res);
      return 0;
   }

   v.d = make_non_negative_zero_double_if_zero(v.d);
   dec = mulle_dtoa_decompose(v.d);
   if (dec.special) {
      const char *s = dec.special == 1 ? (dec.sign ? "-inf" : "inf") :
                      dec.special == 2 ? (dec.sign ? "-nan" : "nan") :
                      dec.sign ? "-0" : "0";
      mulle_buffer_add_string(buffer, (char*)s);
      return 0;
   }

   precision = info->memory.precision_found ? info->precision : 6;
   if (precision == 0) precision = 1;
   num_digits = count_digits(dec.significand);
   exp = dec.exponent + num_digits - 1;
   
   info_copy = *info;
   if (exp < -4 || exp >= precision) {
      info_copy.precision = precision - 1;
      info_copy.memory.precision_found = 1;
      size_t before = mulle_buffer_get_length(buffer);
      _mulle_sprintf_fp_e_conversion(buffer, &info_copy, arguments, argc);
      if (!info->memory.hash_found) {
         char *str = mulle_buffer_get_bytes(buffer);
         char *p = str + before, *end = str + mulle_buffer_get_length(buffer);
         while (p < end && *p != 'e' && *p != 'E') p++;
         char *dot = p - 1;
         while (dot > str + before && *dot == '0') dot--;
         if (*dot == '.') dot--;
         if (dot + 1 < p) {
            memmove(dot + 1, p, end - p);
            mulle_buffer_set_length(buffer, before + (dot + 1 - (str + before)) + (end - p), 0);
         }
      }
   } else {
      info_copy.precision = precision - (exp + 1);
      info_copy.memory.precision_found = 1;
      size_t before = mulle_buffer_get_length(buffer);
      _mulle_sprintf_fp_f_conversion(buffer, &info_copy, arguments, argc);
      if (!info->memory.hash_found) {
         char *str = mulle_buffer_get_bytes(buffer);
         char *end = str + mulle_buffer_get_length(buffer);
         char *p = end - 1;
         while (p > str + before && *p == '0') p--;
         if (*p == '.') p--;
         if (p + 1 < end) mulle_buffer_set_length(buffer, before + (p + 1 - (str + before)), 0);
      }
   }
   return 0;
}

static int _mulle_sprintf_fp_a_conversion(struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc) {
   union mulle_sprintf_argumentvalue v;
   mulle_sprintf_argumenttype_t t;
   char result[128], *p;
   union { double d; uint64_t u; } bits;
   uint64_t mantissa;
   int exponent, precision, i;

   v = arguments->values[argc];
   t = arguments->types[argc];
   if (t != mulle_sprintf_double_argumenttype) {
      auto char format[64], res[128];
      produce_format_string(format, info, 1);
      v.ld = make_non_negative_zero_long_double_if_zero(v.ld);
      snprintf(res, 128, format, v.ld);
      mulle_buffer_add_string(buffer, res);
      return 0;
   }

   v.d = make_non_negative_zero_double_if_zero(v.d);
   bits.d = v.d;
   mantissa = bits.u & 0xFFFFFFFFFFFFFULL;
   exponent = (bits.u >> 52) & 0x7FF;
   
   p = result;
   if (bits.u >> 63) *p++ = '-';
   else if (info->memory.plus_found) *p++ = '+';
   else if (info->memory.space_found) *p++ = ' ';
   
   if (exponent == 0x7FF) {
      if (mantissa) mulle_buffer_add_string(buffer, "nan");
      else mulle_buffer_add_string(buffer, bits.u >> 63 ? "-inf" : "inf");
      return 0;
   }
   
   *p++ = '0';
   *p++ = info->conversion == 'A' ? 'X' : 'x';
   precision = info->memory.precision_found ? info->precision : 13;
   
   if (exponent == 0 && mantissa == 0) {
      *p++ = '0';
      if (precision > 0 || info->memory.hash_found) {
         *p++ = '.';
         for (i = 0; i < precision; i++) *p++ = '0';
      }
      *p++ = 'p';
      *p++ = '+';
      *p++ = '0';
   } else {
      if (exponent == 0) { *p++ = '0'; exponent = 1; }
      else { *p++ = '1'; mantissa |= 0x10000000000000ULL; }
      if (precision > 0 || info->memory.hash_found) {
         *p++ = '.';
         for (i = 0; i < precision && i < 13; i++) {
            int nibble = (mantissa >> (48 - i * 4)) & 0xF;
            *p++ = nibble < 10 ? '0' + nibble : (info->conversion == 'A' ? 'A' : 'a') + nibble - 10;
         }
         for (; i < precision; i++) *p++ = '0';
      }
      *p++ = 'p';
      exponent -= 1023;
      *p++ = exponent >= 0 ? '+' : '-';
      if (exponent < 0) exponent = -exponent;
      if (exponent >= 1000) *p++ = '0' + (exponent / 1000);
      if (exponent >= 100) *p++ = '0' + ((exponent / 100) % 10);
      if (exponent >= 10) *p++ = '0' + ((exponent / 10) % 10);
      *p++ = '0' + (exponent % 10);
   }
   *p = '\0';
   mulle_buffer_add_string(buffer, result);
   return 0;
}

static struct mulle_sprintf_function mulle_sprintf_fp_dtoa_functions = {
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_e_conversion
};

static struct mulle_sprintf_function mulle_sprintf_fp_dtoa_f_functions = {
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_f_conversion
};

static struct mulle_sprintf_function mulle_sprintf_fp_dtoa_g_functions = {
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_g_conversion
};

static struct mulle_sprintf_function mulle_sprintf_fp_dtoa_a_functions = {
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_a_conversion
};

#endif


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
         // avoid negative -0.0 on BSDs which leads to test output that
         // doesn't diff
         v.d = make_non_negative_zero_double_if_zero( v.d);
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
         v.ld = make_non_negative_zero_long_double_if_zero( v.ld);
         snprintf( result, sizeof( result), format, v.ld);
      }
   }
   mulle_buffer_add_string_with_maxlength( buffer, result, sizeof( result));
   return( 0);
}



static mulle_sprintf_argumenttype_t  _mulle_sprintf_get_fp_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   assert( info->width < 64);
   assert( info->precision < 64);

   if( info->modifier[ 0] == 'L')
      return( mulle_sprintf_long_double_argumenttype);
   assert( info->modifier[ 0] == '\0');
   return( mulle_sprintf_double_argumenttype);
}


static struct mulle_sprintf_function     mulle_sprintf_fp_functions =
{
   _mulle_sprintf_get_fp_argumenttype,
   _mulle_sprintf_fp_conversion
};



void  mulle_sprintf_register_fp_functions( struct mulle_sprintf_conversion *tables)
{
#ifndef NO_MULLE__DTOA
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_a_functions, 'a');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_functions, 'e');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_f_functions, 'f');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_g_functions, 'g');

   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_a_functions, 'A');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_functions, 'E');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_f_functions, 'F');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_dtoa_g_functions, 'G');
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

