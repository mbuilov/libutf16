/**********************************************************************************
* UTF-8 -> UTF-16 characters conversion
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_to_utf16_one.c */

#include <stddef.h> /* for size_t */
#ifndef _MSC_VER
#include <stdint.h> /* for uint16_t/uint32_t */
#endif
#include "libutf16/utf8_to_utf16_one.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#endif

/* returns:
  (size_t)-1 - if s contains invalid utf8 byte sequence;
  (size_t)-2 - if s is too short to read complete unicode character,
               n bytes of s have been consumed, state has been updated;
  >0         - number of bytes consumed from s, state contains read unicode character. */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_At(s, A_In_reads(n))
A_At(state, A_Inout)
A_Success(return != (size_t)-1)
#endif
static size_t utf8_read_one_internal(const utf8_char_t *s, size_t n, unsigned *const state, unsigned a)
{
	if (!n)
		return (size_t)-2;
	{
		unsigned r;
		const unsigned t = a >> 30;
		a &= ~0xC0000000;
		switch (t) {
			case 1:
				if (a >= 0xE0) {
					if (a >= 0xF0)
						goto c11;
					goto c12;
				}
				goto c13;
			case 2:
				if (a >= (0xF0 << 6))
					goto c21;
				goto c22;
			case 3:
				goto c31;
			default:
				break;
		}
		a = *s;
		if (a < 0x80) {
			*state = a;
			return 1;
		}
		if (a >= 0xE0) {
			if (a >= 0xF0) {
				if (a >= 0xF8)
					return (size_t)-1; /* expecting max 4 utf8 bytes for a unicode code point */
				if (!--n) {
					*state = a | 0x40000000;
					return (size_t)-2;
				}
				s++;
c11:
				r = *s;
				if (0x80 != (r & 0xC0))
					return (size_t)-1; /* incomplete utf8 character */
				a = (a << 6) ^ r;
				if (!--n) {
					*state = a | 0x80000000;
					return (size_t)-2;
				}
				s++;
c21:
				r = *s;
				if (0x80 != (r & 0xC0))
					return (size_t)-1; /* incomplete utf8 character */
				a = (a << 6) ^ r;
				if (!--n) {
					*state = a | 0xC0000000;
					return (size_t)-2;
				}
				s++;
c31:
				r = *s;
				if (0x80 != (r & 0xC0))
					return (size_t)-1; /* incomplete utf8 character */
				a = ((a << 6) ^ r ^ 0xA82080) - 0x10000;
				/* a        = 11011aaaaabbbbbbbbcccccccc before substracting 0x10000,
				 a must match 110110xxxxxxxxxxxxxxxxxxxx after  substracting 0x10000 */
				if (0x3600000 != (a & 0x3F00000))
					return (size_t)-1; /* bad utf8 character */
				*state = a;
				return 4 - t;
			}
			if (!--n) {
				*state = a | 0x40000000;
				return (size_t)-2;
			}
			s++;
c12:
			r = *s;
			if (0x80 != (r & 0xC0))
				return (size_t)-1; /* incomplete utf8 character */
			a = (a << 6) ^ r;
			if (!--n) {
				*state = a | 0x80000000;
				return (size_t)-2;
			}
			s++;
c22:
			r = *s;
			if (0x80 != (r & 0xC0))
				return (size_t)-1; /* incomplete utf8 character */
			a = (a << 6) ^ r ^ 0xE2080;
			/* must not begin or end a surrogate pair */
			if (!a ||
				0xD800 == (a & 0xFC00) ||
				0xDC00 == (a & 0xFC00))
			{
				return (size_t)-1; /* overlong/bad utf8 character */
			}
			*state = a;
			return 3 - t;
		}
		if (a >= 0xC0) {
			if (!--n) {
				*state = a | 0x40000000;
				return (size_t)-2;
			}
			s++;
c13:
			r = *s;
			if (0x80 != (r & 0xC0))
				return (size_t)-1; /* incomplete utf8 character */
			a = (a << 6) ^ r ^ 0x3080;
			if (!a)
				return (size_t)-1; /* overlong utf8 character */
			*state = a;
			return 2 - t;
		}
		return (size_t)-1; /* not expecting 10xxxxxx */
	}
}

A_Use_decl_annotations
size_t utf8_to_utf16_one(utf16_char_t *const pw, const utf8_char_t s[], size_t n, utf8_state_t *const ps)
{
	size_t r;
	unsigned a = *ps;
	if (a && a < 0x40000000) {
		*pw = (utf16_char_t)a;
		*ps = 0;
		return (size_t)-3;
	}
	r = utf8_read_one_internal(s, n, &a, a);
	if (r >= (size_t)-2) {
		*ps = a;
		return r;
	}
	if (a <= 0xFFFF) {
		*ps = 0;
		return (*pw = (utf16_char_t)a) == 0 ? 0 : r;
	}
	/* a == 110110xxxxxxxxxxxxxxxxxxxx */
	*pw = (utf16_char_t)(a >> 10); /* 110110aaaabbbbbb */
	*ps = (a & 0x3FF) + 0xDC00;    /* 110111bbcccccccc */
	return r;
}

A_Use_decl_annotations
size_t utf8_to_utf32_one(utf32_char_t *const pw, const utf8_char_t s[], size_t n, utf8_state_t *const ps)
{
	unsigned a = *ps;
	size_t r = utf8_read_one_internal(s, n, &a, a);
	if (r >= (size_t)-2) {
		*ps = a;
		return r;
	}
	*ps = 0;
	return (*pw = a) == 0 ? 0 : r;
}

A_Use_decl_annotations
size_t utf8_len_one(const utf8_char_t s[], size_t n, utf8_state_t *const ps)
{
	unsigned a = *ps;
	size_t r = utf8_read_one_internal(s, n, &a, a);
	if (r >= (size_t)-2) {
		*ps = a;
		return r;
	}
	*ps = 0;
	return a ? r : 0;
}
