/**********************************************************************************
* UTF-8 -> UTF-16 characters conversion
* Copyright (C) 2018-2019 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_to_utf16.c */

#include <stddef.h> /* for size_t */
#ifndef _MSC_VER
#include <stdint.h> /* for uint16_t */
#endif
#include "libutf16/utf8_to_utf16.h"

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Use_decl_annotations
#define A_Restrict
#endif

A_Use_decl_annotations
size_t utf8_to_utf16_z(const utf8_char_t **const q, utf16_char_t **const b, size_t sz)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf8_char_t *A_Restrict s = *q;
	const utf8_char_t *t; /* points beyond the last converted utf8_char_t */
	if (!sz)
		t = s;
	else {
		utf16_char_t *A_Restrict d = *b;
		utf16_char_t *const e = d + sz;
		do {
			unsigned a = s[0];
			if (a >= 0x80) {
				unsigned r;
				if (a >= 0xE0) {
					if (a >= 0xF0) {
						if (a >= 0xF8)
							goto bad_utf8; /* expecting max 4 utf8 bytes for a unicode code point */
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = (a << 6) ^ r;
						r = s[2];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = (a << 6) ^ r;
						r = s[3];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = ((a << 6) ^ r ^ 0xA82080) - 0x10000;
						/* a        = 11011aaaaabbbbbbbbcccccccc before substracting 0x10000,
						 a must match 110110xxxxxxxxxxxxxxxxxxxx after  substracting 0x10000 */
						if (0x3600000 != (a & 0x3F00000))
							goto bad_utf8; /* bad utf8 character */
						s += 4;
						if ((size_t)(e - d) < 2) {
							t = s - 4;
							m = 2; /* = (4 utf8_char_t's - 2 utf16_char_t's) */
							goto small_buf; /* too small output buffer */
						}
						*d++ = (utf16_char_t)(a >> 10); /* 110110aaaabbbbbb */
						a = (a & 0x3FF) + 0xDC00;       /* 110111bbcccccccc */
					}
					else {
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = (a << 6) ^ r;
						r = s[2];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = (a << 6) ^ r ^ 0xE2080;
						/* must not begin or end a surrogate pair */
						if (!a ||
						    0xD800 == (a & 0xFC00) ||
						    0xDC00 == (a & 0xFC00))
						{
							goto bad_utf8; /* overlong/bad utf8 character */
						}
						s += 3;
					}
				}
				else if (a >= 0xC0) {
					r = s[1];
					if (0x80 != (r & 0xC0))
						goto bad_utf8; /* incomplete utf8 character */
					a = (a << 6) ^ r ^ 0x3080;
					if (!a)
						goto bad_utf8; /* overlong utf8 character */
					s += 2;
				}
				else
					goto bad_utf8; /* not expecting 10xxxxxx */
			}
			else
				s++;
			*d++ = (utf16_char_t)a;
			if (!a) {
				m = (size_t)(d - *b);
bad_utf8:
				*q = s; /* (*q) != 0 if bad_utf8, else (*q) points beyond successfully converted 0 */
				*b = d;
				return m; /* 0 if bad_utf8, else >0 */
			}
		} while (d != e);
		/* too small output buffer */
		t = s;
small_buf:
		sz = (size_t)(d - *b);
		*b = d;
	}
	for (;;) {
		unsigned a = s[0];
		if (a >= 0x80) {
			unsigned r;
			if (a >= 0xE0) {
				if (a >= 0xF0) {
					if (a >= 0xF8)
						goto bad_utf8_s; /* expecting max 4 utf8 bytes for a unicode code point */
					r = s[1];
					if (0x80 != (r & 0xC0))
						goto bad_utf8_s; /* incomplete utf8 character */
					a = (a << 6) ^ r;
					r = s[2];
					if (0x80 != (r & 0xC0))
						goto bad_utf8_s; /* incomplete utf8 character */
					a = (a << 6) ^ r;
					r = s[3];
					if (0x80 != (r & 0xC0))
						goto bad_utf8_s; /* incomplete utf8 character */
					a = ((a << 6) ^ r ^ 0xA82080) - 0x10000;
					/* a        = 11011aaaaabbbbbbbbcccccccc before substracting 0x10000,
					 a must match 110110xxxxxxxxxxxxxxxxxxxx after  substracting 0x10000 */
					if (0x3600000 != (a & 0x3F00000))
						goto bad_utf8_s; /* bad utf8 character */
					s += 4;
					m += 2; /* + (4 utf8_char_t's - 2 utf16_char_t's) */
				}
				else {
					r = s[1];
					if (0x80 != (r & 0xC0))
						goto bad_utf8_s; /* incomplete utf8 character */
					a = (a << 6) ^ r;
					r = s[2];
					if (0x80 != (r & 0xC0))
						 goto bad_utf8_s; /* incomplete utf8 character */
					a = (a << 6) ^ r ^ 0xE2080;
					/* must not begin or end a surrogate pair */
					if (!a ||
						0xD800 == (a & 0xFC00) ||
						0xDC00 == (a & 0xFC00))
					{
						goto bad_utf8_s; /* overlong/bad utf8 character */
					}
					s += 3;
					m += 2; /* + (3 utf8_char_t's - 1 utf16_char_t) */
				}
			}
			else if (a >= 0xC0) {
				r = s[1];
				if (0x80 != (r & 0xC0))
					goto bad_utf8_s; /* incomplete utf8 character */
				a = (a << 6) ^ r ^ 0x3080;
				if (!a)
					goto bad_utf8_s; /* overlong utf8 character */
				s += 2;
				m++; /* + (2 utf8_char_t's - 1 utf16_char_t) */
			}
			else {
				/* not expecting 10xxxxxx */
bad_utf8_s:
				*q = s; /* (*q) != 0 */
				return 0; /* incomplete utf8 character */
			}
		}
		else {
			s++;
			if (!a) {
				/* no overflow of size_t is possible when counting the length of
				  resulting utf16 string (in utf16_char_t's), assuming that the
				  length of utf8 string may be stored in a variable of size_t without loss:
				 4 utf8_char_t's -> 2 utf16_char_t's (surrogate pair),
				 3 utf8_char_t's -> 1 utf16_char_t,
				 2 utf8_char_t's -> 1 utf16_char_t,
				 1 utf8_char_t  -> 1 utf16_char_t*/
				/* append a number of utf16_char_t's in utf8 string started from 't' */
				sz += (size_t)(s - t) - m;
				*q = t; /* points after the last successfully converted non-0 utf8_char_t */
				return sz; /* ok, >0 */
			}
		}
	}
}

