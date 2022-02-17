/**********************************************************************************
* UTF-16/32 -> UTF-8 characters conversion
* Copyright (C) 2020-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8_one.c */

#include <stddef.h> /* for size_t */
#ifndef _WIN32
#include <stdint.h> /* for uint16_t/uint32_t */
#endif
#include "libutf16/utf16_to_utf8_one.h"

/* unsigned integer type must be at least of 32 bits */
typedef int check_unsigned_int_at_least_32_bits[1-2*((unsigned)-1 < 0xFFFFFFFF)];

size_t utf16_to_utf8_one(utf8_char_t *const LIBUTF16_RESTRICT s,
	const utf16_char_t w, utf8_state_t *const LIBUTF16_RESTRICT ps)
{
	unsigned b;
	unsigned c = w;
	const unsigned a = *ps;
	if (a) {
		/* a is the first part of utf16 surrogate pair,
		   c must be the second part */
		if (!utf16_is_low_surrogate(c))
			return (size_t)-1;
		*ps = 0;
		c = (a << 10) + c - 0x20DC00 + 0x800000 + 0x10000;
		b = 4;
		s[b - 4] = (utf8_char_t)(c >> 18);
		c = (c & 0x3FFFF) + 0x80000;
		goto b3;
	}
	if (c >= 0x80) {
		if (c >= 0x800) {
			if (utf16_is_high_surrogate(c)) {
				*ps = c;
				return 0;
			}
			if (utf16_is_low_surrogate(c))
				return (size_t)-1;
			b = 3;
			c += 0xE0000;
b3:
			s[b - 3] = (utf8_char_t)(c >> 12);
			c = (c & 0xFFF) + 0x2000;
		}
		else {
			b = 2;
			c += 0x3000;
		}
		s[b - 2] = (utf8_char_t)(c >> 6);
		c = (c & 0x3F) + 0x80;
	}
	else
		b = 1;
	s[b - 1] = (utf8_char_t)c;
	return b;
}

size_t utf32_to_utf8_one(utf8_char_t s[UTF8_MAX_LEN], const utf32_char_t w)
{
	unsigned b;
	unsigned c = w;
	if (c >= 0x80) {
		if (c >= 0x800) {
			if (c > 0xFFFF) {
				if (c > UTF32_MAX_VALUE)
					return (size_t)-1; /* out of range */
				c += 0x3C00000;
				b = 4;
				s[b - 4] = (utf8_char_t)(c >> 18);
				c = (c & 0x3FFFF) + 0x80000;
			}
			else if (utf32_is_surrogate(c))
				return (size_t)-1; /* must not be a surrogate */
			else {
				b = 3;
				c += 0xE0000;
			}
			s[b - 3] = (utf8_char_t)(c >> 12);
			c = (c & 0xFFF) + 0x2000;
		}
		else {
			b = 2;
			c += 0x3000;
		}
		s[b - 2] = (utf8_char_t)(c >> 6);
		c = (c & 0x3F) + 0x80;
	}
	else
		b = 1;
	s[b - 1] = (utf8_char_t)c;
	return b;
}
