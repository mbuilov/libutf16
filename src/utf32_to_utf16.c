/**********************************************************************************
* UTF-32 -> UTF-16 characters conversion
* Copyright (C) 2020-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf32_to_utf16.c */

#include <stddef.h> /* for size_t */

#ifndef _MSC_VER
#include <stdint.h> /* for uint32_t/uint16_t */
#endif

#ifdef _MSC_VER
#include <stdlib.h> /* for _byteswap_ushort()/_byteswap_ulong() */
#endif

#include <memory.h> /* for memcpy() */

#include "libutf16/utf32_to_utf16.h"
#include "libutf16/utf16_swap.h"

#include "utf16_internal.h"

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#endif

#ifdef UTF_GET_UNALIGNED
#define UTF32_CHAR_T utf32_char_unaligned_t
#else
#define UTF32_CHAR_T utf32_char_t
#endif

#ifdef UTF_PUT_UNALIGNED
#define UTF16_CHAR_T utf16_char_unaligned_t
#else
#define UTF16_CHAR_T utf16_char_t
#endif

#define UTF_FORM_NAME2(fu, fx, tu, tx, suffix)  utf32##fu##fx##_to_utf16##tu##tx##suffix
#define UTF_FORM_NAME1(fu, fx, tu, tx, suffix)  UTF_FORM_NAME2(fu, fx, tu, tx, suffix)
#define UTF_FORM_NAME(suffix)                   UTF_FORM_NAME1(UTF_GET_U, UTF32_X, UTF_PUT_U, UTF16_X, suffix)

/*
 utf32_to_utf16_z_
 utf32_to_utf16x_z_
 utf32_to_utf16u_z_
 utf32_to_utf16ux_z_
 utf32x_to_utf16_z_
 utf32x_to_utf16x_z_
 utf32x_to_utf16u_z_
 utf32x_to_utf16ux_z_
 utf32u_to_utf16_z_
 utf32u_to_utf16x_z_
 utf32u_to_utf16u_z_
 utf32u_to_utf16ux_z_
 utf32ux_to_utf16_z_
 utf32ux_to_utf16x_z_
 utf32ux_to_utf16u_z_
 utf32ux_to_utf16ux_z_
*/
size_t UTF_FORM_NAME(_z_)(const UTF32_CHAR_T **const w, UTF16_CHAR_T **const b, size_t sz, const int determ_req_size)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF32_CHAR_T *s = *w;
	if (sz) {
		UTF16_CHAR_T *d = *b;
		const UTF16_CHAR_T *const e = (const UTF16_CHAR_T*)d + sz;
		do {
			unsigned c = UTF32_GET(s++);
			if (c > 0xFFFF) {
				if (c > 0x10FFFF) {
					*w = s - 1; /* (**w) != 0 */
					*b = d;
					return 0; /* unicode code point must be <= 0x10FFFF */
				}
				if ((const UTF16_CHAR_T*)d + 1 == e) {
					m = 1;
					break; /* too small output buffer */
				}
				UTF16_PUT(d++, utf32_get_high_surrogate(c));
				c = utf32_get_low_surrogate(c);
			}
			else if (0xD800 <= c && c <= 0xDFFF) {
				*w = s - 1; /* (**w) != 0 */
				*b = d;
				return 0; /* must not be a surrogate */
			}
			UTF16_PUT(d++, (utf16_char_t)c);
			if (!c) {
				sz = (size_t)(d - *b);
				*w = s; /* (*w) points beyond successfully converted 0 */
				*b = d;
				return sz; /* ok, >0 and <= dst buffer size */
			}
		} while ((const UTF16_CHAR_T*)d != e);
		/* too small output buffer */
		sz = (size_t)(d - *b);
		*b = d;
		if (!determ_req_size) {
			*w = s - m; /* points beyond the last converted non-0 utf32_char_t */
			return sz + 1 + m; /* ok, >0, but > dst buffer size */
		}
	}
	/* NOTE: assume total size in bytes of input utf32 string, including terminating 0,
	  may be stored in a variable of size_t type without loss, so for each utf32_char_t may
	  safely increment 'm' at least by 2 without integer overflow */
	{
		const UTF32_CHAR_T *const t = s - m; /* points beyond the last converted non-0 utf32_char_t */
		for (;;) {
			unsigned c = UTF32_GET(s++);
			if (c > 0xFFFF) {
				if (c > 0x10FFFF) {
					*w = s - 1; /* (**w) != 0 */
					return 0; /* unicode code point must be <= 0x10FFFF */
				}
				m++;
			}
			else if (0xD800 <= c && c <= 0xDFFF) {
				*w = s - 1; /* (**w) != 0 */
				return 0; /* must not be a surrogate */
			}
			else if (!c)
				break;
		}
		sz += m + (size_t)(s - t);
		*w = t; /* points after the last successfully converted non-0 utf32_char_t */
		return sz; /* ok, >0, but > dst buffer size */
	}
}

