//
//  mulle_sprintf_integer.c
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

#include "mulle-sprintf-integer.h"

#include "mulle-sprintf.h"


typedef struct
{
   char  *(*convert_unsigned_int)( unsigned int value, char *s);
   char  *(*convert_locale_unsigned_int)( unsigned int value, char *s);
   char  *(*convert_unsigned_long_long)( unsigned long long value, char *s);
   char  *(*convert_locale_unsigned_long_long)( unsigned long long value, char *s);
   int   (*set_prefix)( char *s, int value_is_zero, int length, int precision);
} integer_converters;


#pragma mark - decimal

static int   set_decimal_prefix( char *s,
                                 int value_is_zero,
                                 int length,
                                 int precision)
{
   return( 0);
}


static char  *convert_decimal_unsigned_int( unsigned int  value,
                                            char *s)
{
   while( value)
   {
      *--s  = '0' + value % 10;
      value /= 10;
   }

   return( s);
}


static char  *convert_decimal_unsigned_long_long( unsigned long long value,
                                                  char *s)
{
   while( value)
   {
      *--s  = '0' + value % 10;
      value /= 10;
   }

   return( s);
}


static integer_converters  decimal_converters =
{
   convert_decimal_unsigned_int,
   convert_decimal_unsigned_int,
   convert_decimal_unsigned_long_long,
   convert_decimal_unsigned_long_long,
   set_decimal_prefix
};


void   _mulle_sprintf_justified( struct mulle_buffer *buffer,
                                 struct mulle_sprintf_formatconversioninfo *info,
                                 char *p,
                                 int p_length,
                                 char *q,
                                 int q_length,
                                 int  precision,
                                 char prefix)
{
   int       length;
   size_t    total;
   size_t    used;
   size_t    width;
   char      precision_char;
   char      width_char;

   assert( buffer);
   assert( info);
   assert( p);
   assert( ! q_length || q);

   length         = p_length + q_length;
   precision_char = '0';
   width          = info->width;
   width_char     = info->memory.zero_found ? '0' : ' ';

   if( info->memory.left_justify)
   {
      used = mulle_buffer_get_length( buffer);
      if( prefix)
         mulle_buffer_add_byte( buffer, prefix);
      if( q_length)
         mulle_buffer_add_bytes( buffer, q, q_length);
      if( precision > p_length)
         mulle_buffer_memset( buffer, precision_char, precision - p_length);
      mulle_buffer_add_bytes( buffer, p, p_length);
      used = mulle_buffer_get_length( buffer) - used;
      if( width > used)
         mulle_buffer_memset( buffer, width_char, width - used);
   }
   else
   {
      total = length;
      if( prefix)
         total += 1;
      if( precision > p_length)
         total += precision - p_length;
      if( width > total && width_char != '0')
         mulle_buffer_memset( buffer, width_char, width - total);
      if( prefix)
         mulle_buffer_add_byte( buffer, prefix);
      if( width > total && width_char == '0')
         mulle_buffer_memset( buffer, width_char, width - total);
      if( q_length)
         mulle_buffer_add_bytes( buffer, q, q_length);
      if( precision > length)
         mulle_buffer_memset( buffer, precision_char, precision - p_length);
      mulle_buffer_add_bytes( buffer, p, p_length);
   }
}


void   _mulle_sprintf_justified_and_prefixed( struct mulle_buffer *buffer,
                                              struct mulle_sprintf_formatconversioninfo *info,
                                              char *p,
                                              int p_length,
                                              char prefix,
                                              int is_zero,
                                              int (*set_prefix)( char *, int, int, int))
{
   int     precision;
   int     q_length;
   char    tmp2[ 4];

   assert( buffer);
   assert( info);
   assert( p);
   assert( set_prefix);

   precision = info->memory.precision_found ? info->precision : 1;
   q_length  = 0;

   if( info->memory.hash_found )
   {
      q_length = (*set_prefix)( tmp2, is_zero, p_length, precision);
      if( q_length < 0)
      {
         precision = p_length - q_length;  // octal hack not pretty
         q_length  = 0;
      }
   }

   _mulle_sprintf_justified( buffer,
                             info,
                             p,
                             p_length,
                             tmp2,
                             q_length,
                             precision,
                             prefix);
}


