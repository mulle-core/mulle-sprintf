/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_escape_functions.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, __MyCompanyName__ 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */

#include "mulle_sprintf_escape.h"


int   mulle_sprintf_escape_conversion( struct mulle_buffer *buffer,
                                  struct mulle_sprintf_formatconversioninfo *info,
                                  struct mulle_sprintf_argumentarray *arguments,
                                  int argc)
{
   char   width_char;
   
   width_char = info->memory.zero_found ? '0' : ' ';
   if( info->width > 1)
      mulle_buffer_memset( buffer, width_char, info->width - 1);
   mulle_buffer_add_byte( buffer, '%');
   return( 0);
}                   



mulle_sprintf_argumenttype_t  mulle_sprintf_get_escape_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   return( mulle_sprintf_void_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_escape_functions = 
{
   mulle_sprintf_get_escape_argumenttype,
   mulle_sprintf_escape_conversion   
};



void  _mulle_sprintf_register_escape_functions( struct mulle_sprintf_conversion *tables)
{
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_escape_functions, '%');
}


__attribute__((constructor))
static void  mulle_sprintf_register__default_escape_functions()
{
  _mulle_sprintf_register_escape_functions( &mulle_sprintf_defaultconversion);
}
   