/*
 utf32_to_utf16_
 utf32_to_utf16x_
 utf32_to_utf16u_
 utf32_to_utf16ux_
 utf32x_to_utf16_
 utf32x_to_utf16x_
 utf32x_to_utf16u_
 utf32x_to_utf16ux_
 utf32u_to_utf16_
 utf32u_to_utf16x_
 utf32u_to_utf16u_
 utf32u_to_utf16ux_
 utf32ux_to_utf16_
 utf32ux_to_utf16x_
 utf32ux_to_utf16u_
 utf32ux_to_utf16ux_
*/
size_t UTF_FORM_NAME(_)(const UTF32_CHAR_T **const w, UTF16_CHAR_T **const b, size_t sz, const size_t n, const int determ_req_size)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const UTF32_CHAR_T *s = *w;
		const UTF32_CHAR_T *const se = s + n;
		if (sz) {
			UTF16_CHAR_T *d = *b;
			const UTF16_CHAR_T *const e = (const UTF16_CHAR_T*)d + sz;
			do {
				unsigned c = UTF32_GET(s++);
				if (c > 0xFFFF) {
					if (c > 0x10FFFF) {
						*w = s - 1; /* (*w) < se */
						*b = d;
						return 0; /* unicode code point must be <= 0x10FFFF */
					}
					if ((const UTF16_CHAR_T*)d + 1 == e) {
						m = 1;
						break; /* too small output buffer */
					}
					UTF16_PUT(d++, utf32_get_high_surrogate(c));
					c = utf32_get_low_surrogate(c);
				}
				else if (0xD800 <= c && c <= 0xDFFF) {
					*w = s - 1; /* (*w) < se */
					*b = d;
					return 0; /* must not be a surrogate */
				}
				UTF16_PUT(d++, (utf16_char_t)c);
				if (se == s) {
					sz = (size_t)(d - *b);
					*w = s; /* (*w) == se */
					*b = d;
					return sz; /* ok, >0 and <= dst buffer size */
				}
			} while ((const UTF16_CHAR_T*)d != e);
			/* too small output buffer */
			sz = (size_t)(d - *b);
			*b = d;
			if (!determ_req_size) {
				*w = s - m; /* points beyond the last converted utf32_char_t, (*w) < se */
				return sz + 1 + m; /* ok, >0, but > dst buffer size */
			}
		}
		/* NOTE: assume total size in bytes of input utf32 string,
		  may be stored in a variable of size_t type without loss, so for each utf32_char_t may
		  safely increment 'm' at least by 2 without integer overflow */
		{
			const UTF32_CHAR_T *const t = s - m; /* points beyond the last converted utf32_char_t, t < se */
			do {
				unsigned c = UTF32_GET(s++);
				if (c > 0xFFFF) {
					if (c > 0x10FFFF) {
						*w = s - 1; /* (*w) < se */
						return 0; /* unicode code point must be <= 0x10FFFF */
					}
					m++;
				}
				else if (0xD800 <= c && c <= 0xDFFF) {
					*w = s - 1; /* (*w) < se */
					return 0; /* must not be a surrogate */
				}
			} while (s != se);
			sz += m + (size_t)(s - t);
			*w = t; /* points after the last successfully converted utf32_char_t, (*w) < se */
			return sz; /* ok, >0, but > dst buffer size */
		}
	}
	return 0; /* n is zero */
}

/*
 utf32_to_utf16_z_unsafe
 utf32_to_utf16x_z_unsafe
 utf32_to_utf16u_z_unsafe
 utf32_to_utf16ux_z_unsafe
 utf32x_to_utf16_z_unsafe
 utf32x_to_utf16x_z_unsafe
 utf32x_to_utf16u_z_unsafe
 utf32x_to_utf16ux_z_unsafe
 utf32u_to_utf16_z_unsafe
 utf32u_to_utf16x_z_unsafe
 utf32u_to_utf16u_z_unsafe
 utf32u_to_utf16ux_z_unsafe
 utf32ux_to_utf16_z_unsafe
 utf32ux_to_utf16x_z_unsafe
 utf32ux_to_utf16u_z_unsafe
 utf32ux_to_utf16ux_z_unsafe
*/
const UTF32_CHAR_T *UTF_FORM_NAME(_z_unsafe)(const UTF32_CHAR_T *w, UTF16_CHAR_T buf[])
{
	/* unsigned integer type must be at least of 32 bits */
	UTF16_CHAR_T *b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = UTF32_GET(w++);
		if (c > 0xFFFF) {
			UTF16_PUT(b++, utf32_get_high_surrogate(c));
			c = utf32_get_low_surrogate(c);
		}
		UTF16_PUT(b++, (utf16_char_t)c);
		if (!c)
			return w; /* ok, w[-1] == 0 */
	}
}

/*
 utf32_to_utf16_unsafe
 utf32_to_utf16x_unsafe
 utf32_to_utf16u_unsafe
 utf32_to_utf16ux_unsafe
 utf32x_to_utf16_unsafe
 utf32x_to_utf16x_unsafe
 utf32x_to_utf16u_unsafe
 utf32x_to_utf16ux_unsafe
 utf32u_to_utf16_unsafe
 utf32u_to_utf16x_unsafe
 utf32u_to_utf16u_unsafe
 utf32u_to_utf16ux_unsafe
 utf32ux_to_utf16_unsafe
 utf32ux_to_utf16x_unsafe
 utf32ux_to_utf16u_unsafe
 utf32ux_to_utf16ux_unsafe
*/
void UTF_FORM_NAME(_unsafe)(const UTF32_CHAR_T *w, UTF16_CHAR_T buf[], const size_t n/*>0*/)
{
	/* unsigned integer type must be at least of 32 bits */
	UTF16_CHAR_T *b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF32_CHAR_T *const we = w + n;
	do {
		unsigned c = UTF32_GET(w++);
		if (c > 0xFFFF) {
			UTF16_PUT(b++, utf32_get_high_surrogate(c));
			c = utf32_get_low_surrogate(c);
		}
		UTF16_PUT(b++, (utf16_char_t)c);
	} while (w != we);
}
