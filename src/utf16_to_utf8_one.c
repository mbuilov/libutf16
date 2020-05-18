/**********************************************************************************
* UTF-16 -> UTF-8 characters conversion
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8_one.c */

#include <stddef.h> /* for size_t */
#ifndef _MSC_VER
#include <stdint.h> /* for uint16_t/uint32_t */
#endif
#include "libutf16/utf16_to_utf8_one.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

A_Use_decl_annotations
size_t utf16_to_utf8_one(utf8_char_t s[UTF8_MAX_LEN], utf16_char_t w, utf8_state_t *const ps)
{
	unsigned b = 0;
	unsigned c = w;
	unsigned a = *ps;
	if (a) {
		/* a is the first part of utf16 surrogate pair,
		   c must be the second part */
		if (0xDC00 != (c & 0xFC00))
			return (size_t)-1;
		*ps = 0;
		c = (a << 10) + c - 0x20DC00 + 0x800000 + 0x10000;
		b += 4;
		s[b - 4] = (utf8_char_t)(c >> 18);
		c = (c & 0x3FFFF) + 0x80000;
		goto b3;
	}
	if (c >= 0x80) {
		if (c >= 0x800) {
			if (0xD800 == (c & 0xFC00)) {
				*ps = c;
				return 0;
			}
			b += 3;
			c += 0xE0000;
b3:
			s[b - 3] = (utf8_char_t)(c >> 12);
			c = (c & 0xFFF) + 0x2000;
		}
		else {
			b += 2;
			c += 0x3000;
		}
		s[b - 2] = (utf8_char_t)(c >> 6);
		c = (c & 0x3F) + 0x80;
	}
	else
		b++;
	s[b - 1] = (utf8_char_t)c;
	return b;
}

A_Use_decl_annotations
size_t utf32_to_utf8_one(utf8_char_t s[UTF8_MAX_LEN], utf32_char_t w)
{
	unsigned b = 0;
	unsigned c = w;
	if (c >= 0x80) {
		if (c >= 0x800) {
			if (c >= 0x10000) {
				if (c >= 0x110000)
					return (size_t)-1;
				c += 0x3C00000;
				b += 4;
				s[b - 4] = (utf8_char_t)(c >> 18);
				c = (c & 0x3FFFF) + 0x380000;
			}
			else {
				b += 3;
				c += 0xE0000;
			}
			s[b - 3] = (utf8_char_t)(c >> 12);
			c = (c & 0xFFF) + 0x2000;
		}
		else {
			b += 2;
			c += 0x3000;
		}
		s[b - 2] = (utf8_char_t)(c >> 6);
		c = (c & 0x3F) + 0x80;
	}
	else
		b++;
	s[b - 1] = (utf8_char_t)c;
	return b;
}
