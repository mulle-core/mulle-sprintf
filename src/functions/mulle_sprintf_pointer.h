/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_pointer is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef _NS_POINTER_SPRINTF_CONVERSION__H__
#define _NS_POINTER_SPRINTF_CONVERSION__H__
 
#include "mulle_sprintf_function.h"
 
int  mulle_sprintf_pointer_conversion( struct mulle_buffer *buffer,
                                       struct mulle_sprintf_formatconversioninfo *info,
                                       struct mulle_sprintf_argumentarray *arguments,
                                       int argc);
                                          
mulle_sprintf_argumenttype_t  mulle_sprintf_get_pointer_argumenttype( struct mulle_sprintf_formatconversioninfo *info);

void  mulle_sprintf_register_pointer_functions( struct mulle_sprintf_conversion *tables);
   
#endif
