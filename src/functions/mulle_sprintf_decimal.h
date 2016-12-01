/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_decimal.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef _MULLE_SPRINTF_DECIMAL_FUNCTIONS__H__
#define _MULLE_SPRINTF_DECIMAL_FUNCTIONS__H__
 
#include "mulle_sprintf_int.h"


int  mulle_sprintf_convert_decimal( struct mulle_buffer *buffer,
                                 struct mulle_sprintf_formatconversioninfo *info,
                                 struct mulle_sprintf_argumentarray *arguments,
                                 int argc);
                                          
mulle_sprintf_argumenttype_t  mulle_sprintf_get_decimal_argumenttype( struct mulle_sprintf_formatconversioninfo *info);

void  _mulle_sprintf_register_decimal_functions( struct mulle_sprintf_conversion *tables);
   
#endif
