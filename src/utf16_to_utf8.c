/**********************************************************************************
* UTF-16 -> UTF-8 characters conversion
* Copyright (C) 2018-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8.c */

#include <stddef.h> /* for size_t */

#ifndef _MSC_VER
#include <stdint.h> /* for uint16_t */
#endif

#ifdef _MSC_VER
#include <stdlib.h> /* for _byteswap_ushort()/_byteswap_ulong() */
#endif

#include <memory.h> /* for memcpy() */

#include "libutf16/utf16_to_utf8.h"
#include "libutf16/utf16_swap.h"

#include "utf16_internal.h"

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#endif

#ifdef UTF_GET_UNALIGNED
#define UTF16_CHAR_T utf16_char_unaligned_t
#else
#define UTF16_CHAR_T utf16_char_t
#endif

#define UTF_FORM_NAME2(fu, fx, suffix)  utf16##fu##fx##_to_utf8##suffix
#define UTF_FORM_NAME1(fu, fx, suffix)  UTF_FORM_NAME2(fu, fx, suffix)
#define UTF_FORM_NAME(suffix)           UTF_FORM_NAME1(UTF_GET_U, UTF16_X, suffix)

/*
 utf16_to_utf8_z_
 utf16x_to_utf8_z_
 utf16u_to_utf8_z_
 utf16ux_to_utf8_z_
*/
size_t UTF_FORM_NAME(_z_)(
	const UTF16_CHAR_T **const LIBUTF16_RESTRICT w,
	utf8_char_t **const LIBUTF16_RESTRICT b,
	size_t sz, const int determ_req_size)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF16_CHAR_T *LIBUTF16_RESTRICT s = *w;
	if (sz) {
		utf8_char_t *LIBUTF16_RESTRICT d = *b;
		const utf8_char_t *const e = d + sz;
		do {
			unsigned c = UTF16_GET(s++);
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						const unsigned r = UTF16_GET(s);
						if (0xDC00 != (r & 0xFC00)) {
							*w = s - 1; /* (**w) != 0 */
							*b = d;
							return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
						}
						s++;
						if ((size_t)(e - d) < 4) {
							m = 3;
							break; /* too small output buffer */
						}
						/* 110110xxyyyyyyyy0000000000
						  +          110111aabbbbbbbb
						  -    1000001101110000000000
						  +  100000000000000000000000
						  ===========================
						   111100xxyyyyyyyyaabbbbbbbb
						  +         10000000000000000 */
						c = (c << 10) + r - 0x20DC00 + 0x800000 + 0x10000;
						d += 4;
						d[-4] = (utf8_char_t)(c >> 18);
						c = (c & 0x3FFFF) + 0x80000;
					}
					else if (0xDC00 == (c & 0xFC00)) {
						*w = s - 1; /* (**w) != 0 */
						*b = d;
						return 0; /* bad utf16 surrogate pair: missing high surrogate */
					}
					else if ((size_t)(e - d) < 3) {
						m = 2;
						break; /* too small output buffer */
					}
					else {
						d += 3;
						c += 0xE0000;
					}
					d[-3] = (utf8_char_t)(c >> 12);
					c = (c & 0xFFF) + 0x2000;
				}
				else if ((size_t)(e - d) < 2) {
					m = 1;
					break; /* too small output buffer */
				}
				else {
					d += 2;
					c += 0x3000;
				}
				d[-2] = (utf8_char_t)(c >> 6);
				c = (c & 0x3F) + 0x80;
			}
			else
				d++;
			d[-1] = (utf8_char_t)c;
			if (!c) {
				sz = (size_t)(d - *b);
				*w = s; /* (*w) points beyond successfully converted 0 */
				*b = d;
				return sz; /* ok, >0 and <= dst buffer size */
			}
		} while (d != e);
		/* too small output buffer */
		sz = (size_t)(d - *b);
		*b = d;
		if (!determ_req_size) {
			const size_t x = m + 1;
#ifdef UBSAN_UNSIGNED_OVERFLOW
			if (sz > (size_t)-1 - x)
				goto too_long_find_nul;
#endif
			sz += x;
#ifndef UBSAN_UNSIGNED_OVERFLOW
			if (sz < x)
				goto too_long_find_nul;
#endif
			*w = s - (m != 0) - (3 == m); /* points beyond the last converted non-0 utf16_char_t */
			return sz; /* ok, >0, but > dst buffer size */
		}
	}
	/* NOTE: assume total size in bytes of input utf16 string, including terminating 0,
	  may be stored in a variable of size_t type without loss, so for each utf16_char_t may
	  safely increment 'm' at least by 2 without integer overflow */
	/* NOTE: also, 'm' will not overflow, if it's zero or have one of next values:
	  1 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
	  2 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
	  3 - at least two utf16_char_t's (4 bytes) were checked, no overflow of 'm' is possible */
	{
		const UTF16_CHAR_T *const t = s - (m != 0) - (3 == m); /* points beyond the last converted non-0 utf16_char_t */
		if (3 == m)
			m = 2;
		for (;;) {
			unsigned c = UTF16_GET(s++);
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						c = UTF16_GET(s);
						if (0xDC00 != (c & 0xFC00)) {
							*w = s - 1; /* (**w) != 0 */
							return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
						}
						s++;
					}
					else if (0xDC00 == (c & 0xFC00)) {
						*w = s - 1; /* (**w) != 0 */
						return 0; /* bad utf16 surrogate pair: missing high surrogate */
					}
					m++;
				}
				m++;
			}
			else if (!c)
				break;
		}
