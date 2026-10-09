#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-utf/mulle-utf.h>

#include <stdio.h>
#include <string.h>


//
// There is an interesting dilemma:
// With %.3s do we want to see this
// p: |👣#|
// or this ?
// p: |👣#€|
// The emoji is twice as wide...
//
static char   emojimix[] =
{
   /* UTF16 feet, hash, euro */
   0xf0, 0x9f, 0x91, 0xa3, 0x23, 0xe2, 0x82, 0xac,
   'V', 'f', 'L', ' ', 'B', 'o', 'c', 'h', 'u', 'm', ' ',
   0xf0, 0x9f, 0x8e, 0xb2,
   /* dice */
   0
};

static char   plain[] =
{
   'V', 'f', 'L', ' ', 'B', 'o', 'c', 'h', 'u', 'm', ' ', '1', '8', '4', '8', 0
};


static char   empty[] =
{
   0
};


static char   *longish =
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
"POSSIBILITY OF SUCH DAMAGE. \xf0\x9f\x91\xa3\x23\xe2\x82\xac";





static int   test( char *test_string)
{
   auto char   buf[ 1024];
   auto char   full[ 1024];
   auto char   expected[ 1024];
   int         i;
   int         len;
   size_t      n;

   mulle_sprintf( full, "%hhS", test_string);
   len = mulle_utf8_utf32length( test_string, -1);

   // "%.*hhS" must yield a truncation of the full string
   for( i = 0; i <= len; i++)
   {
      mulle_sprintf( buf, "%.*hhS", i, test_string);

      n = strlen( buf);
      if( n > strlen( full) || strncmp( buf, full, n))
      {
         fprintf( stderr, "\"%%.%dhhS\" is not a prefix of \"%%hhS\"\n", i);
         return( 1);
      }
   }

   // the '*' variations must agree with the literal ones
   mulle_sprintf( buf, "%.50hhS", test_string);
   mulle_sprintf( expected, "%.*hhS", 50, test_string);
   if( strcmp( buf, expected))
   {
      fprintf( stderr, "\"%%.50hhS\" differs from \"%%.*hhS\" with 50\n");
      return( 1);
   }

   mulle_sprintf( buf, "%50hhS", test_string);
   mulle_sprintf( expected, "%*hhS", 50, test_string);
   if( strcmp( buf, expected))
   {
      fprintf( stderr, "\"%%50hhS\" differs from \"%%*hhS\" with 50\n");
      return( 1);
   }

   mulle_sprintf( buf, "%50.50hhS", test_string);
   mulle_sprintf( expected, "%*.*hhS", 50, 50, test_string);
   if( strcmp( buf, expected))
   {
      fprintf( stderr, "\"%%50.50hhS\" differs from \"%%*.*hhS\" with 50,50\n");
      return( 1);
   }

   return( 0);
}



int  main()
{
   int   rval;

   rval = 0;
   rval |= test( empty);
   rval |= test( plain);
   rval |= test( emojimix);
   rval |= test( longish);
   rval |= test( NULL);

   mulle_sprintf_free_storage();

   return( rval ? 1 : 0);
}