A_Use_decl_annotations
size_t utf8_to_utf16(const utf8_char_t **const q, utf16_char_t **const b, size_t sz, const size_t n)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const utf8_char_t *A_Restrict s = *q;
		const utf8_char_t *const se = s + n;
		const utf8_char_t *t; /* points beyond the last converted utf8_char_t */
		if (!sz)
			t = s;
		else {
			utf16_char_t *A_Restrict d = *b;
			utf16_char_t *const e = d + sz;
			do {
				unsigned a = s[0];
				if (a >= 0x80) {
					unsigned r;
					if (a >= 0xE0) {
						if (a >= 0xF0) {
							if (a >= 0xF8)
								goto bad_utf8; /* expecting max 4 utf8 bytes for a unicode code point */
							if ((size_t)(se - s) < 4)
								goto bad_utf8; /* incomplete utf8 character */
							r = s[1];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) ^ r;
							r = s[2];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) ^ r;
							r = s[3];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = ((a << 6) ^ r ^ 0xA82080) - 0x10000;
							/* a        = 11011aaaaabbbbbbbbcccccccc before substracting 0x10000,
							 a must match 110110xxxxxxxxxxxxxxxxxxxx after  substracting 0x10000 */
							if (0x3600000 != (a & 0x3F00000))
								goto bad_utf8; /* bad utf8 character */
							s += 4;
							if ((size_t)(e - d) < 2) {
								t = s - 4;
								m = 2; /* = (4 utf8_char_t's - 2 utf16_char_t's) */
								goto small_buf; /* too small output buffer */
							}
							*d++ = (utf16_char_t)(a >> 10); /* 110110aaaabbbbbb */
							a = (a & 0x3FF) + 0xDC00;       /* 110111bbcccccccc */
						}
						else {
							if ((size_t)(se - s) < 3)
								goto bad_utf8; /* incomplete utf8 character */
							r = s[1];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) ^ r;
							r = s[2];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) ^ r ^ 0xE2080;
							/* must not begin or end a surrogate pair */
							if (!a ||
								0xD800 == (a & 0xFC00) ||
								0xDC00 == (a & 0xFC00))
							{
								goto bad_utf8; /* overlong/bad utf8 character */
							}
							s += 3;
						}
					}
					else if (a >= 0xC0) {
						if ((size_t)(se - s) < 2)
							goto bad_utf8; /* incomplete utf8 character */
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = (a << 6) ^ r ^ 0x3080;
						if (!a)
							goto bad_utf8; /* overlong utf8 character */
						s += 2;
					}
					else
						goto bad_utf8; /* not expecting 10xxxxxx */
				}
				else
					s++;
				*d++ = (utf16_char_t)a;
				if (se == s) {
					m = (size_t)(d - *b);
bad_utf8:
					*q = s; /* (*q) < se if bad_utf8, else (*q) == se */
					*b = d;
					return m; /* 0 if bad_utf8, else >0 */
				}
			} while (d != e);
			/* too small output buffer */
			t = s;
