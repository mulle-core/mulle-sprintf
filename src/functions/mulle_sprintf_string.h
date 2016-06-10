/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  _NSFormattedString_conversion.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef NS_STRING_SPRINTF_FUNCTIONS__H__
#define NS_STRING_SPRINTF_FUNCTIONS__H__
 
#include <mulle_container/mulle_container.h>
#include "mulle_sprintf_function.h"
 
 
int   mulle_sprintf_string_conversion( struct mulle_buffer *buffer,
                                       struct mulle_sprintf_formatconversioninfo *info,
                                       struct mulle_sprintf_argumentarray *arguments,
                                       int argc);

int   mulle_sprintf_widestring_conversion( struct mulle_buffer *buffer,
                                          struct mulle_sprintf_formatconversioninfo *info,
                                          struct mulle_sprintf_argumentarray *arguments,
                                          int argc);

int   mulle_sprintf_charstring_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           char *s);
int   mulle_sprintf_wcharstring_conversion( struct mulle_buffer *buffer,
                                            struct mulle_sprintf_formatconversioninfo *info,
                                            wchar_t *s);

mulle_sprintf_argumenttype_t  mulle_sprintf_get_widestring_argumenttype( struct mulle_sprintf_formatconversioninfo *info);
mulle_sprintf_argumenttype_t  mulle_sprintf_get_string_argumenttype( struct mulle_sprintf_formatconversioninfo *info);


extern struct mulle_sprintf_function   mulle_sprintf_string_functions;
extern struct mulle_sprintf_function   mulle_sprintf_widestring_functions;

void  _mulle_sprintf_register_string_functions( struct mulle_sprintf_conversion *tables); 

#endif
