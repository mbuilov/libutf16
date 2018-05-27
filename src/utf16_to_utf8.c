/**********************************************************************************
* UTF-16 -> UTF-8 characters conversion
* Copyright (C) 2018 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8.c */

#include <stddef.h>
#include <stdint.h>
#include "sal_defs.h"
#include "libutf16/utf16_to_utf8.h"

A_Use_decl_annotations
size_t utf16_to_utf8_z(const utf16_char_t **const w, utf8_char_t **const b, size_t sz)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf16_char_t *A_Restrict s = *w;
	if (sz) {
		utf8_char_t *A_Restrict d = *b;
		utf8_char_t *const e = d + sz;
		do {
			unsigned c = *s++;
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						unsigned r = *s;
						if (0xDC00 != (r & 0xFC00)) {
							*w = s - 1; /* (*w) != 0 */
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
						*d++ = (utf8_char_t)(c >> 18);
						*d++ = (utf8_char_t)(((c >> 12) | 0x80) & 0xBF);
					}
					else if (0xDC00 == (c & 0xFC00)) {
						*w = s - 1; /* (*w) != 0 */
						*b = d;
						return 0; /* bad utf16 surrogate pair: no high surrogate */
					}
					else if ((size_t)(e - d) < 3) {
						m = 2;
						break; /* too small output buffer */
					}
					else
						*d++ = (utf8_char_t)((c >> 12) | 0xE0);
					*d++ = (utf8_char_t)(((c >> 6) | 0x80) & 0xBF);
				}
				else if ((size_t)(e - d) < 2) {
					m = 1;
					break; /* too small output buffer */
				}
				else
					*d++ = (utf8_char_t)((c >> 6) | 0xC0);
				*d++ = (utf8_char_t)((c | 0x80) & 0xBF);
			}
			else {
				*d++ = (utf8_char_t)c;
				if (!c) {
					sz = (size_t)(d - *b);
					*w = s; /* (*w) points beyond successfully converted 0 */
					*b = d;
					return sz; /* ok, >0 */
				}
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
		const utf16_char_t *const t = s - (m != 0) - (m == 3); /* points beyond the last converted utf16_char_t */
		if (m == 3)
			m = 2;
		for (;;) {
			unsigned c = *s++;
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						c = *s;
						if (0xDC00 != (c & 0xFC00)) {
							*w = s - 1; /* (*w) != 0 */
							return 0; /* bad utf16 surrogate pair: no lower surrogate */
						}
						s++;
					}
					else if (0xDC00 == (c & 0xFC00)) {
						*w = s - 1; /* (*w) != 0 */
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
			utf8_char_t *const e = d + sz;
			do {
				unsigned c = *s++;
				if (c >= 0x80) {
					if (c >= 0x800) {
						if (0xD800 == (c & 0xFC00)) {
							unsigned r = (s != se) ? *s : 0u;
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
							*d++ = (utf8_char_t)(c >> 18);
							*d++ = (utf8_char_t)(((c >> 12) | 0x80) & 0xBF);
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
						else
							*d++ = (utf8_char_t)((c >> 12) | 0xE0);
						*d++ = (utf8_char_t)(((c >> 6) | 0x80) & 0xBF);
					}
					else if ((size_t)(e - d) < 2) {
						m = 1;
						break; /* too small output buffer */
					}
					else
						*d++ = (utf8_char_t)((c >> 6) | 0xC0);
					*d++ = (utf8_char_t)((c | 0x80) & 0xBF);
				}
				else
					*d++ = (utf8_char_t)c;
				if (s == se) {
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
			const utf16_char_t *const t = s - (m != 0) - (m == 3); /* points beyond the last converted utf16_char_t */
			if (m == 3)
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
size_t utf16_to_utf8_z_unsafe_out(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf)
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = *w++;
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (0xD800 == (c & 0xFC00)) {
					unsigned r = *w;
					if (0xDC00 != (r & 0xFC00))
						return 0; /* bad utf16 surrogate pair: no lower surrogate */
					w++;
					c = ((c << 10) ^ r ^ 0xA0DC00) + 0x10000;
					*b++ = (utf8_char_t)(c >> 18);
					*b++ = (utf8_char_t)(((c >> 12) | 0x80) & 0xBF);
				}
				else if (0xDC00 == (c & 0xFC00))
					return 0; /* bad utf16 surrogate pair: no high surrogate */
				else
					*b++ = (utf8_char_t)((c >> 12) | 0xE0);
				*b++ = (utf8_char_t)(((c >> 6) | 0x80) & 0xBF);
			}
			else
				*b++ = (utf8_char_t)((c >> 6) | 0xC0);
			*b++ = (utf8_char_t)((c | 0x80) & 0xBF);
		}
		else {
			/* make analyzer happy: this equivalent to:
			   *b++ = (utf8_char_t)c; */
			buf[b++ - buf] = (utf8_char_t)c;
			if (!c)
				return (size_t)(b - buf); /* ok, >0 */
		}
	}
}

A_Use_decl_annotations
size_t utf16_to_utf8_z_unsafe(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf)
{
	/* unsigned integer type must be at least of 32 bits */
	utf8_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned c = *w++;
		if (c >= 0x80) {
			if (c >= 0x800) {
				if (0xD800 == (c & 0xFC00)) {
					unsigned r = *w++;
					c = ((c << 10) ^ r ^ 0xA0DC00) + 0x10000;
					*b++ = (utf8_char_t)(c >> 18);
					*b++ = (utf8_char_t)(((c >> 12) | 0x80) & 0xBF);
				}
				else
					*b++ = (utf8_char_t)((c >> 12) | 0xE0);
				*b++ = (utf8_char_t)(((c >> 6) | 0x80) & 0xBF);
			}
			else
				*b++ = (utf8_char_t)((c >> 6) | 0xC0);
			*b++ = (utf8_char_t)((c | 0x80) & 0xBF);
		}
		else {
			/* make analyzer happy: this equivalent to:
			   *b++ = (utf8_char_t)c; */
			buf[b++ - buf] = (utf8_char_t)c;
			if (!c)
				return (size_t)(b - buf); /* ok, >0 */
		}
	}
}