static int   integer_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                 struct mulle_buffer *buffer,
                                 struct mulle_sprintf_argumentarray *arguments,
                                 int argc,
                                 integer_converters *converters,
                                 int is_signed)
{
   union mulle_sprintf_argumentvalue   v;
   mulle_sprintf_argumenttype_t        t;
   size_t                              s;
   char                                prefix;
   char                                tmp[ sizeof( long long) * 4];
   char                                *p;
   ptrdiff_t                           p_length;
   unsigned long long                  vLL;
   long long                           vll;

   v = arguments->values[ argc];
   t = arguments->types[ argc];
   s = mulle_sprintf_argumentsize[ t];

   if( s >= sizeof( long))
   {
      if( s == sizeof( long))
         vll = v.l;
      else
         vll = v.ll;
   }
   else
   {
      if( s == sizeof( int))
         vll = v.i;
      else
         if( s == sizeof( short))
            vll = v.s;
         else
            vll = v.c;
   }

   vLL = (unsigned long long) vll;

   // bool output shortcut
   if( info->memory.bool_found)
   {
      if( vLL)
         mulle_buffer_add_bytes( buffer, "YES", 3);
      else
         mulle_buffer_add_bytes( buffer, "NO", 2);
      return( 0);

   }

   prefix = 0;

   if( is_signed)
   {
      if( vll < 0)
      {
         vLL     = -vll;
         prefix  = '-';
      }
      else
         if( info->memory.plus_found)
            prefix = '+';
         else
            if( info->memory.space_found)
               prefix  = ' ';
   }

   p = &tmp[ sizeof( tmp)];

   // place ',' appropriately
   if( info->memory.quote_found)
   {
      if( s == sizeof( int))
         p = (*converters->convert_locale_unsigned_int)( (unsigned int) vLL, p);
      else
         p = (*converters->convert_locale_unsigned_long_long)( vLL, p);
   }
   else
      if( s == sizeof( int))
         p = (*converters->convert_unsigned_int)( (unsigned int) vLL, p);
      else
         p = (*converters->convert_unsigned_long_long)( vLL, p);

   // ok we gotz da digit, now build it up, from the front
   p_length = &tmp[ sizeof( tmp)] - p;

   // p is a the front now
   _mulle_sprintf_justified_and_prefixed( buffer, info,
                                                  p,
                                                  (int) p_length,
                                                  prefix,
                                                  vLL == 0,
                                                  converters->set_prefix);

   return( 0);
}


static int
   _mulle_sprintf_int_decimal_conversion( struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 1));
}


static int
   _mulle_sprintf_unsigned_int_decimal_conversion( struct mulle_buffer *buffer,
                                                   struct mulle_sprintf_formatconversioninfo *info,
                                                   struct mulle_sprintf_argumentarray *arguments,
                                                   int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 0));
}


#pragma mark - octal


static char  *convert_octal_unsigned_int( unsigned int value,
                                          char *s)
{
   while( value)
   {
      *--s  = '0' + (value & 0x7);
      value >>= 3;
   }

   return( s);
}


static char  *convert_octal_unsigned_long_long( unsigned long long value,
                                                char *s)
{
   while( value)
   {
      *--s  = '0' + (value & 0x7);
      value >>= 3;
   }

   return( s);
}


static int   set_octal_prefix( char *s, int value_is_zero, int length, int precision)
{
   if( length && precision <= length)
      return( -1);      // increase precision
   return( 0);
}



static integer_converters  octal_converters =
{
   convert_octal_unsigned_int,
   convert_octal_unsigned_int,
   convert_octal_unsigned_long_long,
   convert_octal_unsigned_long_long,
   set_octal_prefix
};


static int
   _mulle_sprintf_int_octal_conversion( struct mulle_buffer *buffer,
                                        struct mulle_sprintf_formatconversioninfo *info,
                                        struct mulle_sprintf_argumentarray *arguments,
                                        int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &octal_converters, 0));
}


#pragma mark - hex


static char   *convert_hex_unsigned_int( unsigned int value,
                                         char *s)
{
   char   v;

   while( value)
   {
      v     = (value & 0xF);
      *--s  = (v >= 10) ? ('a' - 10 + v) : '0' + v;
      value >>= 4;
   }

   return( s);
}


static char   *convert_hex_unsigned_long_long( unsigned long long value,
                                               char *s)
{
   unsigned char   v;

   while( value)
   {
      v     = (unsigned char) value & 0xF;
      *--s  = (v >= 10) ? ('a' - 10 + v) : '0' + v;
      value >>= 4;
   }

   return( s);
}


static int   set_hex_prefix( char *s, int value_is_zero, int length, int precision)
{
   if( length)
   {
      *s    = '0';
      s[ 1] = 'x';
      return( 2);
   }
   return( 0);
}


static integer_converters  hex_converters =
{
   convert_hex_unsigned_int,
   convert_hex_unsigned_int,
   convert_hex_unsigned_long_long,
   convert_hex_unsigned_long_long,
   set_hex_prefix
};


static int
   _mulle_sprintf_int_hex_conversion( struct mulle_buffer *buffer,
                                      struct mulle_sprintf_formatconversioninfo *info,
                                      struct mulle_sprintf_argumentarray *arguments,
                                      int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &hex_converters, 0));
}


#pragma mark - long conversions