#ifdef UBSAN_UNSIGNED_OVERFLOW
		if (sz > (size_t)-1 - m)
			goto too_long;
#endif
		sz += m;
#ifndef UBSAN_UNSIGNED_OVERFLOW
		if (sz < m)
			goto too_long;
#endif
		m = (size_t)(s - t);
#ifdef UBSAN_UNSIGNED_OVERFLOW
		if (sz > (size_t)-1 - m)
			goto too_long;
#endif
		sz += m;
#ifndef UBSAN_UNSIGNED_OVERFLOW
		if (sz < m)
			goto too_long;
#endif
		*w = t; /* points after the last successfully converted non-0 utf16_char_t */
		return sz; /* ok, >0, but > dst buffer size */
	}
too_long_find_nul:
	while (*s++);
too_long:
	*w = s; /* points after the 0-terminator */
	return 0; /* integer overflow, input string is too long */
}

/*
 utf16_to_utf8_
 utf16x_to_utf8_
 utf16u_to_utf8_
 utf16ux_to_utf8_
*/
size_t UTF_FORM_NAME(_)(
	const UTF16_CHAR_T **const LIBUTF16_RESTRICT w,
	utf8_char_t **const LIBUTF16_RESTRICT b,
	size_t sz, const size_t n, const int determ_req_size)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const UTF16_CHAR_T *LIBUTF16_RESTRICT s = *w;
		const UTF16_CHAR_T *const se = s + n;
		if (sz) {
			utf8_char_t *LIBUTF16_RESTRICT d = *b;
			const utf8_char_t *const e = d + sz;
			do {
				unsigned c = UTF16_GET(s++);
				if (c >= 0x80) {
					if (c >= 0x800) {
						if (0xD800 == (c & 0xFC00)) {
							const unsigned r = (s != se) ? UTF16_GET(s) : 0u;
							if (0xDC00 != (r & 0xFC00)) {
								*w = s - 1; /* (*w) < se */
								*b = d;
								return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
							}
							s++;
							if ((size_t)(e - d) < 4) {
								m = 3;
								break; /* too small output buffer */
							}
							c = (c << 10) + r - 0x20DC00 + 0x800000 + 0x10000;
							d += 4;
							d[-4] = (utf8_char_t)(c >> 18);
							c = (c & 0x3FFFF) + 0x80000;
						}
						else if (0xDC00 == (c & 0xFC00)) {
							*w = s - 1; /* (*w) < se */
							*b = d;
							return 0; /* bad utf16 surrogate pair: missing high surrogate */
						}
						else if ((size_t)(e - d) < 3) {
							m = 2;
							break; /* too small output buffer */
						}
						else {
							d += 3;
							c += 0xE0000;
						}
						d[-3] = (utf8_char_t)(c >> 12);
						c = (c & 0xFFF) + 0x2000;
					}
					else if ((size_t)(e - d) < 2) {
						m = 1;
						break; /* too small output buffer */
					}
					else {
						d += 2;
						c += 0x3000;
					}
					d[-2] = (utf8_char_t)(c >> 6);
					c = (c & 0x3F) + 0x80;
				}
				else
					d++;
				d[-1] = (utf8_char_t)c;
				if (se == s) {
					sz = (size_t)(d - *b);
					*w = s; /* (*w) == se */
					*b = d;
					return sz; /* ok, >0 and <= dst buffer size */
				}
			} while (d != e);
			/* too small output buffer */
			sz = (size_t)(d - *b);
			*b = d;
			if (!determ_req_size) {
				const size_t x = m + 1;
#ifdef UBSAN_UNSIGNED_OVERFLOW
				if (sz > (size_t)-1 - x)
					goto too_long;
#endif
				sz += x;
#ifndef UBSAN_UNSIGNED_OVERFLOW
				if (sz < x)
					goto too_long;
#endif
				*w = s - (m != 0) - (3 == m); /* points beyond the last converted utf16_char_t, (*w) < se */
				return sz; /* ok, >0, but > dst buffer size */
			}
		}
		/* NOTE: assume total size in bytes of input utf16 string,
		  may be stored in a variable of size_t type without loss, so for each utf16_char_t may
		  safely increment 'm' at least by 2 without integer overflow */
		/* NOTE: also, 'm' will not overflow, if it's zero or have one of next values:
		  1 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
		  2 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
		  3 - at least two utf16_char_t's (4 bytes) were checked, no overflow of 'm' is possible */
		{
			const UTF16_CHAR_T *const t = s - (m != 0) - (3 == m); /* points beyond the last converted utf16_char_t, t < se */
			if (3 == m)
				m = 2;
			do {
				unsigned c = UTF16_GET(s++);
				if (c >= 0x80) {
					if (c >= 0x800) {
						if (0xD800 == (c & 0xFC00)) {
							c = (s != se) ? UTF16_GET(s) : 0u;
							if (0xDC00 != (c & 0xFC00)) {
								*w = s - 1; /* (*w) < se */
								return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
							}
							s++;
						}
						else if (0xDC00 == (c & 0xFC00)) {
							*w = s - 1; /* (*w) < se */
							return 0; /* bad utf16 surrogate pair: missing high surrogate */
						}
						m++;
					}
					m++;
				}
			} while (s != se);
#ifdef UBSAN_UNSIGNED_OVERFLOW
			if (sz > (size_t)-1 - m)
				goto too_long;
#endif
			sz += m;
#ifndef UBSAN_UNSIGNED_OVERFLOW
			if (sz < m)
				goto too_long;
#endif
			m = (size_t)(s - t);
#ifdef UBSAN_UNSIGNED_OVERFLOW
			if (sz > (size_t)-1 - m)
				goto too_long;
#endif
			sz += m;
#ifndef UBSAN_UNSIGNED_OVERFLOW
			if (sz < m)
				goto too_long;
#endif
			*w = t; /* points after the last successfully converted utf16_char_t, (*w) < se */
			return sz; /* ok, >0, but > dst buffer size */
		}
