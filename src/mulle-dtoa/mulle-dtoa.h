/* Implementation of the Schubfach algorithm:
 * https://drive.google.com/file/d/1IEeATSVnEE6TkrHlCYNY2GjaraBjOT4f.
 * Copyright (c) 2025 - present, Victor Zverovich
 * Copyright (c) 2025 - C conversion by Nat!
 * Distributed under the MIT license (see LICENSE).
 */

#ifndef MULLE__DTOA_H
#define MULLE__DTOA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MULLE__DTOA_BUFFER_SIZE 25

/* Intermediate decimal representation of a double value.
 * Fits in 128 bits (two uint64_t) for efficient return by value.
 */
struct mulle_dtoa_decimal {
  uint64_t significand;  /* decimal significand/mantissa */
  int16_t  exponent;     /* decimal exponent */
  uint8_t  sign;         /* 0 = positive, 1 = negative */
  uint8_t  special;      /* 0 = normal, 1 = inf, 2 = nan, 3 = zero */
  uint32_t _padding;     /* reserved for alignment */
};

/* Decomposes a double into decimal representation.
 * Returns the intermediate form suitable for custom formatting.
 */
struct mulle_dtoa_decimal mulle_dtoa_decompose(double value);

/* Writes the shortest correctly rounded decimal representation of `value` to
 * `buffer`. `buffer` should point to a buffer of size MULLE__DTOA_BUFFER_SIZE or larger.
 */
void mulle_dtoa(double value, char* buffer);

#ifdef __cplusplus
}
#endif

#endif  /* MULLE__DTOA_H */
