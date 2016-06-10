/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  _NSCharacter_conversion.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef _NS_CHARACTER_SPRINTF_FUNCTIONS__H__
#define _NS_CHARACTER_SPRINTF_FUNCTIONS__H__


#include "mulle_sprintf_function.h"
 

int   mulle_sprintf_wide_character_conversion( struct mulle_buffer *buffer,
                                              struct mulle_sprintf_formatconversioninfo *info,
                                              struct mulle_sprintf_argumentarray *arguments,
                                              int argc);

int   mulle_sprintf_character_conversion( struct mulle_buffer *buffer,
                                     struct mulle_sprintf_formatconversioninfo *info,
                                     struct mulle_sprintf_argumentarray *arguments,
                                     int argc);

mulle_sprintf_argumenttype_t  mulle_sprintf_get_widecharacter_argumenttype( struct mulle_sprintf_formatconversioninfo *info);
mulle_sprintf_argumenttype_t  mulle_sprintf_get_character_argumenttype( struct mulle_sprintf_formatconversioninfo *info);

void  _mulle_sprintf_register_character_functions( struct mulle_sprintf_conversion *tables);

#endif
