/**********************************************************************************
* UTF-32 -> UTF-8 characters conversion
* Copyright (C) 2020-2021 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf32_to_utf8.c */

#include <stddef.h> /* for size_t */

#ifndef _MSC_VER
#include <stdint.h> /* for uint32_t */
#endif

#ifdef _MSC_VER
#include <stdlib.h> /* for _byteswap_ushort()/_byteswap_ulong() */
#endif

#include <memory.h> /* for memcpy() */

#include "libutf16/utf32_to_utf8.h"
#include "libutf16/utf16_swap.h"

#include "utf16_internal.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#define A_Restrict
#endif

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#endif

#ifdef UTF_GET_UNALIGNED
#define UTF32_CHAR_T utf32_char_unaligned_t
#else
#define UTF32_CHAR_T utf32_char_t
#endif

#define UTF_FORM_NAME2(fu, fx, suffix)  utf32##fu##fx##_to_utf8##suffix
#define UTF_FORM_NAME1(fu, fx, suffix)  UTF_FORM_NAME2(fu, fx, suffix)
#define UTF_FORM_NAME(suffix)           UTF_FORM_NAME1(UTF_GET_U, UTF32_X, suffix)

/*
 utf32_to_utf8_z_
 utf32x_to_utf8_z_
 utf32u_to_utf8_z_
 utf32ux_to_utf8_z_
*/
A_Use_decl_annotations
size_t UTF_FORM_NAME(_z_)(const UTF32_CHAR_T **const w, utf8_char_t **const b, size_t sz, const int determ_req_size)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF32_CHAR_T *A_Restrict s = *w;
	if (sz) {
		utf8_char_t *A_Restrict d = *b;
		const utf8_char_t *const e = d + sz;
		do {
			unsigned c = UTF32_GET(s++);
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (c > 0xFFFF) {
						if (c > 0x10FFFF) {
							*w = s - 1; /* (**w) != 0 */
							*b = d;
							return 0; /* unicode code point must be <= 0x10FFFF */
						}
						if ((size_t)(e - d) < 4) {
							m = 3;
							break; /* too small output buffer */
						}
						c += 0x3C00000;
						d += 4;
						d[-4] = (utf8_char_t)(c >> 18);
						c = (c & 0x3FFFF) + 0x80000;
					}
					else if (0xD800 <= c && c <= 0xDFFF) {
						*w = s - 1; /* (**w) != 0 */
						*b = d;
						return 0; /* must not be a surrogate */
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
			*w = s - (m != 0); /* points beyond the last converted non-0 utf32_char_t */
			return sz + 1 + m; /* ok, >0, but > dst buffer size */
		}
	}
	/* NOTE: assume total size in bytes of input utf32 string, including terminating 0,
	  may be stored in a variable of size_t type without loss, so for each utf32_char_t may
	  safely increment 'm' at least by 4 without integer overflow */
	{
		const UTF32_CHAR_T *const t = s - (m != 0); /* points beyond the last converted non-0 utf32_char_t */
		for (;;) {
			unsigned c = UTF32_GET(s++);
			if (c >= 0x80) {
				if (c >= 0x800) {
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
					m++;
				}
				m++;
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
 utf32_to_utf8_
 utf32x_to_utf8_
 utf32u_to_utf8_
 utf32ux_to_utf8_
*/
A_Use_decl_annotations
size_t UTF_FORM_NAME(_)(const UTF32_CHAR_T **const w, utf8_char_t **const b, size_t sz, const size_t n, const int determ_req_size)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const UTF32_CHAR_T *A_Restrict s = *w;
		const UTF32_CHAR_T *const se = s + n;
		if (sz) {
			utf8_char_t *A_Restrict d = *b;
			const utf8_char_t *const e = d + sz;
			do {
				unsigned c = UTF32_GET(s++);
				if (c >= 0x80) {
					if (c >= 0x800) {
						if (c > 0xFFFF) {
							if (c > 0x10FFFF) {
								*w = s - 1; /* (*w) < se */
								*b = d;
								return 0; /* unicode code point must be <= 0x10FFFF */
							}
							if ((size_t)(e - d) < 4) {
								m = 3;
								break; /* too small output buffer */
							}
							c += 0x3C00000;
							d += 4;
							d[-4] = (utf8_char_t)(c >> 18);
							c = (c & 0x3FFFF) + 0x80000;
						}
						else if (0xD800 <= c && c <= 0xDFFF) {
							*w = s - 1; /* (*w) < se */
							*b = d;
							return 0; /* must not be a surrogate */
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
				*w = s - (m != 0); /* points beyond the last converted utf32_char_t, (*w) < se */
				return sz + 1 + m; /* ok, >0, but > dst buffer size */
			}
		}
		/* NOTE: assume total size in bytes of input utf32 string,
		  may be stored in a variable of size_t type without loss, so for each utf32_char_t may
		  safely increment 'm' at least by 4 without integer overflow */
		{
			const UTF32_CHAR_T *const t = s - (m != 0); /* points beyond the last converted utf32_char_t, t < se */
			do {
				unsigned c = UTF32_GET(s++);
				if (c >= 0x80) {
					if (c >= 0x800) {
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
						m++;
					}
					m++;
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
 utf32_to_utf8_z_unsafe
 utf32x_to_utf8_z_unsafe
 utf32u_to_utf8_z_unsafe
 utf32ux_to_utf8_z_unsafe
*/
A_Use_decl_annotations
const UTF32_CHAR_T *UTF_FORM_NAME(_z_unsafe)(const UTF32_CHAR_T *w, utf8_char_t buf[])
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *A_Restrict b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = UTF32_GET(w++);
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (c > 0xFFFF) {
					c += 0x3C00000;
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
 utf32_to_utf8_unsafe
 utf32x_to_utf8_unsafe
 utf32u_to_utf8_unsafe
 utf32ux_to_utf8_unsafe
*/
A_Use_decl_annotations
void UTF_FORM_NAME(_unsafe)(const UTF32_CHAR_T *w, utf8_char_t buf[], const size_t n/*>0*/)
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *A_Restrict b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const UTF32_CHAR_T *const we = w + n;
	do {
		unsigned c = UTF32_GET(w++);
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (c > 0xFFFF) {
					c += 0x3C00000;
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
