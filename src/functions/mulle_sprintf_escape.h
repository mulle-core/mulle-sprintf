/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_escape_functions.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, __MyCompanyName__ 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef _NS_SPRINTF_ESCAPE_FUNCTIONS__H__
#define _NS_SPRINTF_ESCAPE_FUNCTIONS__H__
 
#include "mulle_sprintf_function.h"
 
int  mulle_sprintf_escape_conversion( struct mulle_buffer *buffer,
                                 struct mulle_sprintf_formatconversioninfo *info,
                                 struct mulle_sprintf_argumentarray *arguments,
                                 int argc);
                                          
mulle_sprintf_argumenttype_t  mulle_sprintf_get_escape_argumenttype( struct mulle_sprintf_formatconversioninfo *info);

void  _mulle_sprintf_register_escape_functions( struct mulle_sprintf_conversion *tables);
   
#endif
