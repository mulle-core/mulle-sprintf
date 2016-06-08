/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintfInteger_conversion.h is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef NS_INTEGER_SPRINTF_CONVERSION__H__
#define NS_INTEGER_SPRINTF_CONVERSION__H__

#include "mulle_sprintf_function.h"


void   mulle_sprintf_justified( struct mulle_buffer *buffer,
                                struct mulle_sprintf_formatconversioninfo *info,
                                char *p,
                                int p_length,
                                char *q,
                                int q_length,
                                int  precision,
                                char prefix);
                            
void   mulle_sprintf_justified_and_prefixed( struct mulle_buffer *buffer,
                                             struct mulle_sprintf_formatconversioninfo *info,
                                             char *p,
                                             int p_length,
                                             char prefix,
                                             int is_zero,
                                             int (*set_prefix)( char *, int, int, int));

int   mulle_sprintf_int_decimal_conversion( struct mulle_buffer *buffer,
                                                struct mulle_sprintf_formatconversioninfo *info,
                                                struct mulle_sprintf_argumentarray *arguments,
                                                int argc);


int   mulle_sprintf_unsigned_int_decimal_conversion( struct mulle_buffer *buffer,
                                                         struct mulle_sprintf_formatconversioninfo *info,
                                                         struct mulle_sprintf_argumentarray *arguments,
                                                         int argc);


int   mulle_sprintf_int_octal_conversion( struct mulle_buffer *buffer,
                                              struct mulle_sprintf_formatconversioninfo *info,
                                              struct mulle_sprintf_argumentarray *arguments,
                                              int argc);


int   mulle_sprintf_int_hex_conversion( struct mulle_buffer *buffer,
                                            struct mulle_sprintf_formatconversioninfo *info,
                                            struct mulle_sprintf_argumentarray *arguments,
                                            int argc);


int   mulle_sprintf_long_hex_conversion( struct mulle_buffer *buffer,
                                         struct mulle_sprintf_formatconversioninfo *info,
                                         struct mulle_sprintf_argumentarray *arguments,
                                         int argc);

int   mulle_sprintf_unsigned_long_decimal_conversion( struct mulle_buffer *buffer,
                                                      struct mulle_sprintf_formatconversioninfo *info,
                                                      struct mulle_sprintf_argumentarray *arguments,
                                                      int argc);
                                               
int   mulle_sprintf_long_decimal_conversion( struct mulle_buffer *buffer,
                                             struct mulle_sprintf_formatconversioninfo *info,
                                             struct mulle_sprintf_argumentarray *arguments,
                                             int argc);


int   mulle_sprintf_long_octal_conversion( struct mulle_buffer *buffer,
                                           struct mulle_sprintf_formatconversioninfo *info,
                                           struct mulle_sprintf_argumentarray *arguments,
                                           int argc);


mulle_sprintf_argumenttype_t  mulle_sprintf_get_signed_int_argumenttype( struct mulle_sprintf_formatconversioninfo *info);
mulle_sprintf_argumenttype_t  mulle_sprintf_get_unsigned_int_argumenttype( struct mulle_sprintf_formatconversioninfo *info);

extern struct mulle_sprintf_function     mulle_sprintf_int_decimal_functions;
extern struct mulle_sprintf_function     mulle_sprintf_int_hex_functions;
extern struct mulle_sprintf_function     mulle_sprintf_int_octal_functions;
extern struct mulle_sprintf_function     mulle_sprintf_long_decimal_functions;
extern struct mulle_sprintf_function     mulle_sprintf_long_hex_functions;
extern struct mulle_sprintf_function     mulle_sprintf_long_octal_functions;
extern struct mulle_sprintf_function     mulle_sprintf_unsigned_int_decimal_functions;
extern struct mulle_sprintf_function     mulle_sprintf_unsigned_long_decimal_functions;

void  _mulle_sprintf_register_integer_functions( struct mulle_sprintf_conversion *tables);

#endif
