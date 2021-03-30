/**********************************************************************************
* UTF-16 -> UTF-32 characters conversion
* Copyright (C) 2020-2021 Michael M. Builov, https://github.com/mbuilov/libutf16
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

#include "libutf16/utf16_swap.h"
#include "libutf16/utf16_to_utf32.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#define A_Restrict
#endif

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#endif

A_Use_decl_annotations
size_t
#if defined(SWAP_UTF16) && defined(SWAP_UTF32)
utf16x_to_utf32x_z_
#elif defined(SWAP_UTF16)
utf16x_to_utf32_z_
#elif defined(SWAP_UTF32)
utf16_to_utf32x_z_
#else
utf16_to_utf32_z_
#endif
(const utf16_char_t **const q, utf32_char_t **const b, size_t sz, const int determ_req_size)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf16_char_t *A_Restrict s = *q;
	if (sz) {
		utf32_char_t *A_Restrict d = *b;
		const utf32_char_t *const e = d + sz;
		do {
			unsigned c = UTF16_CVT(*s++);
			if (0xD800 == (c & 0xFC00)) {
				const unsigned r = UTF16_CVT(*s);
				if (0xDC00 != (r & 0xFC00)) {
					*q = s - 1; /* (**q) != 0 */
					*b = d;
					return 0; /* bad utf16 surrogate pair: no lower surrogate */
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
				return 0; /* bad utf16 surrogate pair: no high surrogate */
			}
			*d++ = UTF32_CVT((utf32_char_t)c);
			if (!c) {
				sz = (size_t)(d - *b);
				*q = s; /* (*q) points beyond successfully converted 0 */
				*b = d;
				return sz; /* ok, >0 and <= dst buffer size */
			}
		} while (d != e);
		/* too small output buffer */
		sz = (size_t)(d - *b);
		*b = d;
		if (!determ_req_size) {
			*q = s; /* points beyond the last converted non-0 utf16_char_t */
			return sz + 1; /* ok, >0, but > dst buffer size */
		}
	}
	{
		const utf16_char_t *const t = s; /* points beyond the last converted non-0 utf16_char_t */
		for (;;) {
			unsigned c = UTF16_CVT(*s++);
			if (0xD800 == (c & 0xFC00)) {
				c = UTF16_CVT(*s);
				if (0xDC00 != (c & 0xFC00)) {
					*q = s - 1; /* (**q) != 0 */
					return 0; /* bad utf16 surrogate pair: no lower surrogate */
				}
				s++;
				m++;
			}
			else if (0xDC00 == (c & 0xFC00)) {
				*q = s - 1; /* (**q) != 0 */
				return 0; /* bad utf16 surrogate pair: no high surrogate */
			}
			else if (!c)
				break;
		}
		sz += (size_t)(s - t) - m;
		*q = t; /* points after the last successfully converted non-0 utf16_char_t */
		return sz; /* ok, >0, but > dst buffer size */
	}
}

