#include <mulle-sprintf/mulle-sprintf.h>
#include <stdio.h>
#include <wchar.h>

/*
 * This test reproduces a crash on Windows when using %ls with wchar_t strings.
 * 
 * CRITICAL: What format specifier to use for L"..." strings?
 * ===========================================================
 * 
 * **YES, there IS a guarantee from the C standard:**
 * 
 * C99/C11 standard specifies:
 * - %ls (or %S) takes a wchar_t * argument
 * - %lc (or %C) takes a wint_t argument (single wide character)
 * - These MUST work with L"..." strings and L'x' characters
 * 
 * So: printf("%ls", L"hello") is GUARANTEED to work by the C standard.
 * 
 * The standard does NOT specify:
 * - What encoding wchar_t uses (UTF-16, UTF-32, locale-specific, etc.)
 * - The size of wchar_t (can be 16-bit, 32-bit, or other)
 * 
 * But it DOES guarantee:
 * - L"..." produces a wchar_t array
 * - %ls consumes a wchar_t * argument
 * - The implementation must make these work together
 * 
 * In practice:
 * - Windows: wchar_t = 16-bit UTF-16, %ls expects UTF-16
 * - Linux: wchar_t = 32-bit UTF-32, %ls expects UTF-32
 * - Both work because the format and the literal match
 * 
 * CRITICAL: What encoding does L"VfL" produce?
 * ==========================================
 * 
 * L"VfL" is a WIDE STRING LITERAL in C. The encoding it produces is:
 * 
 * **NOT DEFINED BY THE C STANDARD**
 * 
 * The C standard only specifies:
 * - L"..." creates a wchar_t array
 * - wchar_t is an implementation-defined type
 * - The encoding is implementation-defined
 * 
 * In practice (de facto standards):
 * 
 * 1. **Windows (MSVC, MinGW):**
 *    - wchar_t is 16-bit (typedef unsigned short)
 *    - L"VfL" produces UTF-16LE encoding
 *    - Memory: [0x0056, 0x0066, 0x004C, 0x0000] (4 wchar_t values, 8 bytes)
 *    - This is because Windows API uses UTF-16 everywhere (WCHAR = wchar_t)
 * 
 * 2. **Linux/Unix (GCC, Clang):**
 *    - wchar_t is 32-bit (typedef int or unsigned int)
 *    - L"VfL" produces UTF-32 encoding (also called UCS-4)
 *    - Memory: [0x00000056, 0x00000066, 0x0000004C, 0x00000000] (4 wchar_t values, 16 bytes)
 *    - This is the POSIX convention
 * 
 * 3. **Can this change?**
 *    - YES, but extremely unlikely in practice
 *    - Compiler vendors could theoretically use different encodings
 *    - Some old systems used locale-specific encodings (e.g., Shift-JIS on Japanese Windows)
 *    - Modern systems have standardized on UTF-16 (Windows) or UTF-32 (Unix)
 * 
 * 4. **How to detect at compile time:**
 *    - sizeof(wchar_t) == 2  → probably UTF-16 (Windows)
 *    - sizeof(wchar_t) == 4  → probably UTF-32 (Linux/Unix)
 *    - But this is a heuristic, not guaranteed!
 * 
 * 5. **How to detect at runtime:**
 *    - Check __STDC_ISO_10646__ macro (if defined, wchar_t is Unicode)
 *    - On Windows, assume UTF-16
 *    - On POSIX, assume UTF-32
 * 
 * The mulle-sprintf code does:
 *    if (sizeof(wchar_t) == sizeof(mulle_utf16_t))
 *        treat as UTF-16
 *    else
 *        treat as UTF-32
 * 
 * This is reasonable but assumes:
 * - 16-bit wchar_t = UTF-16 (true on Windows)
 * - 32-bit wchar_t = UTF-32 (true on modern Unix)
 * 
 * The issue:
 * - On Windows, wchar_t is 16-bit UTF-16
 * - mulle-sprintf correctly detects this and routes to UTF-16 conversion
 * - But mulle-utf's UTF-16 to UTF-32 conversion has a bug on Windows
 * - It produces invalid Unicode values > 0x10FFFF
 * - This triggers an assertion in mulle-utf32.c
 * 
 * PORTABLE CODE RECOMMENDATIONS:
 * ===============================
 * 
 * If you want portable Unicode strings:
 * 
 * 1. **Use UTF-8 in char * strings (RECOMMENDED):**
 *    - char *s = "Hello 世界";  // UTF-8 source encoding
 *    - printf("%s", s);
 *    - Works everywhere, no wchar_t issues
 * 
 * 2. **Use mulle-sprintf's explicit format specifiers:**
 *    - %hS for UTF-16 strings (uint16_t *)
 *    - %lS for UTF-32 strings (uint32_t *)
 *    - These are explicit about encoding, not platform-dependent
 * 
 * 3. **Avoid wchar_t for portable code:**
 *    - wchar_t encoding varies by platform
 *    - %ls behavior varies by platform
 *    - Only use if you need platform-specific APIs (Windows WCHAR)
 */

