/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  _NSCharacter_conversion.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "mulle_sprintf_character.h"

#include "mulle_sprintf_function.h"
#include "mulle_sprintf_string.h"
#include "mulle_sprintf.h"



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


int   mulle_sprintf_character_conversion( struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc)
{
   union mulle_sprintf_argumentvalue  v;
   
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
   
   return( mulle_sprintf_wcharstring_conversion( buffer, info, s));
}


int   mulle_sprintf_wide_character_conversion( struct mulle_buffer *buffer,
                                               struct mulle_sprintf_formatconversioninfo *info,
                                               struct mulle_sprintf_argumentarray *arguments,
                                               int argc)
{
   union mulle_sprintf_argumentvalue  v;
   
   v = arguments->values[ argc];
   return( _wide_char_string_conversion( info, buffer, v.wc));
}


mulle_sprintf_argumenttype_t  mulle_sprintf_get_widecharacter_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   return( mulle_sprintf_wint_t_argumenttype);
}


mulle_sprintf_argumenttype_t  mulle_sprintf_get_character_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   if( info->modifier[ 0] == 'l')
      return( mulle_sprintf_wint_t_argumenttype);
   return( mulle_sprintf_char_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_character_functions = 
{
   mulle_sprintf_get_character_argumenttype,
   mulle_sprintf_character_conversion
};


struct mulle_sprintf_function     mulle_sprintf_widecharacter_functions = 
{
   mulle_sprintf_get_widecharacter_argumenttype,
   mulle_sprintf_wide_character_conversion
};



void  _mulle_sprintf_register_character_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_character_functions, 'c');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_widecharacter_functions, 'C');
   mulle_sprintf_register_modifier( tables, 'l');
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_character_functions()
{
  _mulle_sprintf_register_character_functions( &mulle_sprintf_get_config()->defaultconversion);
}
   
