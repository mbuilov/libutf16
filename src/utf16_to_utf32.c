/**********************************************************************************
* UTF-16 -> UTF-32 characters conversion
* Copyright (C) 2020-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf32.c */

#include <stddef.h> /* for size_t */

#ifndef _MSC_VER
#include <stdint.h> /* for uint32_t/uint16_t */
#endif

#ifdef _MSC_VER
#include <stdlib.h> /* for _byteswap_ushort()/_byteswap_ulong() */
#endif

#include <memory.h> /* for memcpy() */

#include "libutf16/utf16_to_utf32.h"
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

#ifdef UTF_PUT_UNALIGNED
#define UTF32_CHAR_T utf32_char_unaligned_t
#else
#define UTF32_CHAR_T utf32_char_t
#endif

#define UTF_FORM_NAME2(fu, fx, tu, tx, suffix)  utf16##fu##fx##_to_utf32##tu##tx##suffix
#define UTF_FORM_NAME1(fu, fx, tu, tx, suffix)  UTF_FORM_NAME2(fu, fx, tu, tx, suffix)
#define UTF_FORM_NAME(suffix)                   UTF_FORM_NAME1(UTF_GET_U, UTF16_X, UTF_PUT_U, UTF32_X, suffix)

/*
 utf16_to_utf32_z_
 utf16_to_utf32x_z_
 utf16_to_utf32u_z_
 utf16_to_utf32ux_z_
 utf16x_to_utf32_z_
 utf16x_to_utf32x_z_
 utf16x_to_utf32u_z_
 utf16x_to_utf32ux_z_
 utf16u_to_utf32_z_
 utf16u_to_utf32x_z_
 utf16u_to_utf32u_z_
 utf16u_to_utf32ux_z_
 utf16ux_to_utf32_z_
 utf16ux_to_utf32x_z_
 utf16ux_to_utf32u_z_
 utf16ux_to_utf32ux_z_
*/
size_t UTF_FORM_NAME(_z_)(
	const UTF16_CHAR_T **const LIBUTF16_RESTRICT q,
	UTF32_CHAR_T **const LIBUTF16_RESTRICT b,
	size_t sz, int determ_size)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF16_CHAR_T *LIBUTF16_RESTRICT s = *q;
	if (sz) {
		UTF32_CHAR_T *LIBUTF16_RESTRICT d = *b;
		const UTF32_CHAR_T *const e = (const UTF32_CHAR_T*)d + sz;
		do {
			unsigned c = UTF16_GET(s++);
			if (0xD800 == (c & 0xFC00)) {
				const unsigned r = UTF16_GET(s);
				if (0xDC00 != (r & 0xFC00)) {
					*q = s - 1; /* (**q) != 0 */
					*b = d;
					return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
				}
				s++;
				/* 110110xxyyyyyyyy0000000000
				  +          110111aabbbbbbbb
				  -11011000001101110000000000
				  ===========================
				         xxyyyyyyyyaabbbbbbbb
				  +         10000000000000000 */
				c = (c << 10) + r - 0x360DC00 + 0x10000;
			}
			else if (0xDC00 == (c & 0xFC00)) {
				*q = s - 1; /* (**q) != 0 */
				*b = d;
				return 0; /* bad utf16 surrogate pair: missing high surrogate */
			}
			UTF32_PUT(d++, (utf32_char_t)c);
			if (!c) {
				sz = (size_t)(d - *b);
				*q = s; /* (*q) points beyond successfully converted 0 */
				*b = d;
				return sz; /* ok, >0 and <= dst buffer size */
			}
		} while ((const UTF32_CHAR_T*)d != e);
		/* too small output buffer */
		sz = (size_t)(d - *b);
		*b = d;
		if (!determ_size) {
			*q = s; /* points beyond the last converted non-0 utf16_char_t */
			return sz + 1; /* ok, >0, but > dst buffer size */
		}
	}
	else if (!determ_size)
		return 1;
	else if (2 == determ_size)
		determ_size = 0;
	{
		const UTF16_CHAR_T *const t = s; /* points beyond the last converted non-0 utf16_char_t */
		for (;;) {
			unsigned c = UTF16_GET(s++);
			if (0xD800 == (c & 0xFC00)) {
				c = UTF16_GET(s);
				if (0xDC00 != (c & 0xFC00)) {
					*q = s - 1; /* (**q) != 0 */
					return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
				}
				s++;
				m++;
			}
			else if (0xDC00 == (c & 0xFC00)) {
				*q = s - 1; /* (**q) != 0 */
				return 0; /* bad utf16 surrogate pair: missing high surrogate */
			}
			else if (!c)
				break;
		}
		sz += (size_t)(s - t) - m;
		*q = !determ_size ? s : t/* points after the last successfully converted non-0 utf16_char_t */;
		return sz; /* ok, >0, but > dst buffer size */
	}
}

