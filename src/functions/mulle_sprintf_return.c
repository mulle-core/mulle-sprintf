/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_return_functions.m is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "mulle_sprintf_return.h"


//
// in the spec, it's declared as a signed return value (but what for ?)
// might be a subtle bug
//
int   mulle_sprintf_return_conversion( struct mulle_buffer *buffer,
                                  struct mulle_sprintf_formatconversioninfo *info,
                                  struct mulle_sprintf_argumentarray *arguments,
                                  int argc)
{
   union mulle_sprintf_argumentvalue  v;
   mulle_sprintf_argumenttype_t   t;
   size_t          len;
   size_t          before;
   
   before = (size_t) info->mystery;

   v = arguments->values[ argc];
   t = arguments->types[ argc];
   
   //
   // this will give too many bytes if the initial flex buffer
   // was not empty. But that's not really a bug, it's a feature
   // (if we _really_ really need this, pass in "before" from caller
   //
   len = mulle_buffer_get_length( buffer) - before;
   
   if( t == mulle_sprintf_unsigned_int_pointer_argumenttype)
   {
      *v.pI = (unsigned int) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_long_pointer_argumenttype)
   {
      *v.pL = (unsigned long) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_long_long_pointer_argumenttype)
   {
      *v.pLL = (unsigned long long) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_short_argumenttype)
   {
      *v.pS = (unsigned short) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_char_argumenttype)
   {
      *v.pC = (unsigned char) len;
      return( 0);
   }
   if( t == mulle_sprintf_uintmax_t_pointer_argumenttype)
   {
      *v.pImt = (uintmax_t) len;
      return( 0);
   }
   if( t == mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype)
   {
      *v.pDif = (unsigned __PTRDIFF_TYPE__) len;
      return( 0);
   }
   if( t == mulle_sprintf_size_t_pointer_argumenttype)
   {
      *v.pSt = (size_t) len;
      return( 0);
   }
   if( t == mulle_sprintf_uint64_t_pointer_argumenttype)
   {
      *v.pQt = (uint64_t) len;
      return( 0);
   }
   return( -1);
}                   


mulle_sprintf_argumenttype_t  mulle_sprintf_get_return_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   switch( info->modifier[ 0])
   {
   case 'h' :
      if( info->modifier[ 1] == 'h')
         return( mulle_sprintf_unsigned_char_pointer_argumenttype);
      return( mulle_sprintf_unsigned_short_pointer_argumenttype);
      
   case 'l' :
      if( info->modifier[ 1] == 'l')
         return( mulle_sprintf_unsigned_long_long_pointer_argumenttype);
      return( mulle_sprintf_unsigned_long_pointer_argumenttype);
      
   case 'j' :
      return( mulle_sprintf_size_t_pointer_argumenttype);
      
   case 't' :
      return( mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype);
      
   case 'z' :
      return( mulle_sprintf_uintmax_t_pointer_argumenttype);
      
   case 'q' :
      return( mulle_sprintf_uint64_t_pointer_argumenttype);
   }
   return( mulle_sprintf_unsigned_int_pointer_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_return_functions = 
{
   mulle_sprintf_get_return_argumenttype,
   mulle_sprintf_return_conversion
};



void  _mulle_sprintf_register_return_functions( struct mulle_sprintf_conversion *tables)
{
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_return_functions, 'n');
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_return_functions()
{
  _mulle_sprintf_register_return_functions( &mulle_sprintf_defaultconversion);
}
   
