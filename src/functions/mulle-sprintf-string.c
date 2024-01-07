//
//  mulle_sprintf_string.c
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

#include "mulle-sprintf-string.h"

// other files in this library
#include "mulle-sprintf.h"

// std-c and dependencies
#include "include-private.h"
#include <string.h>

// make this easier to read
#define HAVE_MULLE_UTF
#ifdef DONT_HAVE_MULLE_UTF
# undef HAVE_MULLE_UTF
#endif

// if no widechar support
#ifdef HAVE_MULLE_UTF
#include "include-private.h"

#if MULLE__UTF_VERSION < ((1 << 20) | (0 << 8) | 7)
# error "mulle_utf is too old"
#endif


int
   _mulle_sprintf_utf8_conversion( struct mulle_buffer *buffer,
                                   struct mulle_sprintf_formatconversioninfo *info,
                                   char *s)
{
   int                            rval;
   struct mulle_utf_information   utfinfo;

   assert( buffer);
   assert( info);

   if( s)
   {
      if( mulle_utf8_information( s, (size_t) -1, &utfinfo))
      {
         errno = EINVAL;
         return( -1);
      }

      // check if we have composed multi-chars, if yes convert to utf32 and go
      // there, because otherwise the length/width calculations get too involved
      if( ! utfinfo.is_ascii)
      {
         mulle_alloca_do( tmp, mulle_utf32_t, utfinfo.utf32len + 1)
         {
            _mulle_utf8_convert_to_utf32( s, utfinfo.utf8len + 1, tmp);
            rval = _mulle_sprintf_utf32_conversion( buffer, info, tmp);
         }
         return( rval);
      }
   }

   return( _mulle_sprintf_charstring_conversion( buffer, info, (char *) s));
}


int
   _mulle_sprintf_utf16_conversion( struct mulle_buffer *buffer,
                                    struct mulle_sprintf_formatconversioninfo *info,
                                    mulle_utf16_t *s)
{
   int                    length;
   int                    o_length;
   int                    rval;
   static mulle_utf16_t   null_description[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };
   mulle_utf32_t          tmp_stack[ 32];
   mulle_utf32_t          *tmp;
   mulle_utf32_t          *tmp_malloc;

   assert( buffer);
   assert( info);

   if( ! s)
      s = null_description;

   // check if we have composed multi-chars, if yes convert to utf32 and go
   // there, because otherwise the length/width calculations get too involved
   o_length = (int) mulle_utf16_strlen( s);
   if( ! mulle_utf16_is_utf15( s, o_length))
   {
      // MEMO: don't want mulle-container as a dependency here, so do it
      // the hard way
      // mulle_flexarray_do( tmp, uint32_t, 32, o_length + 1)
      tmp_malloc = NULL;
      tmp        = tmp_stack;
      if( o_length >= sizeof( tmp_stack) / sizeof( uint32_t))
      {
         tmp_malloc = mulle_malloc( (o_length + 1) * sizeof( uint32_t));
         tmp        = tmp_malloc;
      }
      _mulle_utf16_convert_to_utf32( s, o_length + 1, tmp);
      rval = _mulle_sprintf_utf32_conversion( buffer, info, tmp);
      mulle_free( tmp_malloc);
      return( rval);
   }

   // back to the usual program
   length = (int) o_length;
   if( info->memory.precision_found)
      length = (info->precision > o_length) ? o_length : info->precision;

   // alternate (quoted) ignores all other info (as output will vary in length)
   if( info->memory.hash_found)
   {
      mulle_utf16_bufferconvert_to_utf8( s, length, buffer, mulle_buffer_add_c_chars_callback);
      return( 0);
   }

   // left justify or no width is faster
   if( info->memory.left_justify || ! info->width)
   {
      mulle_utf16_bufferconvert_to_utf8( s, length, buffer, mulle_buffer_add_bytes_callback);

      if( info->width > length)
         mulle_buffer_memset( buffer, ' ', info->width - length);
      return( 0);
   }

   if( info->width > length)
      mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);

   mulle_utf16_bufferconvert_to_utf8( s, length, buffer, mulle_buffer_add_bytes_callback);
   return( 0);
}


int
   _mulle_sprintf_utf32_conversion( struct mulle_buffer *buffer,
                                    struct mulle_sprintf_formatconversioninfo *info,
                                    mulle_utf32_t *s)
{
   int                    length;
   int                    o_length;
   static mulle_utf32_t   null_description[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };

   if( ! s)
      s = null_description;

   o_length = (int) mulle_utf32_strlen( s);
   length   = (int) o_length;
   if( info->memory.precision_found)
      length = (info->precision > o_length) ? o_length  : info->precision;

   // alternate (quoted) ignores all other info (as output will vary in length)
   if( info->memory.hash_found)
   {
      mulle_utf32_bufferconvert_to_utf8( s, length, buffer, mulle_buffer_add_c_chars_callback);
      return( 0);
   }

   // left justify or no width is faster
   if( info->memory.left_justify || ! info->width)
   {
      mulle_utf32_bufferconvert_to_utf8( s, length, buffer, mulle_buffer_add_bytes_callback);

      if( info->width > length)
         mulle_buffer_memset( buffer, ' ', info->width - length);
      return( 0);
   }

   if( info->width > length)
      mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);

   mulle_utf32_bufferconvert_to_utf8( s, length, buffer, mulle_buffer_add_bytes_callback);
   return( 0);
}