int main(void)
{
   char buf[256];
   
   // First, let's demonstrate what encoding L"VfL" actually uses
   printf("=== Encoding Analysis ===\n");
   printf("sizeof(wchar_t) = %zu bytes\n", sizeof(wchar_t));
   
   wchar_t demo[] = L"VfL";
   printf("L\"VfL\" in memory (hex):\n");
   for (size_t i = 0; i < 4; i++) {
      printf("  demo[%zu] = 0x%08X", i, (unsigned int)demo[i]);
      if (demo[i] == 0)
         printf(" (null terminator)\n");
      else
         printf(" ('%c')\n", (char)demo[i]);
   }
   
   if (sizeof(wchar_t) == 2) {
      printf("Interpretation: UTF-16 encoding (Windows)\n");
   } else if (sizeof(wchar_t) == 4) {
      printf("Interpretation: UTF-32 encoding (Linux/Unix)\n");
   } else {
      printf("Interpretation: Unknown (sizeof(wchar_t) = %zu)\n", sizeof(wchar_t));
   }
   printf("\n");
   
   // Test 1: Simple ASCII wchar_t string using L"" literal
   // This is the proper way to create a wide string literal
   // On Windows: L"VfL" creates a const wchar_t[4] = {0x0056, 0x0066, 0x004C, 0x0000}
   // On Linux: L"VfL" creates a const wchar_t[4] = {0x00000056, 0x00000066, 0x0000004C, 0x00000000}
   printf("Test 1: L\"VfL\" literal\n");
   mulle_sprintf(buf, "|%ls|", L"VfL");
   printf("Result: %s\n", buf);
   
   // Test 2: wchar_t array initialized with wide character literals
   // L'V' is a wchar_t literal (16-bit on Windows, 32-bit on Linux)
   // This is the correct way to initialize a wchar_t array
   printf("\nTest 2: wchar_t array with L'x' literals\n");
   wchar_t test1[] = { L'V', L'f', L'L', 0 };
   mulle_sprintf(buf, "|%ls|", test1);
   printf("Result: %s\n", buf);
   
   // Test 3: wchar_t array initialized with char literals (WRONG but common)
   // 'V' is a char literal (8-bit), gets implicitly converted to wchar_t
   // On Windows: 'V' (0x56) -> wchar_t (0x0056) - should work but is bad practice
   // This might expose issues if the conversion isn't handled properly
   printf("\nTest 3: wchar_t array with 'x' literals (bad practice)\n");
   wchar_t test2[] = { 'V', 'f', 'L', 0 };
   mulle_sprintf(buf, "|%ls|", test2);
   printf("Result: %s\n", buf);
   
   // Test 4: Empty wchar_t string
   printf("\nTest 4: Empty L\"\" literal\n");
   mulle_sprintf(buf, "|%ls|", L"");
   printf("Result: %s\n", buf);
   
   // Test 5: Longer ASCII string
   printf("\nTest 5: Longer ASCII string\n");
   mulle_sprintf(buf, "|%ls|", L"Hello World");
   printf("Result: %s\n", buf);
   
   printf("\nAll tests completed successfully!\n");
   return 0;
}
