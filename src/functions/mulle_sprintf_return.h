/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf_return.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef NS_FORMATTED_RETURN_STRING_CONVERSION__H__
#define NS_FORMATTED_RETURN_STRING_CONVERSION__H__
 
#include "mulle_sprintf_function.h"

 
int   mulle_sprintf_return_conversion( struct mulle_buffer *buffer,
                                       struct mulle_sprintf_formatconversioninfo *info,
                                       struct mulle_sprintf_argumentarray *arguments,
                                       int argc);
                                    
mulle_sprintf_argumenttype_t  mulle_sprintf_get_return_argumenttype( struct mulle_sprintf_formatconversioninfo *info);

extern struct mulle_sprintf_function     _NSReturnString_functions;

void  mulle_sprintf_register_return_functions( struct mulle_sprintf_conversion *tables);

#endif
