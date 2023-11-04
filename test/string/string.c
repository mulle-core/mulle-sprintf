#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-testallocator/mulle-testallocator.h>

#include <stdio.h>



static void   printf_string_test()
{
   printf( "p: |%s|\n", "VfL Bochum 1848");
   printf( "p: |%.3s|\n", "VfL Bochum 1848");
   printf( "p: |\"%.100s\"|\n", "VfL Bochum 1848");
   printf( "p: |\"%100s\"|\n", "VfL Bochum 1848");
   printf( "p: |\"%100.100s\"|\n", "VfL Bochum 1848");
   printf( "p: |\"%.*s\"|\n", 100, "VfL Bochum 1848");
   printf( "p: |\"%*s\"|\n", 100, "VfL Bochum 1848");
   printf( "p: |\"%*.*s\"|\n", 100, 100, "VfL Bochum 1848");
   printf( "p: |%s|\n", "");
}


static void   simple_string_test()
{
   struct mulle_buffer   *buffer;

   buffer = mulle_buffer_create( NULL);

   mulle_buffer_sprintf( buffer, "m: |%s|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |%.3s|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%.100s\"|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%100s\"|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%100.100s\"|\n", "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%.*s\"|\n", 100, "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%*s\"|\n", 100, "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |\"%*.*s\"|\n", 100, 100, "VfL Bochum 1848");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_sprintf( buffer, "m: |%s|\n", "");
   mulle_buffer_add_byte( buffer, 0);
   printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   mulle_buffer_reset( buffer);

   mulle_buffer_destroy( buffer);
}


static void   simple_null_test()
{
   mulle_buffer_do( buffer)
   {
      mulle_buffer_sprintf( buffer, "|%s|\n", NULL);
      mulle_buffer_make_string( buffer);
      printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   }
}

static void   simple_long_test()
{
   mulle_buffer_do( buffer)
   {
      mulle_buffer_sprintf( buffer, "m: |%s|\n",
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" "
"AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE "
"IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE "
"ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE "
"LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR "
"CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF "
"SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS "
"INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN "
"CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) "
"ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE "
"POSSIBILITY OF SUCH DAMAGE. \xf0\x9f\x91\xa3\x23\xe2\x82\xac");
      mulle_buffer_make_string( buffer);
      printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   }
}

static void   simple_wchar_test()
{
   wchar_t   test_string[] = { 'V', 'f', 'L', 0 };

   mulle_buffer_do( buffer)
   {
      mulle_buffer_sprintf( buffer, "|%ls|\n", test_string);
      mulle_buffer_make_string( buffer);
      printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   }
}

static void   simple_width_test()
{

   mulle_buffer_do( buffer)
   {
      mulle_buffer_sprintf( buffer, "|%-08s|\n", "xyz");
      mulle_buffer_make_string( buffer);
      printf( "%s", (char *) mulle_buffer_get_bytes( buffer));
   }
}


int  main()
{
   simple_string_test();
   printf_string_test();
   simple_null_test();
   simple_wchar_test();
   simple_width_test();
   simple_long_test();

   return( 0);
}

