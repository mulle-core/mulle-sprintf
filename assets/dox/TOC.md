# mulle-sprintf Library Documentation for AI

## 1. Introduction & Purpose

mulle-sprintf is an extensible sprintf implementation that supports both stdarg and mulle-vararg style variable arguments. It provides drop-in replacements for standard sprintf functions with the ability to add custom format conversion specifiers. The library supports standard C format specifiers plus non-standard extensions like UTF-32 (%lS), UTF-16 (%hS), and Objective-C specific formats (%@ for objects, %b for BOOL). Floating-point conversions are delegated to the system sprintf for portability.

## 2. Key Concepts & Design Philosophy

- **Extensibility**: Format conversion table allows runtime registration of custom format handlers, enabling libraries like MulleObjCStandardFoundation to add domain-specific format specifiers
- **Dual Argument Support**: Works with both traditional C varargs (va_list) and mulle's optimized vararg list format
- **Buffer-First Design**: Core functions operate on mulle_buffer, providing safe, resizable output handling without requiring fixed-size output buffers
- **Unicode Support**: Native support for UTF-32 and UTF-16 string output via format specifiers, eliminating wchar_t pain
- **BOOL Printing**: Custom %b flag for Objective-C BOOL values (prints "YES"/"NO")
- **Safe Overflow Handling**: Fixed-size wrapper functions (mulle_snprintf) safely handle buffer overflow with proper error reporting

## 3. Core API & Data Structures

### mulle-sprintf.h - Core Functions

#### Buffer-Based Sprintf Functions (Primary API)

- `mulle_buffer_sprintf(buffer, format, ...)` → `int`: Printf-style formatting into mulle_buffer with variadic args
- `mulle_buffer_vsprintf(buffer, format, va)` → `int`: Printf-style formatting with va_list
- `_mulle_buffer_vsprintf(buffer, format, va, table)` → `int`: Printf-style formatting with custom conversion table
- `mulle_buffer_mvsprintf(buffer, format, arguments)` → `int`: Printf-style formatting with mulle_vararg_list
- `_mulle_buffer_mvsprintf(buffer, format, arguments, table)` → `int`: Printf-style formatting with mulle_vararg and custom table

#### Fixed-Buffer Sprintf Functions (Convenience API)

- `mulle_snprintf(buf, size, format, ...)` → `int`: Safe fixed-buffer sprintf (returns -1 on overflow, always null-terminates)
- `mulle_vsnprintf(buf, size, format, va)` → `int`: Safe fixed-buffer vsprintf
- `mulle_mvsnprintf(buf, size, format, arguments)` → `int`: Safe fixed-buffer sprintf with mulle_vararg

### mulle-sprintf-function.h - Format Conversion

