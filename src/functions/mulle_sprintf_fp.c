/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_fp_functions.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "mulle_sprintf_fp.h"

#include "mulle_sprintf.h"
#include <mulle_buffer/mulle_buffer.h>
#include <float.h>
#include <stdio.h>


//
// first idea was to use dtoa or gcvt, but those use global variables
// in the end, it's just easier to use sprintf and produce a format
// string on the fly.
//
static void  produce_format_string( char format[ 64],
                                    struct mulle_sprintf_formatconversioninfo *info,
                                    int is_long)
{
   char   prefix[ 2];
   
   prefix[ 0] = is_long ? 'L' : 0;
   prefix[ 1] = 0;
   
   if( info->memory.width_found)
   {
      if( info->memory.precision_found)
         snprintf( format, 64, "%%%ld.%d%s%c", (long) info->width, info->precision, prefix, info->conversion);
      else
         snprintf( format, 64, "%%%ld%s%c", (long) info->width, prefix, info->conversion);

      return;
   }

   if( info->memory.precision_found)
      snprintf( format, 64, "%%.%d%s%c", info->precision, prefix, info->conversion);
   else
      snprintf( format, 64, "%%%s%c", prefix, info->conversion);
}


int   mulle_sprintf_fp_conversion( struct mulle_buffer *buffer,
                                   struct mulle_sprintf_formatconversioninfo *info,
                                   struct mulle_sprintf_argumentarray *arguments,
                                   int argc)
{
   union mulle_sprintf_argumentvalue   v;
   mulle_sprintf_argumenttype_t        t;
   auto char                           format[ 64];
   auto char                           result[ 128];
   
   v = arguments->values[ argc];
   t = arguments->types[ argc];

   if( t == mulle_sprintf_double_argumenttype)
   {
      produce_format_string( format, info, 0);
      snprintf( result, sizeof( result), format, v.d);
   }
   else
   {
      produce_format_string( format, info, 1);
      snprintf( result, sizeof( result), format, v.ld);
   }
   mulle_buffer_add_string_with_maxlength( buffer, result, sizeof( result));
   return( 0);
}



mulle_sprintf_argumenttype_t  mulle_sprintf_get_fp_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
   if( info->modifier[ 0] == 'L')
      return( mulle_sprintf_long_double_argumenttype);
   return( mulle_sprintf_double_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_fp_functions = 
{
   mulle_sprintf_get_fp_argumenttype,
   mulle_sprintf_fp_conversion
};



void  mulle_sprintf_register_fp_functions( struct mulle_sprintf_conversion *tables)
{
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'a');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'e');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'f');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'g');

   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'A');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'E');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'F');
   mulle_sprintf_register_functions( tables, &mulle_sprintf_fp_functions, 'G');

   mulle_sprintf_register_modifier( tables, 'L');
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_fp_functions()
{
  mulle_sprintf_register_fp_functions( &mulle_sprintf_get_config()->defaultconversion);
}
 
