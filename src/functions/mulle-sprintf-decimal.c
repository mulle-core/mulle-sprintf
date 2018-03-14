/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_decimal_functions.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "mulle-sprintf-decimal.h"


static int   mulle_sprintf_convert_decimal( struct mulle_buffer *buffer,
                                            struct mulle_sprintf_formatconversioninfo *info,
                                            struct mulle_sprintf_argumentarray *arguments,
                                            int argc)
{
   union mulle_sprintf_argumentvalue   v;
   char                                *tmp;
   char                                *p;
   NSDecimal                           *v_decimal;
   _NSDecimalFormattingInfo            format_info;
   unsigned int                        len;

   assert( buffer);
   assert( info);
   assert( arguments);

   v = arguments->values[ argc];

   v_decimal = v.pDecimal;

   memset( &format_info, 0, sizeof( format_info));

   format_info.negative_string = "-";
   if( info->memory.quote_found)
      format_info.thousands_string = ",";
   if( info->memory.plus_found)
      format_info.positive_string = "+";
   else
      if( info->memory.space_found)
         format_info.positive_string  = " ";

   format_info.n_chars_after_dot = info->precision ? info->precision : USHRT_MAX;

   len = _NSDecimalNumberFormattedPrintBufferSize( v_decimal, &format_info);
   tmp = alloca( len);
   if( ! tmp)
      return( -1);

   p = _NSDecimalFormattedString( tmp, len, v_decimal, &format_info);

   mulle_sprintf_justified( buffer, info, p, len - (p - tmp), 0, 0, 0, 0);
   return( 0);
}


static mulle_sprintf_argumenttype_t  mulle_sprintf_get_decimal_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   return( mulle_sprintf_voidargumenttype);
}


static struct mulle_sprintf_function     mulle_sprintf_decimal_functions =
{
   mulle_sprintf_get_decimal_argumenttype,
   mulle_sprintf_convert_decimal
};



void  mulle_sprintf_register_decimal_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_decimal_functions, 'N');
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_decimal_functions()
{
   mulle_sprintf_register_decimal_functions( &mulle_sprintf_get_config()->defaultconversion);
}


