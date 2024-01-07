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


static int   _mulle_sprintf_character_conversion( struct mulle_buffer *buffer,
                                                  struct mulle_sprintf_formatconversioninfo *info,
                                                  struct mulle_sprintf_argumentarray *arguments,
                                                  int argc)
{
   union mulle_sprintf_argumentvalue  v;

   assert( buffer);
   assert( info);
   assert( arguments);

   v = arguments->values[ argc];
   return( _char_string_conversion( buffer, info, v.c));
}


static inline int   _wide_char_string_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                                  struct mulle_buffer *buffer,
                                                  wint_t c)
{
   wchar_t   s[ 2];

   s[ 0] = c;
   s[ 1] = 0;

   return( _mulle_sprintf_wcharstring_conversion( buffer, info, s));
}


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


static int   _mulle_sprintf_wide_character_conversion( struct mulle_buffer *buffer,
                                                       struct mulle_sprintf_formatconversioninfo *info ,
                                                       struct mulle_sprintf_argumentarray *arguments,
                                                       int argc)
{
   union mulle_sprintf_argumentvalue  v;

   assert( buffer);
   assert( info);
   assert( arguments);

   v = arguments->values[ argc];
   if( info->modifier[ 0] == 'h')
      return( _utf16_char_conversion( info, buffer, v.I));  // conversion to int from uint16_t
   if( info->modifier[ 0] == 'l')
      return( _utf32_char_conversion( info, buffer, v.I));  // conversion to int from uint16_t
   return( _wide_char_string_conversion( info, buffer, v.wc));
}


static mulle_sprintf_argumenttype_t  mulle_sprintf_get_widecharacter_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   if( info->modifier[ 0] == 'h')
   {
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_unsigned_int_argumenttype);  // conversion to int from uint16_t
   }
   if( info->modifier[ 0] == 'l')
   {
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_unsigned_int_argumenttype);  // conversion to int from uint32_t
   }
   assert( info->modifier[ 0] == '\0');
   return( mulle_sprintf_wint_t_argumenttype);
}


static mulle_sprintf_argumenttype_t  _mulle_sprintf_get_character_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   if( info->modifier[ 0] == 'l')
   {
      assert( info->modifier[ 1] == '\0');
      return( mulle_sprintf_wint_t_argumenttype);
   }
   assert( info->modifier[ 0] == '\0');
   return( mulle_sprintf_char_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_character_functions =
{
   _mulle_sprintf_get_character_argumenttype,
   _mulle_sprintf_character_conversion
};


struct mulle_sprintf_function     mulle_sprintf_widecharacter_functions =
{
   mulle_sprintf_get_widecharacter_argumenttype,
   _mulle_sprintf_wide_character_conversion
};



void  mulle_sprintf_register_character_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_character_functions, 'c');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_widecharacter_functions, 'C');
   mulle_sprintf_register_modifiers( tables, "hl");
}


//MULLE_C_CONSTRUCTOR( mulle_sprintf_register_default_character_functions)
// static void  mulle_sprintf_register_default_character_functions()
// {
//    mulle_sprintf_register_character_functions( mulle_sprintf_get_defaultconversion());
// }