A_Use_decl_annotations
size_t
#if defined(SWAP_UTF16) && defined(SWAP_UTF32)
utf16x_to_utf32x_
#elif defined(SWAP_UTF16)
utf16x_to_utf32_
#elif defined(SWAP_UTF32)
utf16_to_utf32x_
#else
utf16_to_utf32_
#endif
(const utf16_char_t **const q, utf32_char_t **const b, size_t sz, const size_t n, const int determ_req_size)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const utf16_char_t *A_Restrict s = *q;
		const utf16_char_t *const se = s + n;
		if (sz) {
			utf32_char_t *A_Restrict d = *b;
			const utf32_char_t *const e = d + sz;
			do {
				unsigned c = UTF16_CVT(*s++);
				if (0xD800 == (c & 0xFC00)) {
					const unsigned r = (s != se) ? UTF16_CVT(*s) : 0u;
					if (0xDC00 != (r & 0xFC00)) {
						*q = s - 1; /* (*q) < se */
						*b = d;
						return 0; /* bad utf16 surrogate pair: no lower surrogate */
					}
					s++;
					c = (c << 10) + r - 0x360DC00 + 0x10000;
				}
				else if (0xDC00 == (c & 0xFC00)) {
					*q = s - 1; /* (*q) < se */
					*b = d;
					return 0; /* bad utf16 surrogate pair: no high surrogate */
				}
				*d++ = UTF32_CVT((utf32_char_t)c);
				if (se == s) {
					sz = (size_t)(d - *b);
					*q = s; /* (*q) == se */
					*b = d;
					return sz; /* ok, >0 and <= dst buffer size */
				}
			} while (d != e);
			/* too small output buffer */
			sz = (size_t)(d - *b);
			*b = d;
			if (!determ_req_size) {
				*q = s; /* points beyond the last converted utf16_char_t, (*q) < se */
				return sz + 1; /* ok, >0, but > dst buffer size */
			}
		}
		{
			const utf16_char_t *const t = s; /* points beyond the last converted utf16_char_t, t < se */
			do {
				unsigned c = UTF16_CVT(*s++);
				if (0xD800 == (c & 0xFC00)) {
					c = (s != se) ? UTF16_CVT(*s) : 0u;
					if (0xDC00 != (c & 0xFC00)) {
						*q = s - 1; /* (*q) < se */
						return 0; /* bad utf16 surrogate pair: no lower surrogate */
					}
					s++;
					m++;
				}
				else if (0xDC00 == (c & 0xFC00)) {
					*q = s - 1; /* (*q) < se */
					return 0; /* bad utf16 surrogate pair: no high surrogate */
				}
			} while (s != se);
			sz += (size_t)(s - t) - m;
			*q = t; /* points after the last successfully converted utf16_char_t, (*q) < se */
			return sz; /* ok, >0, but > dst buffer size */
		}
	}
	return 0; /* n is zero */
}

A_Use_decl_annotations
const utf16_char_t *
#if defined(SWAP_UTF16) && defined(SWAP_UTF32)
utf16x_to_utf32x_z_unsafe
#elif defined(SWAP_UTF16)
utf16x_to_utf32_z_unsafe
#elif defined(SWAP_UTF32)
utf16_to_utf32x_z_unsafe
#else
utf16_to_utf32_z_unsafe
#endif
(const utf16_char_t *q, utf32_char_t buf[])
{
	/* unsigned integer type must be at least of 32 bits */
	utf32_char_t *A_Restrict b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = UTF16_CVT(*q++);
		if (0xD800 == (c & 0xFC00)) {
			const unsigned r = UTF16_CVT(*q++);
			c = (c << 10) + r - 0x360DC00 + 0x10000;
		}
		*b++ = UTF32_CVT((utf32_char_t)c);
		if (!c)
			return q; /* ok, q[-1] == 0 */
	}
}

A_Use_decl_annotations
void
#if defined(SWAP_UTF16) && defined(SWAP_UTF32)
utf16x_to_utf32x_unsafe
#elif defined(SWAP_UTF16)
utf16x_to_utf32_unsafe
#elif defined(SWAP_UTF32)
utf16_to_utf32x_unsafe
#else
utf16_to_utf32_unsafe
#endif
(const utf16_char_t *q, utf32_char_t buf[], const size_t n/*>0*/)
{
	/* unsigned integer type must be at least of 32 bits */
	utf32_char_t *A_Restrict b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf16_char_t *const qe = q + n;
	do {
		unsigned c = UTF16_CVT(*q++);
		if (0xD800 == (c & 0xFC00)) {
			unsigned r;
#ifdef _MSC_VER
			__assume(n > 1); /* assume input string is valid */
#endif
			r = UTF16_CVT(*q++);
			c = (c << 10) + r - 0x360DC00 + 0x10000;
		}
		*b++ = UTF32_CVT((utf32_char_t)c);
	} while (q != qe);
}
