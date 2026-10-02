#include <mulle-sprintf/mulle-sprintf.h>

#include <stdio.h>
#include <string.h>


static void   test_roundtrip( char *format, char *description, ...)
{
   va_list               args;
   va_list               args2;
   char                  scan_format[ 256];
   int                   sprintf_result;
   int                   sscanf_result;
   char                  *str;
   void                  *ptr_in;
   void                  *ptr_out;
   int                   int_in;
   int                   int_out;
   long                  long_in;
   long                  long_out;
   long long             llong_in;
   long long             llong_out;
   unsigned int          uint_in;
   unsigned int          uint_out;
   unsigned long         ulong_in;
   unsigned long         ulong_out;
   unsigned long long    ullong_in;
   unsigned long long    ullong_out;
   double                double_in;
   double                double_out;
   char                  str_in[ 256];
   char                  str_out[ 256];

   mulle_buffer_do( buffer)
   {
      va_start( args, description);
      sprintf_result = mulle_buffer_vsprintf( buffer, format, args);
      va_end( args);

      if( sprintf_result < 0)
      {
         fprintf( stderr, "FAIL: %s - sprintf failed\n", description);
         exit( 1);
      }

      str = mulle_buffer_get_string( buffer);
      printf( "Testing: %s\n", description);
      printf( "  Format:  \"%s\"\n", format);
      printf( "  Output:  \"%s\"\n", str);

      strcpy( scan_format, format);

      va_start( args2, description);

      if( strstr( format, "%p"))
      {
         ptr_in       = va_arg( args2, void *);
         sscanf_result = mulle_sscanf( str, scan_format, &ptr_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( ptr_in != ptr_out)
         {
            fprintf( stderr, "FAIL: %s - pointer mismatch (%p vs %p)\n", description, ptr_in, ptr_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%d") || strstr( format, "%i"))
      {
         int_in        = va_arg( args2, int);
         sscanf_result = mulle_sscanf( str, scan_format, &int_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( int_in != int_out)
         {
            fprintf( stderr, "FAIL: %s - int mismatch (%d vs %d)\n", description, int_in, int_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%ld"))
      {
         long_in       = va_arg( args2, long);
         sscanf_result = mulle_sscanf( str, scan_format, &long_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( long_in != long_out)
         {
            fprintf( stderr, "FAIL: %s - long mismatch (%ld vs %ld)\n", description, long_in, long_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%lld"))
      {
         llong_in      = va_arg( args2, long long);
         sscanf_result = mulle_sscanf( str, scan_format, &llong_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( llong_in != llong_out)
         {
            fprintf( stderr, "FAIL: %s - long long mismatch (%lld vs %lld)\n", description, llong_in, llong_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%u"))
      {
         uint_in       = va_arg( args2, unsigned int);
         sscanf_result = mulle_sscanf( str, scan_format, &uint_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( uint_in != uint_out)
         {
            fprintf( stderr, "FAIL: %s - unsigned int mismatch (%u vs %u)\n", description, uint_in, uint_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%lu"))
      {
         ulong_in      = va_arg( args2, unsigned long);
         sscanf_result = mulle_sscanf( str, scan_format, &ulong_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( ulong_in != ulong_out)
         {
            fprintf( stderr, "FAIL: %s - unsigned long mismatch (%lu vs %lu)\n", description, ulong_in, ulong_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%llu"))
      {
         ullong_in     = va_arg( args2, unsigned long long);
         sscanf_result = mulle_sscanf( str, scan_format, &ullong_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( ullong_in != ullong_out)
         {
            fprintf( stderr, "FAIL: %s - unsigned long long mismatch (%llu vs %llu)\n", description, ullong_in, ullong_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%x") || strstr( format, "%X"))
      {
         uint_in       = va_arg( args2, unsigned int);
         sscanf_result = mulle_sscanf( str, scan_format, &uint_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( uint_in != uint_out)
         {
            fprintf( stderr, "FAIL: %s - hex mismatch (%x vs %x)\n", description, uint_in, uint_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%o"))
      {
         uint_in       = va_arg( args2, unsigned int);
         sscanf_result = mulle_sscanf( str, scan_format, &uint_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( uint_in != uint_out)
         {
            fprintf( stderr, "FAIL: %s - octal mismatch (%o vs %o)\n", description, uint_in, uint_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%f") || strstr( format, "%e") || strstr( format, "%g"))
      {
         double_in     = va_arg( args2, double);
         sscanf_result = mulle_sscanf( str, scan_format, &double_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( double_in != double_out)
         {
            fprintf( stderr, "FAIL: %s - double mismatch (%g vs %g)\n", description, double_in, double_out);
            exit( 1);
         }
      }
      else if( strstr( format, "%s"))
      {
         strcpy( str_in, va_arg( args2, char *));
         sscanf_result = mulle_sscanf( str, scan_format, str_out);

         if( sscanf_result != 1)
         {
            fprintf( stderr, "FAIL: %s - sscanf returned %d (expected 1)\n", description, sscanf_result);
            exit( 1);
         }

         if( strcmp( str_in, str_out) != 0)
         {
            fprintf( stderr, "FAIL: %s - string mismatch (\"%s\" vs \"%s\")\n", description, str_in, str_out);
            exit( 1);
         }
      }

      va_end( args2);

      printf( "  PASS\n\n");
   }
}


int   main( void)
{

   test_roundtrip( "%p", "pointer", (void *) 0x1234FEAD);
   test_roundtrip( "%d", "decimal int", 42);
   test_roundtrip( "%d", "negative int", -1848);
   test_roundtrip( "%u", "unsigned int", 4294967295U);
   test_roundtrip( "%x", "hex lowercase", 0xDEADBEEF);
   test_roundtrip( "%X", "hex uppercase", 0xCAFEBABE);
   test_roundtrip( "%o", "octal", 0755);
   test_roundtrip( "%ld", "long", 123456789L);
   test_roundtrip( "%lld", "long long", 9223372036854775807LL);
   test_roundtrip( "%lu", "unsigned long", 18446744073709551615UL);
   test_roundtrip( "%llu", "unsigned long long", 18446744073709551615ULL);
   test_roundtrip( "%f", "float", 3.14159);
   test_roundtrip( "%e", "scientific", 1.23e-4);
   test_roundtrip( "%g", "general", 123.456);
   test_roundtrip( "%s", "string", "hello");

   printf( "All tests passed!\n");

   mulle_sprintf_free_storage();

   return( 0);
}
