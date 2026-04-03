//
//  mulle_sprintf_character.c
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

#include "mulle-sprintf-character.h"

#include "mulle-sprintf-function.h"
#include "mulle-sprintf-string.h"
#include "mulle-sprintf.h"

#include <stddef.h>

// Forward declaration
static inline int   _utf32_char_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                            struct mulle_buffer *buffer,
                                            unsigned int c);

static inline int   _char_string_conversion( struct mulle_buffer *buffer,
                                             struct mulle_sprintf_formatconversioninfo *info,
                                             char c)
{
   char   width_char;

   assert( info);
   assert( buffer);

   width_char = info->memory.zero_found ? '0' : ' ';
   if( info->width > 1)
      mulle_buffer_memset( buffer, width_char, info->width - 1);
   mulle_buffer_add_byte( buffer, c);
   return( 0);
}


static inline int   _wide_char_string_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                                  struct mulle_buffer *buffer,
                                                  int32_t c)
   __attribute__((unused));

static inline int   _wide_char_string_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                                  struct mulle_buffer *buffer,
                                                  int32_t c)
{
   return( _utf32_char_conversion( info, buffer, c));
}


static inline int   _utf16_char_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                            struct mulle_buffer *buffer,
                                            unsigned int c)
   __attribute__((unused));

static inline int   _utf16_char_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                            struct mulle_buffer *buffer,
                                            unsigned int c)
{
   mulle_utf16_t   s[ 2];

   s[ 0] = c;
   s[ 1] = 0;

   return( _mulle_sprintf_utf16_conversion( buffer, info, s));
}


static inline int   _utf32_char_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                            struct mulle_buffer *buffer,
                                            unsigned int c)
{
   mulle_utf32_t   s[ 2];

   s[ 0] = c;
   s[ 1] = 0;

   return( _mulle_sprintf_utf32_conversion( buffer, info, s));
}


static int   _mulle_sprintf_lowercase_c_conversion( struct mulle_buffer *buffer,
                                                    struct mulle_sprintf_formatconversioninfo *info,
                                                    struct mulle_sprintf_argumentarray *arguments,
                                                    int argc)
{
   union mulle_sprintf_argumentvalue  v;

   assert( buffer);
   assert( info);
   assert( arguments);

   v = arguments->values[ argc];
#ifdef HAVE_MULLE_UTF
   if( info->modifier[ 0] == 'l')
      return( _wide_char_string_conversion( info, buffer, v.i32));
#endif
   return( _char_string_conversion( buffer, info, v.c));
}


static int   _mulle_sprintf_uppercase_C_conversion( struct mulle_buffer *buffer,
                                                    struct mulle_sprintf_formatconversioninfo *info ,
                                                    struct mulle_sprintf_argumentarray *arguments,
                                                    int argc)
{
   union mulle_sprintf_argumentvalue  v;

   assert( buffer);
   assert( info);
   assert( arguments);

   v = arguments->values[ argc];
#ifdef HAVE_MULLE_UTF
   if( info->modifier[ 0] == 'h')
      return( _utf16_char_conversion( info, buffer, v.I));
#endif
   // %C and %lC both use UTF-32
   return( _utf32_char_conversion( info, buffer, v.I));
}


static mulle_sprintf_argumenttype_t  mulle_sprintf_get_uppercase_C_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
#ifdef HAVE_MULLE_UTF
   if( info->modifier[ 0] == 'h')
   {
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_unsigned_int_argumenttype);
   }
#endif
   // %C and %lC both use UTF-32
   return( mulle_sprintf_unsigned_int_argumenttype);
}


static mulle_sprintf_argumenttype_t  mulle_sprintf_get_lowercase_c_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
#ifdef HAVE_MULLE_UTF
   if( info->modifier[ 0] == 'l')
   {
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_wint_t_argumenttype);
   }
#endif
   return( mulle_sprintf_char_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_lowercase_c_functions =
{
   mulle_sprintf_get_lowercase_c_argumenttype,
   _mulle_sprintf_lowercase_c_conversion
};


struct mulle_sprintf_function     mulle_sprintf_uppercase_C_functions =
{
   mulle_sprintf_get_uppercase_C_argumenttype,
   _mulle_sprintf_uppercase_C_conversion
};



void  mulle_sprintf_register_character_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_lowercase_c_functions, 'c');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_uppercase_C_functions, 'C');
   mulle_sprintf_register_modifiers( tables, "hl");
}


//MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_character_functions)
// static void  mulle_sprintf_register_default_character_functions()
// {
//    mulle_sprintf_register_character_functions( mulle_sprintf_get_defaultconversion());
// }