#### struct mulle_sprintf_formatconversionflags
- **Purpose**: Bit flags parsed from format string to control output behavior
- **Key Fields**:
  - `zero_found`: Pad with zeros (0 flag)
  - `minus_found`: Left-justify (- flag)
  - `space_found`: Space before positive numbers
  - `hash_found`: Alternate form (# flag)
  - `plus_found`: Always show sign
  - `bool_found`: BOOL formatting (b flag)
  - `width_found`: Width specifier present
  - `precision_found`: Precision specifier present
  - `left_justify`: Computed justification mode

#### struct mulle_sprintf_formatconversioninfo
- **Purpose**: Parsed format conversion specification with all extracted parameters
- **Usage**: Intermediate representation used by conversion handlers

#### struct mulle_sprintf_conversion
- **Purpose**: Format conversion table entry for custom format handlers
- **Fields**: Conversion character, handler function pointer, context
- **Usage**: Registered in conversion table for custom format specifiers

### Format Specifiers Supported

#### Standard C Format Characters
- `d`, `i`: Decimal integers
- `u`: Unsigned decimal
- `o`: Octal
- `x`, `X`: Hexadecimal (lowercase/uppercase)
- `f`, `F`: Floating-point
- `e`, `E`: Scientific notation
- `g`, `G`: General format
- `s`: C strings (char *)
- `c`: Single character
- `p`: Pointer (void *)
- `%`: Literal percent

#### Non-Standard Extensions
- `@`: Object pointer (Objective-C), requires custom conversion table
- `lS`: UTF-32 string output (long string)
- `hS`: UTF-16 string output (short string)
- `b`: BOOL output as "YES"/"NO" (Objective-C flag)

#### Flag Characters
- ` ` (space): Blank before positive number
- `0`: Zero padding
- `#`: Alternate form (0x for hex, 0 for octal)
- `-`: Left justify
- `+`: Always show sign
- `'`: Thousands grouping (currently unused)
- `b`: BOOL formatting

## 4. Performance Characteristics

- **O(n) for output generation**: Linear in the length of formatted output
- **Format parsing**: O(m) where m is the length of format string
- **Buffer growth**: mulle_buffer automatically resizes; amortized O(1) append
- **Conversion table lookup**: O(1) hash or O(m) linear search depending on table implementation
- **Thread-Safety**: All functions operate on local va_list or mulle_vararg_list; no global state modified
- **Memory**: No internal allocations beyond mulle_buffer (if used); fixed-buffer versions allocate internally only on error

## 5. AI Usage Recommendations & Patterns

### Best Practices

- **Use Buffer API for Performance**: mulle_buffer_sprintf is more efficient than repeated mulle_snprintf calls
- **Provide Conversion Table**: If using custom format specifiers, pass conversion table to _mulle_buffer_vsprintf variants
- **Buffer Hints**: Pre-allocate mulle_buffer with expected size to minimize reallocations
- **Error Checking**: Buffer functions return character count; check for negative return values in snprintf variants
- **Vararg Choice**: Use mulle-vararg for new code (more efficient); use va_list for C compatibility

### Common Pitfalls

- **Float Precision**: Floating-point conversions delegate to system sprintf; precision not guaranteed between platforms
- **Custom Conversions**: Must register conversion table before use; passing NULL table disables custom specifiers
- **Buffer Ownership**: mulle_buffer_sprintf does NOT null-terminate; use with mulle_buffer_append for safety
- **snprintf Semantics**: mulle_snprintf returns -1 on overflow (not character count needed like system snprintf)
- **Format String Validation**: No format string validation; invalid specifiers may produce undefined behavior

### Idiomatic Usage

```c
// Pattern 1: Use buffer when building strings programmatically
struct mulle_buffer buffer = mulle_buffer_make_with_size(NULL, 256);
mulle_buffer_sprintf(&buffer, "Value: %d, Text: %s", 42, "hello");
// Use buffer.bytes, buffer.length

// Pattern 2: Use snprintf for simple, fixed-size outputs
char buf[64];
if (mulle_snprintf(buf, sizeof(buf), "x=%d, y=%d", x, y) < 0) {
    // Handle overflow
}

// Pattern 3: Extend with custom conversions
struct mulle_sprintf_conversion custom_table[] = { ... };
_mulle_buffer_vsprintf(&buffer, format, va, custom_table);
```

## 6. Integration Examples

### Example 1: Basic sprintf with Buffer

```c
#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-buffer/mulle-buffer.h>

int main() {
    struct mulle_buffer buffer = mulle_buffer_make_zero();
    
    mulle_buffer_sprintf(&buffer, "The answer is %d\n", 42);
    mulle_buffer_sprintf(&buffer, "Pi is approximately %.2f\n", 3.14159);
    
    // Access result
    printf("Buffer: %.*s", (int)buffer.length, (char *)buffer.bytes);
    
    mulle_buffer_done(&buffer);
    return 0;
}
```

### Example 2: Safe Fixed-Size Formatting

```c
#include <mulle-sprintf/mulle-sprintf.h>
#include <stdio.h>

int main() {
    char buffer[32];
    
    int result = mulle_snprintf(buffer, sizeof(buffer), 
                                "x=%d y=%d", 100, 200);
    
    if (result < 0) {
        fprintf(stderr, "Output truncated (overflow)\n");
    } else {
        printf("Result: %s (length %d)\n", buffer, result);
    }
    
    return 0;
}
```

### Example 3: Custom Format Specifier

```c
#include <mulle-sprintf/mulle-sprintf.h>

// Custom handler for %T (time) format
static int custom_time_handler(struct mulle_buffer *buffer, 
                               const struct mulle_sprintf_formatconversioninfo *info,
                               void *p) {
    time_t t = (time_t)(intptr_t)p;
    const char *time_str = ctime(&t);
    return mulle_buffer_append_bytes(buffer, (void *)time_str, strlen(time_str));
}

int main() {
    struct mulle_sprintf_conversion table[] = {
        { 'T', custom_time_handler, NULL },
        { 0, NULL, NULL }
    };
    
    struct mulle_buffer buffer = mulle_buffer_make_zero();
    time_t now = time(NULL);
    
    _mulle_buffer_sprintf(&buffer, "Current time: %T", table, now);
    
    mulle_buffer_done(&buffer);
    return 0;
}
```

### Example 4: Hexadecimal and Alternate Forms

```c
#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-buffer/mulle-buffer.h>

int main() {
    struct mulle_buffer buffer = mulle_buffer_make_zero();
    
    int value = 255;
    
    // Standard hex
    mulle_buffer_sprintf(&buffer, "Hex: %x\n", value);
    
    // Hex with 0x prefix (alternate form)
    mulle_buffer_sprintf(&buffer, "Hex alt: %#x\n", value);
    
    // Zero-padded hex to width 8
    mulle_buffer_sprintf(&buffer, "Hex padded: %08x\n", value);
    
    printf("%.*s", (int)buffer.length, (char *)buffer.bytes);
    mulle_buffer_done(&buffer);
    return 0;
}
```

### Example 5: UTF-32 and UTF-16 Strings

```c
#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-buffer/mulle-buffer.h>

int main() {
    struct mulle_buffer buffer = mulle_buffer_make_zero();
    
    // UTF-32 string (%lS)
    uint32_t utf32_str[] = { 0x41, 0x42, 0x43, 0 };  // "ABC" in UTF-32
    mulle_buffer_sprintf(&buffer, "UTF-32: %lS\n", utf32_str);
    
    // UTF-16 string (%hS)
    uint16_t utf16_str[] = { 0x41, 0x42, 0x43, 0 };  // "ABC" in UTF-16
    mulle_buffer_sprintf(&buffer, "UTF-16: %hS\n", utf16_str);
    
    printf("%.*s", (int)buffer.length, (char *)buffer.bytes);
    mulle_buffer_done(&buffer);
    return 0;
}
```

### Example 6: Printf-to-Buffer Stream

```c
#include <mulle-sprintf/mulle-sprintf.h>
#include <mulle-buffer/mulle-buffer.h>

void log_message(struct mulle_buffer *log_buffer, const char *format, ...) {
    va_list va;
    va_start(va, format);
    
    mulle_buffer_append_bytes(log_buffer, (void *)"[LOG] ", 6);
    mulle_buffer_vsprintf(log_buffer, format, va);
    mulle_buffer_append_bytes(log_buffer, (void *)"\n", 1);
    
    va_end(va);
}

int main() {
    struct mulle_buffer log = mulle_buffer_make_zero();
    
    log_message(&log, "Initialization complete");
    log_message(&log, "Processing %d items", 42);
    
    printf("%.*s", (int)log.length, (char *)log.bytes);
    mulle_buffer_done(&log);
    return 0;
}
```

## 7. Dependencies

- mulle-c11
- mulle-buffer
- mulle-vararg (for mulle_vararg support)
