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

#include <string.h>


static int   no_conversion( struct mulle_buffer *buffer,
                          struct mulle_sprintf_formatconversioninfo *info,
                          wchar_t *s)
{
   return( -1);
}


int  (*mulle_sprintfsystemwidestring_conversion)( struct mulle_buffer *buffer,
                                             struct mulle_sprintf_formatconversioninfo *info,
                                             wchar_t *s) = no_conversion;



// push it to unicode ...
static int   wstring_conversion( struct mulle_buffer *buffer,
                                 struct mulle_sprintf_formatconversioninfo *info,
                                 struct mulle_sprintf_argumentarray *arguments,
                                 int argc)
{
   union mulle_sprintf_argumentvalue   v;
   wchar_t                   *ws;
   
   v  = arguments->values[ argc];
   ws = v.pwc;
   
   if( ! info->memory.width_found && ! info->memory.precision_found)
      info = NULL;
   
   return( (*mulle_sprintfsystemwidestring_conversion)( buffer, info, ws));
}             


int   mulle_sprintf_cstring_conversion( struct mulle_buffer *buffer,
                                        struct mulle_sprintf_formatconversioninfo *info,
                                        char *s)
{ 
   size_t      before;
   ptrdiff_t   length;
   
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
      length = (ptrdiff_t) (mulle_buffer_get_length( buffer) - before);
      if( info->width > length)
         mulle_buffer_memset( buffer, info->memory.zero_found ? '0' : ' ', info->width - length);
      return( 0);
   }

   length = strlen( s);
   if( info->memory.precision_found)
      length = info->precision > (int) length ? length : info->precision;
      
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
   mulle_sprintf_argumenttype_t   t;
   char                     *s;
   
   t = arguments->types[ argc];
   if( t == mulle_sprintf_wchar_pointer_argumenttype)
      return( wstring_conversion( buffer, info, arguments, argc));
      
   v = arguments->values[ argc];
   s = v.pc;
   
   return( mulle_sprintf_cstring_conversion( buffer, info, s));
}                   


mulle_sprintf_argumenttype_t  mulle_sprintf_get_systemwidestring_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
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


struct mulle_sprintf_function     mulle_systemwidestring_functions = 
{
   mulle_sprintf_get_systemwidestring_argumenttype,
   mulle_sprintf_string_conversion
};


void  _mulle_sprintf_register_string_functions( struct mulle_sprintf_conversion *tables) 
{
   _mulle_sprintf_register_functions( tables, &mulle_string_functions,'s');
   _mulle_sprintf_register_functions( tables, &mulle_systemwidestring_functions, 'S');
   _mulle_sprintf_register_modifier( tables, 'l');
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_string_functions()
{
  _mulle_sprintf_register_string_functions( &mulle_sprintf_defaultconversion);
}
