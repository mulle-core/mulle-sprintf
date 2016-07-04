/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintf.m is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK.
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#ifndef mulle_sprintf_h__
#define mulle_sprintf_h__

#include "mulle_sprintf_function.h"
#include <mulle_vararg/mulle_vararg.h>


#define MULLE_SPRINTF_VERSION  ((0 << 20) | (6 << 8) | 0)

struct mulle_buffer;
struct mulle_sprintf_conversion;


int   mulle_sprintf( struct mulle_buffer *buffer,
                     char *format,
                     ...);

int   mulle_vsprintf( struct mulle_buffer *buffer,
                      char *format,
                      va_list va);

int   _mulle_vsprintf( struct mulle_buffer *buffer,
                       char *format,
                       va_list va,
                       struct mulle_sprintf_conversion *table);

int   mulle_mvsprintf( struct mulle_buffer *buffer,
                       char *format,
                       mulle_vararg_list va);

int   _mulle_mvsprintf( struct mulle_buffer *buffer,
                        char *format,
                        mulle_vararg_list arguments,
                        struct mulle_sprintf_conversion *table);

#pragma mark -
#pragma manage "static" storage

extern void   *(*mulle_sprintf_get_storage)( struct mulle_allocator *);
extern void   (*mulle_sprintf_free_storage)( void);


#if MULLE_ALLOCATOR_VERSION < ((1 << 20) | (5 << 8) | 0)
# error "mulle_allocator is too old"
#endif
#if MULLE_CONTAINER_VERSION < ((0 << 20) | (5 << 8) | 0)
# error "mulle_container is too old"
#endif

#endif