too_long:
		*w = se; /* (*w) == se */
		return 0; /* integer overflow, input string is too long */
	}
	return 0; /* n is zero */
}

/*
 utf16_to_utf8_z_unsafe
 utf16x_to_utf8_z_unsafe
 utf16u_to_utf8_z_unsafe
 utf16ux_to_utf8_z_unsafe
*/
const UTF16_CHAR_T *UTF_FORM_NAME(_z_unsafe)(
	const UTF16_CHAR_T *LIBUTF16_RESTRICT w,
	utf8_char_t *const LIBUTF16_RESTRICT buf)
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *LIBUTF16_RESTRICT b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = UTF16_GET(w++);
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (0xD800 == (c & 0xFC00)) {
					c = (c << 10) + (unsigned)UTF16_GET(w++) - 0x20DC00 + 0x800000 + 0x10000;
					b += 4;
					b[-4] = (utf8_char_t)(c >> 18);
					c = (c & 0x3FFFF) + 0x80000;
				}
				else {
					b += 3;
					c += 0xE0000;
				}
				b[-3] = (utf8_char_t)(c >> 12);
				c = (c & 0xFFF) + 0x2000;
			}
			else {
				b += 2;
				c += 0x3000;
			}
			b[-2] = (utf8_char_t)(c >> 6);
			c = (c & 0x3F) + 0x80;
		}
		else
			b++;
		/* make analyzer happy: this equivalent to:
		  b[-1] = (utf8_char_t)c; */
		buf[b - buf - 1] = (utf8_char_t)c;
		if (!c)
			return w; /* ok, w[-1] == 0 */
	}
}

/*
 utf16_to_utf8_unsafe
 utf16x_to_utf8_unsafe
 utf16u_to_utf8_unsafe
 utf16ux_to_utf8_unsafe
*/
void UTF_FORM_NAME(_unsafe)(
	const UTF16_CHAR_T *LIBUTF16_RESTRICT w,
	utf8_char_t *const LIBUTF16_RESTRICT buf,
	const size_t n/*>0*/)
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *LIBUTF16_RESTRICT b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF16_CHAR_T *const we = w + n;
	do {
		unsigned c = UTF16_GET(w++);
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (0xD800 == (c & 0xFC00)) {
					c = (c << 10) + (unsigned)UTF16_GET(w++) - 0x20DC00 + 0x800000 + 0x10000;
					b += 4;
					b[-4] = (utf8_char_t)(c >> 18);
					c = (c & 0x3FFFF) + 0x80000;
				}
				else {
					b += 3;
					c += 0xE0000;
				}
				b[-3] = (utf8_char_t)(c >> 12);
				c = (c & 0xFFF) + 0x2000;
			}
			else {
				b += 2;
				c += 0x3000;
			}
			b[-2] = (utf8_char_t)(c >> 6);
			c = (c & 0x3F) + 0x80;
		}
		else
			b++;
		b[-1] = (utf8_char_t)c;
	} while (w != we);
}
