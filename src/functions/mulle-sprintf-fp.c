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



static inline int   make_non_negative_zero_long_double_if_zero( long double x)
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
      // it is, which is inconsistent (do now want)
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
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'a');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'e');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'f');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'g');

   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'A');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'E');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'F');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'G');

   mulle_sprintf_register_modifier( tables, 'L');
}


// MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_fp_functions)
// static void  mulle_sprintf_register_default_fp_functions()
// {
//    mulle_sprintf_register_fp_functions( mulle_sprintf_get_defaultconversion());
// }