/*
 utf16_to_utf32_
 utf16_to_utf32x_
 utf16_to_utf32u_
 utf16_to_utf32ux_
 utf16x_to_utf32_
 utf16x_to_utf32x_
 utf16x_to_utf32u_
 utf16x_to_utf32ux_
 utf16u_to_utf32_
 utf16u_to_utf32x_
 utf16u_to_utf32u_
 utf16u_to_utf32ux_
 utf16ux_to_utf32_
 utf16ux_to_utf32x_
 utf16ux_to_utf32u_
 utf16ux_to_utf32ux_
*/
size_t UTF_FORM_NAME(_)(
	const UTF16_CHAR_T **const LIBUTF16_RESTRICT q,
	UTF32_CHAR_T **const LIBUTF16_RESTRICT b,
	size_t sz, const size_t n, const int determ_size)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const UTF16_CHAR_T *LIBUTF16_RESTRICT s = *q;
		const UTF16_CHAR_T *const se = s + n;
		if (sz) {
			UTF32_CHAR_T *LIBUTF16_RESTRICT d = *b;
			const UTF32_CHAR_T *const e = (const UTF32_CHAR_T*)d + sz;
			do {
				unsigned c = UTF16_GET(s++);
				if (0xD800 == (c & 0xFC00)) {
					const unsigned r = (s != se) ? UTF16_GET(s) : 0u;
					if (0xDC00 != (r & 0xFC00)) {
						*q = s - 1; /* (*q) < se */
						*b = d;
						return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
					}
					s++;
					c = (c << 10) + r - 0x360DC00 + 0x10000;
				}
				else if (0xDC00 == (c & 0xFC00)) {
					*q = s - 1; /* (*q) < se */
					*b = d;
					return 0; /* bad utf16 surrogate pair: missing high surrogate */
				}
				UTF32_PUT(d++, (utf32_char_t)c);
				if (se == s) {
					sz = (size_t)(d - *b);
					*q = s; /* (*q) == se */
					*b = d;
					return sz; /* ok, >0 and <= dst buffer size */
				}
			} while ((const UTF32_CHAR_T*)d != e);
			/* too small output buffer */
			sz = (size_t)(d - *b);
			*b = d;
			if (!determ_size) {
				*q = s; /* points beyond the last converted utf16_char_t, (*q) < se */
				return sz + 1; /* ok, >0, but > dst buffer size */
			}
		}
		else if (!determ_size)
			return 1;
		{
			const UTF16_CHAR_T *const t = s; /* points beyond the last converted utf16_char_t, t < se */
			do {
				unsigned c = UTF16_GET(s++);
				if (0xD800 == (c & 0xFC00)) {
					c = (s != se) ? UTF16_GET(s) : 0u;
					if (0xDC00 != (c & 0xFC00)) {
						*q = s - 1; /* (*q) < se */
						return 0; /* bad utf16 surrogate pair: expecting lower surrogate */
					}
					s++;
					m++;
				}
				else if (0xDC00 == (c & 0xFC00)) {
					*q = s - 1; /* (*q) < se */
					return 0; /* bad utf16 surrogate pair: missing high surrogate */
				}
			} while (s != se);
			sz += (size_t)(s - t) - m;
			*q = t; /* points after the last successfully converted utf16_char_t, (*q) < se */
			return sz; /* ok, >0, but > dst buffer size */
		}
	}
	return 0; /* n is zero */
}

/*
 utf16_to_utf32_z_unsafe
 utf16_to_utf32x_z_unsafe
 utf16_to_utf32u_z_unsafe
 utf16_to_utf32ux_z_unsafe
 utf16x_to_utf32_z_unsafe
 utf16x_to_utf32x_z_unsafe
 utf16x_to_utf32u_z_unsafe
 utf16x_to_utf32ux_z_unsafe
 utf16u_to_utf32_z_unsafe
 utf16u_to_utf32x_z_unsafe
 utf16u_to_utf32u_z_unsafe
 utf16u_to_utf32ux_z_unsafe
 utf16ux_to_utf32_z_unsafe
 utf16ux_to_utf32x_z_unsafe
 utf16ux_to_utf32u_z_unsafe
 utf16ux_to_utf32ux_z_unsafe
*/
const UTF16_CHAR_T *UTF_FORM_NAME(_z_unsafe)(
	const UTF16_CHAR_T *LIBUTF16_RESTRICT q,
	UTF32_CHAR_T *const LIBUTF16_RESTRICT buf)
{
	/* unsigned integer type must be at least of 32 bits */
	UTF32_CHAR_T *LIBUTF16_RESTRICT b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = UTF16_GET(q++);
		if (0xD800 == (c & 0xFC00)) {
			const unsigned r = UTF16_GET(q++);
			c = (c << 10) + r - 0x360DC00 + 0x10000;
		}
		UTF32_PUT(b++, (utf32_char_t)c);
		if (!c)
			return q; /* ok, q[-1] == 0 */
	}
}

/*
 utf16_to_utf32_unsafe
 utf16_to_utf32x_unsafe
 utf16_to_utf32u_unsafe
 utf16_to_utf32ux_unsafe
 utf16x_to_utf32_unsafe
 utf16x_to_utf32x_unsafe
 utf16x_to_utf32u_unsafe
 utf16x_to_utf32ux_unsafe
 utf16u_to_utf32_unsafe
 utf16u_to_utf32x_unsafe
 utf16u_to_utf32u_unsafe
 utf16u_to_utf32ux_unsafe
 utf16ux_to_utf32_unsafe
 utf16ux_to_utf32x_unsafe
 utf16ux_to_utf32u_unsafe
 utf16ux_to_utf32ux_unsafe
*/
void UTF_FORM_NAME(_unsafe)(
	const UTF16_CHAR_T *LIBUTF16_RESTRICT q,
	UTF32_CHAR_T *const LIBUTF16_RESTRICT buf,
	const size_t n/*>0*/)
{
	/* unsigned integer type must be at least of 32 bits */
	UTF32_CHAR_T *LIBUTF16_RESTRICT b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF16_CHAR_T *const qe = q + n;
	do {
		unsigned c = UTF16_GET(q++);
		if (0xD800 == (c & 0xFC00)) {
			unsigned r;
#ifdef _MSC_VER
			__assume(n > 1); /* assume input string is valid */
#endif
			r = UTF16_GET(q++);
			c = (c << 10) + r - 0x360DC00 + 0x10000;
		}
		UTF32_PUT(b++, (utf32_char_t)c);
	} while (q != qe);
}