int   _mulle_sprintf_wcharstring_conversion( struct mulle_buffer *buffer,
                                             struct mulle_sprintf_formatconversioninfo *info,
                                             wchar_t  *s)
{
   if( sizeof( wchar_t) == sizeof( mulle_utf16_t))
      return( _mulle_sprintf_utf16_conversion( buffer, info, (mulle_utf16_t *) s));
   return( _mulle_sprintf_utf32_conversion( buffer, info, (mulle_utf32_t *) s));
}


static int
   _mulle_sprintf_widestring_conversion( struct mulle_buffer *buffer,
                                         struct mulle_sprintf_formatconversioninfo *info,
                                         struct mulle_sprintf_argumentarray *arguments,
                                         int argc)
{
   union mulle_sprintf_argumentvalue  v;

   v = arguments->values[ argc];
#ifdef HAVE_MULLE_UTF
   if( info->modifier[ 0] == 'h')
   {
      if( info->modifier[ 1] == 'h')
         return( _mulle_sprintf_utf8_conversion( buffer, info, (char *) v.pC));
      return( _mulle_sprintf_utf16_conversion( buffer, info, (mulle_utf16_t *) v.pu16));
   }
   if( info->modifier[ 0] == 'l')
      return( _mulle_sprintf_utf32_conversion( buffer, info, (mulle_utf32_t *) v.pu32));
#endif
   return( _mulle_sprintf_wcharstring_conversion( buffer, info, v.pwc));
}


static mulle_sprintf_argumenttype_t
   mulle_sprintf_get_widestring_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
#ifdef HAVE_MULLE_UTF
   if( info->modifier[ 0] == 'h')
   {
      if( info->modifier[ 1] == 'h')
      {
         assert( info->modifier[ 2] == '\0');
         return( mulle_sprintf_unsigned_char_pointer_argumenttype);
      }

      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_uint16_t_pointer_argumenttype);
   }
   if( info->modifier[ 0] == 'l')
   {
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_uint32_t_pointer_argumenttype);
   }
#endif
   assert( info->modifier[ 0] == '\0');
   return( mulle_sprintf_wchar_pointer_argumenttype);
}


static struct mulle_sprintf_function     mulle_widestring_functions =
{
   mulle_sprintf_get_widestring_argumenttype,
   _mulle_sprintf_widestring_conversion
};

#endif


int   _mulle_sprintf_charstring_conversion( struct mulle_buffer *buffer,
                                            struct mulle_sprintf_formatconversioninfo *info,
                                            char *s)
{
   size_t     before;
   ptrdiff_t  length;

   if( ! s)
      s = "(null)";  // TODO: make this a global variable ?

   // alternate (quoted) ignores all other info (as output will vary in length)
   // but precision
   if( info->memory.hash_found)
   {
      length = (int) strlen( s);
      if( info->memory.precision_found)
      {
         if( length > info->precision)
            length = info->precision;
      }
      mulle_buffer_add_c_chars( buffer, s, length);
      return( 0);
   }

   // left justify or no width is faster
   if( info->memory.left_justify || ! info->width)
   {
      before = mulle_buffer_get_length( buffer);

      if( info->memory.precision_found)
         mulle_buffer_add_string_with_maxlength( buffer, s, info->precision);
      else
         mulle_buffer_add_string( buffer, s);
      length = mulle_buffer_get_length( buffer) - before;
      // as we are definitely left justified with -, 0 has no meaning
      // so just use space
      if( info->width > length)
         mulle_buffer_memset( buffer, ' ', info->width - length);
      return( 0);
   }

   length = (int) strlen( s);
   if( info->memory.precision_found)
      length = info->precision > length ? length : info->precision;

   if( info->width > length)
      mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);

   mulle_buffer_add_string_with_maxlength( buffer, s, length);
   return( 0);
}


static int   _mulle_sprintf_string_conversion( struct mulle_buffer *buffer,
                                               struct mulle_sprintf_formatconversioninfo *info,
                                               struct mulle_sprintf_argumentarray *arguments,
                                               int argc)
{
   union mulle_sprintf_argumentvalue  v;
   mulle_sprintf_argumenttype_t       t;

#ifdef HAVE_MULLE_UTF
   t = arguments->types[ argc];
   if( t == mulle_sprintf_wchar_pointer_argumenttype)
      return( _mulle_sprintf_widestring_conversion( buffer, info, arguments, argc));
#endif

   v = arguments->values[ argc];
   return( _mulle_sprintf_charstring_conversion( buffer, info, v.pc));
}


static mulle_sprintf_argumenttype_t  _mulle_sprintf_get_string_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
#ifdef HAVE_MULLE_UTF
   if( info->modifier[ 0] == 'l')
   {
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_wchar_pointer_argumenttype);
   }
#endif
   assert( info->modifier[ 0] == '\0');
   return( mulle_sprintf_char_pointer_argumenttype);
}


static struct mulle_sprintf_function     mulle_string_functions =
{
   _mulle_sprintf_get_string_argumenttype,
   _mulle_sprintf_string_conversion
};


void  mulle_sprintf_register_string_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_string_functions, 's');
#ifdef HAVE_MULLE_UTF
   mulle_sprintf_register_functions( tables, &mulle_widestring_functions, 'S');
   mulle_sprintf_register_modifiers( tables, "hl");
#endif
}


// MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_string_functions)
// static void  mulle_sprintf_register_default_string_functions()
// {
//    mulle_sprintf_register_string_functions( mulle_sprintf_get_defaultconversion());
// }
