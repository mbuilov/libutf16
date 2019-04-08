/**********************************************************************************
* UTF-16 -> UTF-8 characters conversion
* Copyright (C) 2018-2019 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8.c */

#include <stddef.h> /* for size_t */
#ifndef _MSC_VER
#include <stdint.h> /* for uint16_t */
#endif
#include "libutf16/utf16_to_utf8.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#define A_Restrict
#endif

A_Use_decl_annotations
size_t utf16_to_utf8_z(const utf16_char_t **const w, utf8_char_t **const b, size_t sz)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf16_char_t *A_Restrict s = *w;
	if (sz) {
		utf8_char_t *A_Restrict d = *b;
		const utf8_char_t *const e = d + sz;
		do {
			unsigned c = *s++;
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						const unsigned r = *s;
						if (0xDC00 != (r & 0xFC00)) {
							*w = s - 1; /* (**w) != 0 */
							*b = d;
							return 0; /* bad utf16 surrogate pair: no lower surrogate */
						}
						s++;
						if ((size_t)(e - d) < 4) {
							m = 3;
							break; /* too small output buffer */
						}
						/* c is in range: 0xD800..0xDBFF, i.e.: 110110xxyyyyyyyy */
						/* r is in range: 0xDC00..0xDFFF, i.e.: 110111aabbbbbbbb */
						c = ((c << 10) ^ r ^ 0xA0DC00) + 0x10000;
						d += 4;
						d[-4] = (utf8_char_t)(c >> 18);
						c = (c & 0x3FFFF) | 0x80000;
					}
					else if (0xDC00 == (c & 0xFC00)) {
						*w = s - 1; /* (**w) != 0 */
						*b = d;
						return 0; /* bad utf16 surrogate pair: no high surrogate */
					}
					else if ((size_t)(e - d) < 3) {
						m = 2;
						break; /* too small output buffer */
					}
					else {
						d += 3;
						c |= 0xE0000;
					}
					d[-3] = (utf8_char_t)(c >> 12);
					c = (c & 0xFFF) | 0x2000;
				}
				else if ((size_t)(e - d) < 2) {
					m = 1;
					break; /* too small output buffer */
				}
				else {
					d += 2;
					c |= 0x3000;
				}
				d[-2] = (utf8_char_t)(c >> 6);
				c = (c & 0x3F) | 0x80;
			}
			else
				d++;
			d[-1] = (utf8_char_t)c;
			if (!c) {
				sz = (size_t)(d - *b);
				*w = s; /* (*w) points beyond successfully converted 0 */
				*b = d;
				return sz; /* ok, >0 */
			}
		} while (d != e);
		/* too small output buffer */
		sz = (size_t)(d - *b);
		*b = d;
	}
	/* NOTE: assume total size in bytes of input utf16 string, including terminating 0,
	  may be stored in a variable of size_t type without loss, so for each utf16_char_t may
	  safely increment 'm' at least by 2 without integer overflow */
	/* NOTE: also, 'm' will not overflow, if it's zero or have one of next values:
	  1 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
	  2 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
	  3 - at least two utf16_char_t's (4 bytes) were checked, no overflow of 'm' is possible */
	{
		const utf16_char_t *const t = s - (m != 0) - (3 == m); /* points beyond the last converted utf16_char_t */
		if (3 == m)
			m = 2;
		for (;;) {
			unsigned c = *s++;
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						c = *s;
						if (0xDC00 != (c & 0xFC00)) {
							*w = s - 1; /* (**w) != 0 */
							return 0; /* bad utf16 surrogate pair: no lower surrogate */
						}
						s++;
					}
					else if (0xDC00 == (c & 0xFC00)) {
						*w = s - 1; /* (**w) != 0 */
						return 0; /* bad utf16 surrogate pair: no high surrogate */
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
		return sz; /* ok, >0 */
	}
too_long:
	*w = s; /* points after the 0-terminator */
	return 0; /* integer overflow, input string is too long */
}

