#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void   simple_string_test()
{
   char                          *s;
   int                           rval;
   mulle_vararg_builderbuffer_t  storage[ mulle_vararg_builderbuffer_n( 0
                                          + mulle_vararg_sizeof_integer( char)
                                          + mulle_vararg_sizeof_pointer( char *)
                                          + mulle_vararg_sizeof_integer( int)
                                          + mulle_vararg_sizeof_integer( short)
                                          + mulle_vararg_sizeof_integer( long)

                                          + mulle_vararg_sizeof_integer( long long)
                                          + mulle_vararg_sizeof_fp( double)
                                          + mulle_vararg_sizeof_integer( size_t)
                                          + mulle_vararg_sizeof_integer( intmax_t)
                                          + mulle_vararg_sizeof_integer( ptrdiff_t)
                                          )];
   mulle_vararg_list             varargs = mulle_vararg_list_make( storage);
   mulle_vararg_list             p = varargs;

   mulle_vararg_push_char( p, 'V');
   mulle_vararg_push_pointer( p, "Bochum");
   mulle_vararg_push_int( p, 1848);
   mulle_vararg_push_short( p, 1);
   mulle_vararg_push_long( p, 2);

   mulle_vararg_push_longlong( p, 3LL);
   mulle_vararg_push_double( p, 4.0);
   mulle_vararg_push_integer( p, size_t, 5);
   mulle_vararg_push_integer( p, intmax_t, 6);
   mulle_vararg_push_integer( p, ptrdiff_t, 7);
   rval = mulle_mvasprintf( &s, "%cfL %s %d |%hhd|%ld|%lld|%g|%zd|%jd|%td|", varargs);
   printf( "%u: %s\n", rval, s);
   mulle_free( s);
}

static mulle_sprintf_argumenttype_t  types[] =
{
   0, // unused
   mulle_sprintf_int_argumenttype,
   mulle_sprintf_char_argumenttype,
   mulle_sprintf_char_pointer_argumenttype,
   mulle_sprintf_double_argumenttype,
   mulle_sprintf_intmax_t_argumenttype,
   mulle_sprintf_long_argumenttype,
   mulle_sprintf_long_double_argumenttype,
   mulle_sprintf_long_long_argumenttype,
   mulle_sprintf_void_argumenttype,
   mulle_sprintf_ptrdiff_t_argumenttype,
   mulle_sprintf_object_argumenttype,
   mulle_sprintf_int64_t_argumenttype,
   mulle_sprintf_short_argumenttype,
   mulle_sprintf_signed_size_t_argumenttype,
   mulle_sprintf_size_t_argumenttype,
   mulle_sprintf_size_t_pointer_argumenttype,
   mulle_sprintf_uint64_t_argumenttype,
   mulle_sprintf_uint64_t_pointer_argumenttype,
   mulle_sprintf_uintmax_t_argumenttype,
   mulle_sprintf_uintmax_t_pointer_argumenttype,
   mulle_sprintf_unsigned_char_argumenttype,
   mulle_sprintf_unsigned_char_pointer_argumenttype,
   mulle_sprintf_unsigned_int_argumenttype,
   mulle_sprintf_unsigned_int_pointer_argumenttype,
   mulle_sprintf_unsigned_long_argumenttype,
   mulle_sprintf_unsigned_long_long_argumenttype,
   mulle_sprintf_unsigned_long_pointer_argumenttype,
   mulle_sprintf_unsigned_ptrdiff_t_argumenttype,
   mulle_sprintf_unsigned_short_argumenttype,
   mulle_sprintf_unsigned_short_pointer_argumenttype,
   mulle_sprintf_void_pointer_argumenttype,
   mulle_sprintf_wchar_pointer_argumenttype,
   mulle_sprintf_wint_t_argumenttype,
   mulle_sprintf_uint16_t_pointer_argumenttype,
   mulle_sprintf_uint32_t_pointer_argumenttype,
   mulle_sprintf_NSDecimal_pointer_argumenttype,
   mulle_sprintf_unsigned_long_long_pointer_argumenttype,
   mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype
};


void  coverage()
{
   union mulle_sprintf_argumentvalue   values[ 2];
   mulle_sprintf_argumenttype_t        *p_types;
   mulle_sprintf_argumenttype_t        *p_sentinel;
   mulle_vararg_builderbuffer_t        storage[ mulle_vararg_builderbuffer_n( 0
                                          + mulle_vararg_sizeof_fp( double) * 5
                                          )];
   mulle_vararg_list                   varargs = mulle_vararg_list_make( storage);

   // mulle_mvsprintf_set_values indexes by 1
   p_types    = types;
   p_sentinel = &p_types[ sizeof( types) / sizeof( *types) - 1];

   for( ; p_types < p_sentinel; p_types++)
   {
      memset( values, 0x00, sizeof( values));
      memset( storage, 0xFF, sizeof( storage));

      // values and types are indexes by 1
      mulle_mvsprintf_set_values( &values[ 0],
                                  p_types,
                                  2,
                                  varargs);
      assert( values[ 0].ll == 0);
      if( p_types[ 1] == mulle_sprintf_void_argumenttype)
      {
         assert( values[ 1].ll == 0);
         continue;
      }
      assert( values[ 1].ll != 0);
   }
}


int  main()
{
   simple_string_test();
   coverage();

   return( 0);
}