A_Use_decl_annotations
size_t utf16_to_utf8_unsafe_out(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf, const size_t n)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		utf8_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const utf16_char_t *const we = w + n;
		do {
			unsigned c = *w++;
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						unsigned r = (w != we) ? *w : 0u;
						if (0xDC00 != (r & 0xFC00))
							return 0; /* bad utf16 surrogate pair: no lower surrogate */
						w++;
						c = ((c << 10) ^ r ^ 0xA0DC00) + 0x10000;
						*b++ = (utf8_char_t)(c >> 18);
						*b++ = (utf8_char_t)(((c >> 12) | 0x80) & 0xBF);
					}
					else if (0xDC00 == (c & 0xFC00))
						return 0; /* bad utf16 surrogate pair: no high surrogate */
					else
						*b++ = (utf8_char_t)((c >> 12) | 0xE0);
					*b++ = (utf8_char_t)(((c >> 6) | 0x80) & 0xBF);
				}
				else
					*b++ = (utf8_char_t)((c >> 6) | 0xC0);
				*b++ = (utf8_char_t)((c | 0x80) & 0xBF);
			}
			else
				*b++ = (utf8_char_t)c;
		} while (w != we);
		return (size_t)(b - buf); /* ok, >0 */
	}
	return 0; /* n is zero */
}

A_Use_decl_annotations
size_t utf16_to_utf8_unsafe(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf, const size_t n)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		utf8_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const utf16_char_t *const we = w + n;
		do {
			unsigned c = *w++;
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						unsigned r = *w++;
						c = ((c << 10) ^ r ^ 0xA0DC00) + 0x10000;
						*b++ = (utf8_char_t)(c >> 18);
						*b++ = (utf8_char_t)(((c >> 12) | 0x80) & 0xBF);
					}
					else
						*b++ = (utf8_char_t)((c >> 12) | 0xE0);
					*b++ = (utf8_char_t)(((c >> 6) | 0x80) & 0xBF);
				}
				else
					*b++ = (utf8_char_t)((c >> 6) | 0xC0);
				*b++ = (utf8_char_t)((c | 0x80) & 0xBF);
			}
			else
				*b++ = (utf8_char_t)c;
		} while (w != we);
		return (size_t)(b - buf); /* ok, >0 */
	}
	return 0; /* n is zero */
}