A_Use_decl_annotations
size_t utf16_to_utf8(const utf16_char_t **const w, utf8_char_t **const b, size_t sz, const size_t n)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const utf16_char_t *A_Restrict s = *w;
		const utf16_char_t *const se = s + n;
		if (sz) {
			utf8_char_t *A_Restrict d = *b;
			const utf8_char_t *const e = d + sz;
			do {
				unsigned c = *s++;
				if (c >= 0x80) {
					if (c >= 0x800) {
						if (0xD800 == (c & 0xFC00)) {
							const unsigned r = (s != se) ? *s : 0u;
							if (0xDC00 != (r & 0xFC00)) {
								*w = s - 1; /* (*w) < se */
								*b = d;
								return 0; /* bad utf16 surrogate pair: no lower surrogate */
							}
							s++;
							if ((size_t)(e - d) < 4) {
								m = 3;
								break; /* too small output buffer */
							}
							/* c is in range: 0xD800..0xDBFF, i.e.: 110110xxyyyyyyyy */
							/* r is in range: 0xDC00..0xDFFF, i.e.: 110111aabbbbbbbb */
							c = ((c << 10) ^ r ^ 0xA0DC00) + 0x10000;
							d += 4;
							d[-4] = (utf8_char_t)(c >> 18);
							c = (c & 0x3FFFF) | 0x80000;
						}
						else if (0xDC00 == (c & 0xFC00)) {
							*w = s - 1; /* (*w) < se */
							*b = d;
							return 0; /* bad utf16 surrogate pair: no high surrogate */
						}
						else if ((size_t)(e - d) < 3) {
							m = 2;
							break; /* too small output buffer */
						}
						else {
							d += 3;
							c |= 0xE0000;
						}
						d[-3] = (utf8_char_t)(c >> 12);
						c = (c & 0xFFF) | 0x2000;
					}
					else if ((size_t)(e - d) < 2) {
						m = 1;
						break; /* too small output buffer */
					}
					else {
						d += 2;
						c |= 0x3000;
					}
					d[-2] = (utf8_char_t)(c >> 6);
					c = (c & 0x3F) | 0x80;
				}
				else
					d++;
				d[-1] = (utf8_char_t)c;
				if (se == s) {
					sz = (size_t)(d - *b);
					*w = s; /* (*w) == se */
					*b = d;
					return sz; /* ok, >0 */
				}
			} while (d != e);
			/* too small output buffer */
			sz = (size_t)(d - *b);
			*b = d;
		}
		/* NOTE: assume total size in bytes of input utf16 string,
		  may be stored in a variable of size_t type without loss, so for each utf16_char_t may
		  safely increment 'm' at least by 2 without integer overflow */
		/* NOTE: also, 'm' will not overflow, if it's zero or have one of next values:
		  1 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
		  2 - at least one utf16_char_t (2 bytes) was checked, no overflow of 'm' is possible,
		  3 - at least two utf16_char_t's (4 bytes) were checked, no overflow of 'm' is possible */
		{
			const utf16_char_t *const t = s - (m != 0) - (3 == m); /* points beyond the last converted utf16_char_t */
			if (3 == m)
				m = 2;
			do {
				unsigned c = *s++;
				if (c >= 0x80) {
					if (c >= 0x800) {
						if (0xD800 == (c & 0xFC00)) {
							c = (s != se) ? *s : 0u;
							if (0xDC00 != (c & 0xFC00)) {
								*w = s - 1; /* (*w) < se */
								return 0; /* bad utf16 surrogate pair: no lower surrogate */
							}
							s++;
						}
						else if (0xDC00 == (c & 0xFC00)) {
							*w = s - 1; /* (*w) < se */
							return 0; /* bad utf16 surrogate pair: no high surrogate */
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
			return sz; /* ok, >0 */
		}
too_long:
		*w = s; /* (*w) == se */
		return 0; /* integer overflow, input string is too long */
	}
	return 0; /* n is zero */
}

A_Use_decl_annotations
const utf16_char_t *utf16_to_utf8_z_unsafe(const utf16_char_t *w, utf8_char_t buf[])
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = *w++;
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (0xD800 == (c & 0xFC00)) {
					c = ((c << 10) ^ (unsigned)*w++ ^ 0xA0DC00) + 0x10000;
					b += 4;
					b[-4] = (utf8_char_t)(c >> 18);
					c = (c & 0x3FFFF) | 0x80000;
				}
				else {
					b += 3;
					c |= 0xE0000;
				}
				b[-3] = (utf8_char_t)(c >> 12);
				c = (c & 0xFFF) | 0x2000;
			}
			else {
				b += 2;
				c |= 0x3000;
			}
			b[-2] = (utf8_char_t)(c >> 6);
			c = (c & 0x3F) | 0x80;
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

A_Use_decl_annotations
void utf16_to_utf8_unsafe(const utf16_char_t *w, utf8_char_t buf[], const size_t n)
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf16_char_t *const we = w + n;
	do {
		unsigned c = *w++;
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (0xD800 == (c & 0xFC00)) {
					c = ((c << 10) ^ (unsigned)*w++ ^ 0xA0DC00) + 0x10000;
					b += 4;
					b[-4] = (utf8_char_t)(c >> 18);
					c = (c & 0x3FFFF) | 0x80000;
				}
				else {
					b += 3;
					c |= 0xE0000;
				}
				b[-3] = (utf8_char_t)(c >> 12);
				c = (c & 0xFFF) | 0x2000;
			}
			else {
				b += 2;
				c |= 0x3000;
			}
			b[-2] = (utf8_char_t)(c >> 6);
			c = (c & 0x3F) | 0x80;
		}
		else
			b++;
		b[-1] = (utf8_char_t)c;
	} while (w != we);
}