small_buf:
			sz = (size_t)(d - *b);
			*b = d;
		}
		do {
			unsigned a = s[0];
			if (a >= 0x80) {
				unsigned r;
				if (a >= 0xE0) {
					if (a >= 0xF0) {
						if (a >= 0xF8)
							goto bad_utf8_s; /* expecting max 4 utf8 bytes for a unicode code point */
						if ((size_t)(se - s) < 4)
							goto bad_utf8_s; /* incomplete utf8 character */
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						a = (a << 6) ^ r;
						r = s[2];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						a = (a << 6) ^ r;
						r = s[3];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						a = ((a << 6) ^ r ^ 0xA82080) - 0x10000;
						/* a        = 11011aaaaabbbbbbbbcccccccc before substracting 0x10000,
						 a must match 110110xxxxxxxxxxxxxxxxxxxx after  substracting 0x10000 */
						if (0x3600000 != (a & 0x3F00000))
							goto bad_utf8_s; /* bad utf8 character */
						s += 4;
						m += 2; /* + (4 utf8_char_t's - 2 utf16_char_t's) */
					}
					else {
						if ((size_t)(se - s) < 3)
							goto bad_utf8_s; /* incomplete utf8 character */
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						a = (a << 6) ^ r;
						r = s[2];
						if (0x80 != (r & 0xC0))
							 goto bad_utf8_s; /* incomplete utf8 character */
						a = (a << 6) ^ r ^ 0xE2080;
						/* must not begin or end a surrogate pair */
						if (!a ||
							0xD800 == (a & 0xFC00) ||
							0xDC00 == (a & 0xFC00))
						{
							goto bad_utf8_s; /* overlong/bad utf8 character */
						}
						s += 3;
						m += 2; /* + (3 utf8_char_t's - 1 utf16_char_t) */
					}
				}
				else if (a >= 0xC0) {
					if ((size_t)(se - s) < 2)
						goto bad_utf8_s; /* incomplete utf8 character */
					r = s[1];
					if (0x80 != (r & 0xC0))
						goto bad_utf8_s; /* incomplete utf8 character */
					a = (a << 6) ^ r ^ 0x3080;
					if (!a)
						goto bad_utf8_s; /* overlong utf8 character */
					s += 2;
					m++; /* + (2 utf8_char_t's - 1 utf16_char_t) */
				}
				else {
					/* not expecting 10xxxxxx */
bad_utf8_s:
					*q = s; /* (*q) < se */
					return 0; /* incomplete utf8 character */
				}
			}
			else
				s++;
		} while (s != se);
		/* no overflow of size_t is possible when counting the length of
		  resulting utf16 string (in utf16_char_t's), assuming that the
		  length of utf8 string may be stored in a variable of size_t without loss:
		 4 utf8_char_t's -> 2 utf16_char_t's (surrogate pair),
		 3 utf8_char_t's -> 1 utf16_char_t,
		 2 utf8_char_t's -> 1 utf16_char_t,
		 1 utf8_char_t  -> 1 utf16_char_t*/
		/* append a number of utf16_char_t's in utf8 string started from 't' */
		sz += (size_t)(s - t) - m;
		*q = t; /* points after the last successfully converted utf8_char_t, (*q) < se */
		return sz; /* ok, >0 */
	}
	return 0; /* n is zero */
}