static int
   _mulle_sprintf_long_decimal_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           struct mulle_sprintf_argumentarray *arguments,
                                           int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 1));
}



static int
   _mulle_sprintf_unsigned_long_decimal_conversion( struct mulle_buffer *buffer,
                                                    struct mulle_sprintf_formatconversioninfo *info,
                                                    struct mulle_sprintf_argumentarray *arguments,
                                                    int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 1));
}


int
   _mulle_sprintf_long_hex_conversion( struct mulle_buffer *buffer,
                                       struct mulle_sprintf_formatconversioninfo *info,
                                       struct mulle_sprintf_argumentarray *arguments,
                                       int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &hex_converters, 0));
}


static int
   _mulle_sprintf_long_octal_conversion( struct mulle_buffer *buffer,
                                         struct mulle_sprintf_formatconversioninfo *info,
                                         struct mulle_sprintf_argumentarray *arguments,
                                         int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &octal_converters, 0));
}


static mulle_sprintf_argumenttype_t
  _mulle_sprintf_get_signed_int_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   switch( info->modifier[ 0])
   {
   case 'h' :
      if( info->modifier[ 1] == 'h')
         return( mulle_sprintf_char_argumenttype);
      return( mulle_sprintf_short_argumenttype);

   case 'l' :
      if( info->modifier[ 1] == 'l')
         return( mulle_sprintf_long_long_argumenttype);
      return( mulle_sprintf_long_argumenttype);

   case 'j' : return( mulle_sprintf_intmax_t_argumenttype);
   case 'q' : return( mulle_sprintf_int64_t_argumenttype);
   case 't' : return( mulle_sprintf_ptrdiff_t_argumenttype);
   }
   return( mulle_sprintf_int_argumenttype);
}


static mulle_sprintf_argumenttype_t
  mulle_sprintf_get_unsigned_int_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   switch( info->modifier[ 0])
   {
   case 'h' :
      if( info->modifier[ 1] == 'h')
         return( mulle_sprintf_unsigned_char_argumenttype);
      return( mulle_sprintf_unsigned_short_argumenttype);

   case 'l' :
      if( info->modifier[ 1] == 'l')
         return( mulle_sprintf_unsigned_long_long_argumenttype);
      return( mulle_sprintf_unsigned_long_argumenttype);

   case 'j' : return( mulle_sprintf_uintmax_t_argumenttype);
   case 'q' : return( mulle_sprintf_uint64_t_argumenttype);
   case 't' : return( mulle_sprintf_unsigned_ptrdiff_t_argumenttype);
   case 'z' : return( mulle_sprintf_size_t_argumenttype);
   }
   return( mulle_sprintf_unsigned_int_argumenttype);
}


static struct mulle_sprintf_function   mulle_sprintf_int_decimal_functions =
{
   _mulle_sprintf_get_signed_int_argumenttype,
   _mulle_sprintf_int_decimal_conversion
};


static struct mulle_sprintf_function     mulle_sprintf_unsigned_int_decimal_functions =
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   _mulle_sprintf_unsigned_int_decimal_conversion
};


static struct mulle_sprintf_function   mulle_sprintf_int_octal_functions =
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   _mulle_sprintf_int_octal_conversion
};


static struct mulle_sprintf_function   mulle_sprintf_int_hex_functions =
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   _mulle_sprintf_int_hex_conversion
};


static struct mulle_sprintf_function   mulle_sprintf_long_decimal_functions =
{
   _mulle_sprintf_get_signed_int_argumenttype,
   _mulle_sprintf_long_decimal_conversion
};


static struct mulle_sprintf_function   mulle_sprintf_unsigned_long_decimal_functions =
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   _mulle_sprintf_unsigned_long_decimal_conversion
};


static struct mulle_sprintf_function   mulle_sprintf_long_hex_functions =
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   _mulle_sprintf_long_hex_conversion
};


static struct mulle_sprintf_function     mulle_sprintf_long_octal_functions =
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   _mulle_sprintf_long_octal_conversion
};


void   mulle_sprintf_register_integer_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_int_decimal_functions, 'i');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_int_decimal_functions, 'd');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_long_decimal_functions, 'D');

   mulle_sprintf_register_functions( tables, &mulle_sprintf_unsigned_int_decimal_functions, 'u');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_int_octal_functions, 'o');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_int_hex_functions, 'x');

   mulle_sprintf_register_functions( tables, &mulle_sprintf_unsigned_long_decimal_functions, 'U');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_long_octal_functions, 'O');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_long_hex_functions, 'X');

   mulle_sprintf_register_modifiers( tables, "hljtzq");
}


// MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_integer_functions)
// static void  mulle_sprintf_register_default_integer_functions()
// {
//    mulle_sprintf_register_integer_functions( mulle_sprintf_get_defaultconversion());
// }

