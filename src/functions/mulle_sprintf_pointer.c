/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_pointer_conversion.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "mulle_sprintf_pointer.h"
#include "mulle_sprintf_int.h"



//static int   object_conversion( struct mulle_sprintf_formatconversioninfo *info,
//                                struct mulle_buffer *buffer,
//                                struct mulle_sprintf_argumentarray *arguments,
//                                int argc)
//{
//   union mulle_sprintf_argumentvalue  v;
//   
//   v = arguments->values[ argc];
//   if( ! info->memory.width_found && ! info->memory.precision_found)
//      info = NULL;
//   return( (*struct mulle_bufferObject_conversion)( info, buffer, v.obj));
//}


int  mulle_sprintf_pointer_conversion( struct mulle_buffer *buffer,
                                  struct mulle_sprintf_formatconversioninfo *info,
                                  struct mulle_sprintf_argumentarray *arguments,
                                  int argc)
{
   info->memory.hash_found = 1;  
   return( mulle_sprintf_long_hex_conversion( buffer, info, arguments, argc));
}                   



mulle_sprintf_argumenttype_t  mulle_sprintf_get_pointer_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   return( mulle_sprintf_void_pointer_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_pointer_functions = 
{
   mulle_sprintf_get_pointer_argumenttype,
   mulle_sprintf_pointer_conversion   
};



void  _mulle_sprintf_register_pointer_functions( struct mulle_sprintf_conversion *tables)
{
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_pointer_functions, 'p');
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_pointer_functions()
{
  _mulle_sprintf_register_pointer_functions( &mulle_sprintf_defaultconversion);
}
   