A_Use_decl_annotations
const utf8_char_t *utf8_to_utf16_z_remaining(const utf8_char_t *q, utf16_char_t buf[])
{
	/* unsigned integer type must be at least of 32 bits */
	utf16_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned a = q[0];
		if (a >= 0x80) {
			unsigned r = q[1];
			a = (a << 6) ^ r;
			if (a >= (0xE0 << 6)) {
				r = q[2];
				a = (a << 6) ^ r;
				if (a >= (0xF0 << 12)) {
					r = q[3];
					a = ((a << 6) ^ r ^ 0xA82080) - 0x10000;
					/* a        = 11011aaaaabbbbbbbbcccccccc before substracting 0x10000,
					 a must match 110110xxxxxxxxxxxxxxxxxxxx after  substracting 0x10000 */
					*b++ = (utf16_char_t)(a >> 10); /* 110110aaaabbbbbb */
					a = (a & 0x3FF) + 0xDC00;       /* 110111bbcccccccc */
					q += 4;
				}
				else {
					a ^= 0xE2080;
					q += 3;
				}
			}
			else {
				a ^= 0x3080;
				q += 2;
			}
		}
		else
			q++;
		/* make analyzer happy: this equivalent to:
		   *b++ = (utf16_char_t)a; */
		buf[b++ - buf] = (utf16_char_t)a;
		if (!a)
			return q; /* ok, q[-1] == 0 */
	}
}

A_Use_decl_annotations
void utf8_to_utf16_remaining(const utf8_char_t *q, utf16_char_t buf[], const size_t n)
{
	/* unsigned integer type must be at least of 32 bits */
	utf16_char_t *A_Restrict b = buf + 0*sizeof(int(*)[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf8_char_t *const qe = q + n;
	do {
		unsigned a = q[0];
		if (a >= 0x80) {
			unsigned r;
#ifdef _MSC_VER
			__assume(n > 1); /* assume input string is valid */
#endif
			r = q[1];
			a = (a << 6) ^ r;
			if (a >= (0xE0 << 6)) {
#ifdef _MSC_VER
				__assume(n > 2); /* assume input string is valid */
#endif
				r = q[2];
				a = (a << 6) ^ r;
				if (a >= (0xF0 << 12)) {
#ifdef _MSC_VER
					__assume(n > 3); /* assume input string is valid */
#endif
					r = q[3];
					a = ((a << 6) ^ r ^ 0xA82080) - 0x10000;
					/* a        = 11011aaaaabbbbbbbbcccccccc before substracting 0x10000,
					 a must match 110110xxxxxxxxxxxxxxxxxxxx after  substracting 0x10000 */
					*b++ = (utf16_char_t)(a >> 10); /* 110110aaaabbbbbb */
					a = (a & 0x3FF) + 0xDC00;       /* 110111bbcccccccc */
					q += 4;
				}
				else {
					a ^= 0xE2080;
					q += 3;
				}
			}
			else {
				a ^= 0x3080;
				q += 2;
			}
		}
		else
			q++;
		*b++ = (utf16_char_t)a;
	} while (q != qe);
}
