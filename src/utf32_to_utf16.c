/**********************************************************************************
* UTF-32 -> UTF-16 characters conversion
* Copyright (C) 2020 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf32_to_utf16.c */

#include <stddef.h> /* for size_t */
#ifndef _WIN32
#include <stdint.h> /* for uint32_t/uint16_t */
#endif
#include "libutf16/utf32_to_utf16.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#define A_Restrict
#endif

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#endif

A_Use_decl_annotations
size_t utf32_to_utf16_z_(const utf32_char_t **const w,
	utf16_char_t **const b, size_t sz, const int determ_req_size)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf32_char_t *A_Restrict s = *w;
	if (sz) {
		utf16_char_t *A_Restrict d = *b;
		const utf16_char_t *const e = d + sz;
		do {
			unsigned c = *s++;
			if (c > 0xFFFF) {
				if (c > 0x10FFFF) {
					*w = s - 1; /* (**w) != 0 */
					*b = d;
					return 0; /* unicode code point must be <= 0x10FFFF */
				}
				if (d + 1 == e) {
					m = 1;
					break; /* too small output buffer */
				}
				*d++ = utf32_get_high_surrogate(c);
				c = utf32_get_low_surrogate(c);
			}
			else if (0xD800 <= c && c <= 0xDFFF) {
				*w = s - 1; /* (**w) != 0 */
				*b = d;
				return 0; /* must not be a surrogate */
			}
			*d++ = (utf16_char_t)c;
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
			*w = s - m; /* points beyond the last converted non-0 utf32_char_t */
			return sz + 1 + m; /* ok, >0, but > dst buffer size */
		}
	}
	/* NOTE: assume total size in bytes of input utf32 string, including terminating 0,
	  may be stored in a variable of size_t type without loss, so for each utf32_char_t may
	  safely increment 'm' at least by 2 without integer overflow */
	{
		const utf32_char_t *const t = s - m; /* points beyond the last converted non-0 utf32_char_t */
		for (;;) {
			unsigned c = *s++;
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

A_Use_decl_annotations
size_t utf32_to_utf16_(const utf32_char_t **const w,
	utf16_char_t **const b, size_t sz, const size_t n, const int determ_req_size)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const utf32_char_t *A_Restrict s = *w;
		const utf32_char_t *const se = s + n;
		if (sz) {
			utf16_char_t *A_Restrict d = *b;
			const utf16_char_t *const e = d + sz;
			do {
				unsigned c = *s++;
				if (c > 0xFFFF) {
					if (c > 0x10FFFF) {
						*w = s - 1; /* (*w) < se */
						*b = d;
						return 0; /* unicode code point must be <= 0x10FFFF */
					}
					if (d + 1 == e) {
						m = 1;
						break; /* too small output buffer */
					}
					*d++ = utf32_get_high_surrogate(c);
					c = utf32_get_low_surrogate(c);
				}
				else if (0xD800 <= c && c <= 0xDFFF) {
					*w = s - 1; /* (*w) < se */
					*b = d;
					return 0; /* must not be a surrogate */
				}
				*d++ = (utf16_char_t)c;
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
				*w = s - m; /* points beyond the last converted utf32_char_t, (*w) < se */
				return sz + 1 + m; /* ok, >0, but > dst buffer size */
			}
		}
		/* NOTE: assume total size in bytes of input utf32 string,
		  may be stored in a variable of size_t type without loss, so for each utf32_char_t may
		  safely increment 'm' at least by 2 without integer overflow */
		{
			const utf32_char_t *const t = s - m; /* points beyond the last converted utf32_char_t, t < se */
			do {
				unsigned c = *s++;
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

A_Use_decl_annotations
const utf32_char_t *utf32_to_utf16_z_unsafe(const utf32_char_t *w, utf16_char_t buf[])
{
	/* unsigned integer type must be at least of 32 bits */
	utf16_char_t *A_Restrict b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = *w++;
		if (c > 0xFFFF) {
			*b++ = utf32_get_high_surrogate(c);
			c = utf32_get_low_surrogate(c);
		}
		*b++ = (utf16_char_t)c;
		if (!c)
			return w; /* ok, w[-1] == 0 */
	}
}

A_Use_decl_annotations
void utf32_to_utf16_unsafe(const utf32_char_t *w, utf16_char_t buf[], const size_t n/*>0*/)
{
	/* unsigned integer type must be at least of 32 bits */
	utf16_char_t *A_Restrict b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf32_char_t *const we = w + n;
	do {
		unsigned c = *w++;
		if (c > 0xFFFF) {
			*b++ = utf32_get_high_surrogate(c);
			c = utf32_get_low_surrogate(c);
		}
		*b++ = (utf16_char_t)c;
	} while (w != we);
}
