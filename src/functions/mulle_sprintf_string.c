/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_tring_conversion.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "mulle_sprintf_string.h"

// other files in this library

// std-c and dependencies
#include <mulle_container/mulle_container.h>
#include <mulle_utf/mulle_utf.h>
#include <string.h>



static int   mulle_sprintf_utf16conversion( struct mulle_buffer *buffer,
                                             struct mulle_sprintf_formatconversioninfo *info,
                                             mulle_utf16_t *s)
{
   int                    length;
   int                    o_length;
   size_t                 before;
   static mulle_utf16_t   null_description[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };
   
   if( ! s)
      s = null_description;
   
   before = mulle_buffer_get_length( buffer);
   
   o_length = (int) mulle_utf16_strlen( s);
   length   = (int) o_length;
   if( info->memory.precision_found)
      length = (info->precision > o_length) ? o_length  : info->precision;
   
   // left justify or no width is faster
   if( info->memory.left_justify || ! info->width)
   {
      mulle_utf16_convert_to_utf8_bytebuffer( s, length, buffer, (void *) mulle_buffer_add_bytes);
      
      if( info->width > length)
         mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);
      return( 0);
   }
   
   if( info->width > length)
      mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);
   
   mulle_utf16_convert_to_utf8_bytebuffer( s, length, buffer,  (void *) mulle_buffer_add_bytes);
   return( 0);
}


static int   mulle_sprintf_utf32conversion( struct mulle_buffer *buffer,
                                            struct mulle_sprintf_formatconversioninfo *info,
                                            mulle_utf32_t *s)
{
   int                    length;
   int                    o_length;
   size_t                 before;
   static mulle_utf32_t   null_description[] = { '(', 'n', 'u', 'l', 'l', ')', 0 };
   
   if( ! s)
      s = null_description;
   
   before = mulle_buffer_get_length( buffer);
   
   o_length = (int) mulle_utf32_strlen( s);
   length   = (int) o_length;
   if( info->memory.precision_found)
      length = (info->precision > o_length) ? o_length  : info->precision;
   
   // left justify or no width is faster
   if( info->memory.left_justify || ! info->width)
   {
      mulle_utf32_convert_to_utf8_bytebuffer( s, length, buffer, (void *) mulle_buffer_add_bytes);
      
      if( info->width > length)
         mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);
      return( 0);
   }
   
   if( info->width > length)
      mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);
   
   mulle_utf32_convert_to_utf8_bytebuffer( s, length, buffer, (void *) mulle_buffer_add_bytes);
   return( 0);
}


int   mulle_sprintf_wcharstring_conversion( struct mulle_buffer *buffer,
                                            struct mulle_sprintf_formatconversioninfo *info,
                                            wchar_t  *s)
{
   if( sizeof( wchar_t) == sizeof( mulle_utf16_t))
      return( mulle_sprintf_utf16conversion( buffer, info, (mulle_utf16_t *) s));
   return( mulle_sprintf_utf32conversion( buffer, info, (mulle_utf32_t *) s));
}


int   mulle_sprintf_widestring_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           struct mulle_sprintf_argumentarray *arguments,
                                           int argc)
{
   union mulle_sprintf_argumentvalue  v;
   
   v = arguments->values[ argc];
   return( mulle_sprintf_wcharstring_conversion( buffer, info, v.pwc));
}


int   mulle_sprintf_charstring_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           char *s)
{ 
   size_t     before;
   ptrdiff_t  length;
   
   if( ! s)
      s = "(null)";
      
   before = mulle_buffer_get_length( buffer);
   
   // left justify or no width is faster
   if( info->memory.left_justify || ! info->width)
   {
      if( info->memory.precision_found)
         mulle_buffer_add_string_with_maxlength( buffer, s, info->precision);
      else
         mulle_buffer_add_string( buffer, s);
      length = mulle_buffer_get_length( buffer) - before;
      if( info->width > length)
         mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);
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


int   mulle_sprintf_string_conversion( struct mulle_buffer *buffer,
                                       struct mulle_sprintf_formatconversioninfo *info,
                                       struct mulle_sprintf_argumentarray *arguments,
                                       int argc)
{
   union mulle_sprintf_argumentvalue  v;
   mulle_sprintf_argumenttype_t       t;
   
   t = arguments->types[ argc];
   if( t == mulle_sprintf_wchar_pointer_argumenttype)
      return( mulle_sprintf_widestring_conversion( buffer, info, arguments, argc));

   v = arguments->values[ argc];
   return( mulle_sprintf_charstring_conversion( buffer, info, v.pc));
}                   


mulle_sprintf_argumenttype_t  mulle_sprintf_get_widestring_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   return( mulle_sprintf_wchar_pointer_argumenttype);
}


mulle_sprintf_argumenttype_t  mulle_sprintf_get_string_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   if( info->modifier[ 0] == 'l')
      return( mulle_sprintf_wchar_pointer_argumenttype);
   return( mulle_sprintf_char_pointer_argumenttype);
}


struct mulle_sprintf_function     mulle_string_functions = 
{
   mulle_sprintf_get_string_argumenttype,
   mulle_sprintf_string_conversion   
};


struct mulle_sprintf_function     mulle_widestring_functions = 
{
   mulle_sprintf_get_widestring_argumenttype,
   mulle_sprintf_widestring_conversion
};


void  _mulle_sprintf_register_string_functions( struct mulle_sprintf_conversion *tables) 
{
   _mulle_sprintf_register_functions( tables, &mulle_string_functions,'s');
   _mulle_sprintf_register_functions( tables, &mulle_widestring_functions, 'S');
   _mulle_sprintf_register_modifier( tables, 'l');
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_string_functions()
{
  _mulle_sprintf_register_string_functions( &mulle_sprintf_defaultconversion);
}
