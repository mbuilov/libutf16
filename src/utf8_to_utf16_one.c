/**********************************************************************************
* UTF-8 -> UTF-16/32 characters conversion
* Copyright (C) 2020-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_to_utf16_one.c */

#include <stddef.h> /* for size_t */
#ifndef _WIN32
#include <stdint.h> /* for uint16_t/uint32_t */
#endif
#include "libutf16/utf8_to_utf16_one.h"

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#endif

/* unsigned integer type must be at least of 32 bits */
typedef int check_unsigned_int_at_least_32_bits[1-2*((unsigned)-1 < 0xFFFFFFFF)];

/* returns:
  (size_t)-1 - if s contains invalid utf8 byte sequence;
  (size_t)-2 - if s is too short to read complete unicode character,
               n bytes of s have been consumed, state has been updated;
  >0         - number of bytes consumed from s, state contains read unicode character. */
static size_t utf8_read_one_internal(
	const utf8_char_t *LIBUTF16_RESTRICT s/*[n],!=NULL if n>0*/,
	size_t n/*>=0*/,
	unsigned *const LIBUTF16_RESTRICT state/*in,out,!=NULL*/,
	unsigned a)
{
	if (!n)
		return (size_t)-2;
	{
		unsigned r;
		const unsigned t = a >> 30;
		const unsigned m = ((unsigned)1 << 31) - 1 + ((unsigned)1 << 31);
		switch (t) {
			case 1:
				if (a >= 0x40000000 + 0xE0) {
					if (a >= 0x40000000 + 0xF0)
						goto c11;
					goto c12;
				}
				goto c13;
			case 2:
				if (a >= 0x80000000 + (0xF0 << 6) + 0x80)
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
				if (a > 0xF4)
					return (size_t)-1; /* unicode code point must be <= 0x10FFFF */
				if (!--n) {
					*state = a + 0x40000000; /* 1 of 4 */
					return (size_t)-2;
				}
				s++;
c11:
				r = *s;
				if (0x80 != (r & 0xC0))
					return (size_t)-1; /* incomplete utf8 character */
				a = ((a << 6) & m) + r;
				if (!(0x3C90 <= a && a <= 0x3D8F))
					return (size_t)-1; /* overlong utf8 character/out of range */
				if (!--n) {
					*state = a + 0x80000000; /* 2 of 4 */
					return (size_t)-2;
				}
				s++;
c21:
				r = *s;
				if (0x80 != (r & 0xC0))
					return (size_t)-1; /* incomplete utf8 character */
				a = ((a << 6) & m) + r;
				if (!--n) {
					*state = a + 0xC0000000; /* 3 of 4 */
					return (size_t)-2;
				}
				s++;
c31:
				r = *s;
				if (0x80 != (r & 0xC0))
					return (size_t)-1; /* incomplete utf8 character */
				a = ((a << 6) & m) + r - 0x682080 - 0x10000;
				*state = a;
				return 4 - t;
			}
			if (!--n) {
				*state = a + 0x40000000; /* 1 of 3 */
				return (size_t)-2;
			}
			s++;
c12:
			r = *s;
			if (0x80 != (r & 0xC0))
				return (size_t)-1; /* incomplete utf8 character */
			a = ((a << 6) & m) + r;
			if (a < 0x38A0 || (0x3BE0 <= a && a <= 0x3BFF))
				return (size_t)-1; /* overlong utf8 character/surrogate */
			if (!--n) {
				*state = a + 0x80000000; /* 2 of 3 */
				return (size_t)-2;
			}
			s++;
c22:
			r = *s;
			if (0x80 != (r & 0xC0))
				return (size_t)-1; /* incomplete utf8 character */
			a = ((a << 6) & m) + r - 0xE2080;
			*state = a;
			return 3 - t;
		}
		if (a >= 0xC2) {
			if (!--n) {
				*state = a + 0x40000000; /* 1 of 2 */
				return (size_t)-2;
			}
			s++;
c13:
			r = *s;
			if (0x80 != (r & 0xC0))
				return (size_t)-1; /* incomplete utf8 character */
			a = ((a << 6) & m) + r - 0x3080;
			*state = a;
			return 2 - t;
		}
		return (size_t)-1; /* not expecting 10xxxxxx or overlong utf8 character: 1100000x */
	}
}

size_t utf8_to_utf16_one(
	utf16_char_t *const LIBUTF16_RESTRICT pw,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *const LIBUTF16_RESTRICT ps)
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
		return 0 == (*pw = (utf16_char_t)a) ? 0 : r;
	}
	*pw = (utf16_char_t)(a >> 10); /* 110110aaaabbbbbb */
	*ps = (a & 0x3FF) + 0xDC00;    /* 110111bbcccccccc */
	return r;
}

size_t utf8_to_utf32_one(
	utf32_char_t *const LIBUTF16_RESTRICT pw,
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *const LIBUTF16_RESTRICT ps)
{
	unsigned a = *ps;
	size_t r = utf8_read_one_internal(s, n, &a, a);
	if (r >= (size_t)-2) {
		*ps = a;
		return r;
	}
	*ps = 0;
	if (a > 0xFFFF) {
		/* 110110aaaabbbbbbbbcccccccc
		  -11011000000000000000000000
		  +         10000000000000000 */
		a = a - 0x3600000 + 0x10000;
	}
	return 0 == (*pw = a) ? 0 : r;
}

size_t utf8_len_one(
	const utf8_char_t *const LIBUTF16_RESTRICT s,
	const size_t n,
	utf8_state_t *const LIBUTF16_RESTRICT ps)
{
	unsigned a = *ps;
	size_t r = utf8_read_one_internal(s, n, &a, a);
	if (r >= (size_t)-2) {
		*ps = a;
		return r;
	}
	*ps = 0;
	return !a ? 0 : r;
}

const utf8_char_t *utf8_to_utf32_one_z(
	utf32_char_t *const LIBUTF16_RESTRICT pw,
	const utf8_char_t *const LIBUTF16_RESTRICT s)
{
	unsigned a = s[0];
	if (a < 0x80) {
		*pw = a;
		return s + 1;
	}
	{
		unsigned r = s[1];
		if (0x80 != (r & 0xC0))
			return NULL; /* incomplete utf8 character */
		a = (a << 6) + r;
		if (a >= (0xE0 << 6) + 0x80) {
			r = s[2];
			if (0x80 != (r & 0xC0))
				return NULL; /* incomplete utf8 character */
			a = (a << 6) + r;
			if (a >= (((0xF0 << 6) + 0x80) << 6) + 0x80) {
				if (a > (((0xF4 << 6) + 0x80) << 6) + 0x80)
					return NULL; /* unicode code point must be <= 0x10FFFF */
				if (!((0x3C90 << 6) + 0x80 <= a && a <= (0x3D8F << 6) + 0xBF))
					return NULL; /* overlong utf8 character/out of range */
				r = s[3];
				if (0x80 != (r & 0xC0))
					return NULL; /* incomplete utf8 character */
				*pw = (a << 6) + r - 0x3C82080;
				return s + 4;
			}
			if (a < (0x38A0 << 6) + 0x80 || ((0x3BE0 << 6) + 0x80 <= a && a <= (0x3BFF << 6) + 0xBF))
				return NULL; /* overlong utf8 character/surrogate */
			*pw = a - 0xE2080;
			return s + 3;
		}
		if (a >= (0xC2 << 6) + 0x80) {
			*pw = a - 0x3080;
			return s + 2;
		}
	}
	return NULL; /* not expecting 10xxxxxx or overlong utf8 character: 1100000x */
}
