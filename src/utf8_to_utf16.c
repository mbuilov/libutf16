/**********************************************************************************
* UTF-8 -> UTF-16 characters conversion
* Copyright (C) 2018-2022 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_to_utf16.c */

#include <stddef.h> /* for size_t */

#ifndef _MSC_VER
#include <stdint.h> /* for uint16_t */
#endif

#ifdef _MSC_VER
#include <stdlib.h> /* for _byteswap_ushort()/_byteswap_ulong() */
#endif

#include <memory.h> /* for memcpy() */

#include "libutf16/utf8_to_utf16.h"
#include "libutf16/utf16_swap.h"

#include "utf16_internal.h"

#ifdef _MSC_VER
#pragma warning(disable:5045) /* Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified */
#endif

#ifdef UTF_PUT_UNALIGNED
#define UTF16_CHAR_T utf16_char_unaligned_t
#else
#define UTF16_CHAR_T utf16_char_t
#endif

#define UTF_FORM_NAME2(tu, tx, suffix)  utf8_to_utf16##tu##tx##suffix
#define UTF_FORM_NAME1(tu, tx, suffix)  UTF_FORM_NAME2(tu, tx, suffix)
#define UTF_FORM_NAME(suffix)           UTF_FORM_NAME1(UTF_PUT_U, UTF16_X, suffix)

/*
 utf8_to_utf16_z_
 utf8_to_utf16x_z_
 utf8_to_utf16u_z_
 utf8_to_utf16ux_z_
*/
size_t UTF_FORM_NAME(_z_)(
	const utf8_char_t **const LIBUTF16_RESTRICT q,
	UTF16_CHAR_T **const LIBUTF16_RESTRICT b,
	size_t sz, int determ_req_size)
{
	/* unsigned integer type must be at least of 32 bits */
	size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf8_char_t *LIBUTF16_RESTRICT s = *q;
	const utf8_char_t *t; /* points beyond the last converted utf8_char_t */
	if (!sz) {
		if (!determ_req_size)
			return 1;
		if (2 == determ_req_size)
			determ_req_size = 0;
		t = s;
	}
	else {
		UTF16_CHAR_T *LIBUTF16_RESTRICT d = *b;
		const UTF16_CHAR_T *const e = (const UTF16_CHAR_T*)d + sz;
		do {
			unsigned a = s[0];
			if (a >= 0x80) {
				unsigned r = s[1];
				if (0x80 != (r & 0xC0))
					goto bad_utf8; /* incomplete utf8 character */
				a = (a << 6) + r;
				if (a >= (0xE0 << 6) + 0x80) {
					r = s[2];
					if (0x80 != (r & 0xC0))
						goto bad_utf8; /* incomplete utf8 character */
					a = (a << 6) + r;
					if (a >= (((0xF0 << 6) + 0x80) << 6) + 0x80) {
						if (a > (((0xF4 << 6) + 0x80) << 6) + 0x80)
							goto bad_utf8; /* unicode code point must be <= 0x10FFFF */
						if (!((0x3C90 << 6) + 0x80 <= a && a <= (0x3D8F << 6) + 0xBF))
							goto bad_utf8; /* overlong utf8 character/out of range */
						r = s[3];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = (a << 6) + r - 0x682080 - 0x10000;
						s += 4;
						if ((const UTF16_CHAR_T*)d + 1 == e) {
							t = s - 4;
							m = 2; /* = (4 utf8_char_t's - 2 utf16_char_t's) */
							goto small_buf; /* too small output buffer */
						}
						UTF16_PUT(d++, (utf16_char_t)(a >> 10)); /* 110110aaaabbbbbb */
						a = (a & 0x3FF) + 0xDC00;                /* 110111bbcccccccc */
					}
					else {
						if (a < (0x38A0 << 6) + 0x80 || ((0x3BE0 << 6) + 0x80 <= a && a <= (0x3BFF << 6) + 0xBF))
							goto bad_utf8; /* overlong utf8 character/surrogate */
						a -= 0xE2080;
						s += 3;
					}
				}
				else if (a >= (0xC2 << 6) + 0x80) {
					a -= 0x3080;
					s += 2;
				}
				else
					goto bad_utf8; /* not expecting 10xxxxxx or overlong utf8 character: 1100000x */
			}
			else
				s++;
			UTF16_PUT(d++, (utf16_char_t)a);
			if (!a) {
				m = (size_t)(d - *b);
bad_utf8:
				*q = s; /* (**q) != 0 if bad_utf8, else (*q) points beyond successfully converted 0 */
				*b = d;
				return m; /* 0 if bad_utf8, else >0 and <= dst buffer size */
			}
		} while ((const UTF16_CHAR_T*)d != e);
		/* too small output buffer */
		t = s;
small_buf:
		sz = (size_t)(d - *b);
		*b = d;
		if (!determ_req_size) {
			*q = t; /* points after the last successfully converted non-0 utf8_char_t */
			return sz + 1 + m; /* ok, >0, but > dst buffer size */
		}
	}
	for (;;) {
		unsigned a = s[0];
		if (a >= 0x80) {
			unsigned r = s[1];
			if (0x80 != (r & 0xC0))
				goto bad_utf8_s; /* incomplete utf8 character */
			if (a >= 0xE0) {
				unsigned x = s[2];
				if (0x80 != (x & 0xC0))
					goto bad_utf8_s; /* incomplete utf8 character */
				a = (a << 6) + r;
				if (a >= (0xF0 << 6) + 0x80) {
					if (a > (0xF4 << 6) + 0x80)
						goto bad_utf8_s; /* unicode code point must be <= 0x10FFFF */
					if (!(0x3C90 <= a && a <= 0x3D8F))
						goto bad_utf8_s; /* overlong utf8 character/out of range */
					r = s[3];
					if (0x80 != (r & 0xC0))
						goto bad_utf8_s; /* incomplete utf8 character */
					s += 4;
					m += 2; /* + (4 utf8_char_t's - 2 utf16_char_t's) */
				}
				else {
					if (a < 0x38A0 || (0x3BE0 <= a && a <= 0x3BFF))
						goto bad_utf8_s; /* overlong utf8 character/surrogate */
					s += 3;
					m += 2; /* + (3 utf8_char_t's - 1 utf16_char_t) */
				}
			}
			else if (a >= 0xC2) {
				s += 2;
				m++; /* + (2 utf8_char_t's - 1 utf16_char_t) */
			}
			else {
				/* not expecting 10xxxxxx or overlong utf8 character: 1100000x */
bad_utf8_s:
				*q = s; /* (**q) != 0 */
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
				 1 utf8_char_t   -> 1 utf16_char_t. */
				/* append a number of utf16_char_t's in utf8 string started from 't' */
				sz += (size_t)(s - t) - m;
				*q = !determ_req_size ? s : t/* points after the last successfully converted non-0 utf8_char_t */;
				return sz; /* ok, >0, but > dst buffer size */
			}
		}
	}
}

/*
 utf8_to_utf16_
 utf8_to_utf16u_
 utf8_to_utf16x_
 utf8_to_utf16ux_
*/
size_t UTF_FORM_NAME(_)(
	const utf8_char_t **const LIBUTF16_RESTRICT q,
	UTF16_CHAR_T **const LIBUTF16_RESTRICT b,
	size_t sz, const size_t n, const int determ_req_size)
{
	if (n) {
		/* unsigned integer type must be at least of 32 bits */
		size_t m = 0 + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
		const utf8_char_t *LIBUTF16_RESTRICT s = *q;
		const utf8_char_t *const se = s + n;
		const utf8_char_t *t; /* points beyond the last converted utf8_char_t */
		if (!sz) {
			if (!determ_req_size)
				return 1;
			t = s;
		}
		else {
			UTF16_CHAR_T *LIBUTF16_RESTRICT d = *b;
			const UTF16_CHAR_T *const e = (const UTF16_CHAR_T*)d + sz;
			do {
				unsigned a = s[0];
				if (a >= 0x80) {
					unsigned r;
					if (a >= 0xE0) {
						if (a >= 0xF0) {
							if (a > 0xF4)
								goto bad_utf8; /* unicode code point must be <= 0x10FFFF */
							if ((size_t)(se - s) < 4)
								goto bad_utf8; /* incomplete utf8 character */
							r = s[1];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) + r;
							if (!(0x3C90 <= a && a <= 0x3D8F))
								goto bad_utf8; /* overlong utf8 character/out of range */
							r = s[2];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) + r;
							r = s[3];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) + r - 0x682080 - 0x10000;
							s += 4;
							if ((const UTF16_CHAR_T*)d + 1 == e) {
								t = s - 4;
								m = 2; /* = (4 utf8_char_t's - 2 utf16_char_t's) */
								goto small_buf; /* too small output buffer */
							}
							UTF16_PUT(d++, (utf16_char_t)(a >> 10)); /* 110110aaaabbbbbb */
							a = (a & 0x3FF) + 0xDC00;                /* 110111bbcccccccc */
						}
						else {
							if ((size_t)(se - s) < 3)
								goto bad_utf8; /* incomplete utf8 character */
							r = s[1];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) + r;
							if (a < 0x38A0 || (0x3BE0 <= a && a <= 0x3BFF))
								goto bad_utf8; /* overlong utf8 character/surrogate */
							r = s[2];
							if (0x80 != (r & 0xC0))
								goto bad_utf8; /* incomplete utf8 character */
							a = (a << 6) + r - 0xE2080;
							s += 3;
						}
					}
					else if (a >= 0xC2) {
						if ((size_t)(se - s) < 2)
							goto bad_utf8; /* incomplete utf8 character */
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8; /* incomplete utf8 character */
						a = (a << 6) + r - 0x3080;
						s += 2;
					}
					else
						goto bad_utf8; /* not expecting 10xxxxxx or overlong utf8 character: 1100000x */
				}
				else
					s++;
				UTF16_PUT(d++, (utf16_char_t)a);
				if (se == s) {
					m = (size_t)(d - *b);
bad_utf8:
					*q = s; /* (*q) < se if bad_utf8, else (*q) == se */
					*b = d;
					return m; /* 0 if bad_utf8, else >0 and <= dst buffer size */
				}
			} while ((const UTF16_CHAR_T*)d != e);
			/* too small output buffer */
			t = s;
small_buf:
			sz = (size_t)(d - *b);
			*b = d;
			if (!determ_req_size) {
				*q = t; /* points after the last successfully converted utf8_char_t, (*q) < se */
				return sz + 1 + m; /* ok, >0, but > dst buffer size */
			}
		}
		do {
			unsigned a = s[0];
			if (a >= 0x80) {
				unsigned r;
				if (a >= 0xE0) {
					if (a >= 0xF0) {
						if (a > 0xF4)
							goto bad_utf8_s; /* unicode code point must be <= 0x10FFFF */
						if ((size_t)(se - s) < 4)
							goto bad_utf8_s; /* incomplete utf8 character */
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						a = (a << 6) + r;
						if (!(0x3C90 <= a && a <= 0x3D8F))
							goto bad_utf8_s; /* overlong utf8 character/out of range */
						r = s[2];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						r = s[3];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						s += 4;
						m += 2; /* + (4 utf8_char_t's - 2 utf16_char_t's) */
					}
					else {
						if ((size_t)(se - s) < 3)
							goto bad_utf8_s; /* incomplete utf8 character */
						r = s[1];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						a = (a << 6) + r;
						if (a < 0x38A0 || (0x3BE0 <= a && a <= 0x3BFF))
							goto bad_utf8_s; /* overlong utf8 character/surrogate */
						r = s[2];
						if (0x80 != (r & 0xC0))
							goto bad_utf8_s; /* incomplete utf8 character */
						s += 3;
						m += 2; /* + (3 utf8_char_t's - 1 utf16_char_t) */
					}
				}
				else if (a >= 0xC2) {
					if ((size_t)(se - s) < 2)
						goto bad_utf8_s; /* incomplete utf8 character */
					r = s[1];
					if (0x80 != (r & 0xC0))
						goto bad_utf8_s; /* incomplete utf8 character */
					s += 2;
					m++; /* + (2 utf8_char_t's - 1 utf16_char_t) */
				}
				else {
					/* not expecting 10xxxxxx or overlong utf8 character: 1100000x */
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
		 1 utf8_char_t   -> 1 utf16_char_t. */
		/* append a number of utf16_char_t's in utf8 string started from 't' */
		sz += (size_t)(s - t) - m;
		*q = t; /* points after the last successfully converted utf8_char_t, (*q) < se */
		return sz; /* ok, >0, but > dst buffer size */
	}
	return 0; /* n is zero */
}

/*
 utf8_to_utf16_z_unsafe
 utf8_to_utf16x_z_unsafe
 utf8_to_utf16u_z_unsafe
 utf8_to_utf16ux_z_unsafe
*/
const utf8_char_t *UTF_FORM_NAME(_z_unsafe)(
	const utf8_char_t *LIBUTF16_RESTRICT q,
	UTF16_CHAR_T *const LIBUTF16_RESTRICT buf)
{
	/* unsigned integer type must be at least of 32 bits */
	UTF16_CHAR_T *LIBUTF16_RESTRICT b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	for (;;) {
		unsigned a = q[0];
		if (a >= 0x80) {
			unsigned r = q[1];
			a = (a << 6) + r;
			if (a >= (0xE0 << 6) + 0x80) {
				r = q[2];
				a = (a << 6) + r;
				if (a >= (((0xF0 << 6) + 0x80) << 6) + 0x80) {
					r = q[3];
					a = (a << 6) + r - 0x682080 - 0x10000;
					UTF16_PUT(b++, (utf16_char_t)(a >> 10)); /* 110110aaaabbbbbb */
					a = (a & 0x3FF) + 0xDC00;                /* 110111bbcccccccc */
					q += 4;
				}
				else {
					a -= 0xE2080;
					q += 3;
				}
			}
			else {
				a -= 0x3080;
				q += 2;
			}
		}
		else
			q++;
		UTF16_PUT(b++, (utf16_char_t)a);
		if (!a)
			return q; /* ok, q[-1] == 0 */
	}
}

/*
 utf8_to_utf16_unsafe
 utf8_to_utf16x_unsafe
 utf8_to_utf16u_unsafe
 utf8_to_utf16ux_unsafe
*/
void UTF_FORM_NAME(_unsafe)(
	const utf8_char_t *LIBUTF16_RESTRICT q,
	UTF16_CHAR_T *const LIBUTF16_RESTRICT buf,
	const size_t n/*>0*/)
{
	/* unsigned integer type must be at least of 32 bits */
	UTF16_CHAR_T *LIBUTF16_RESTRICT b = buf + 0*sizeof(int[1-2*((unsigned)-1 < 0xFFFFFFFF)]);
	const utf8_char_t *const qe = q + n;
	do {
		unsigned a = q[0];
		if (a >= 0x80) {
			unsigned r;
#ifdef _MSC_VER
			__assume(n > 1); /* assume input string is valid */
#endif
			r = q[1];
			a = (a << 6) + r;
			if (a >= (0xE0 << 6) + 0x80) {
#ifdef _MSC_VER
				__assume(n > 2); /* assume input string is valid */
#endif
				r = q[2];
				a = (a << 6) + r;
				if (a >= (((0xF0 << 6) + 0x80) << 6) + 0x80) {
#ifdef _MSC_VER
					__assume(n > 3); /* assume input string is valid */
#endif
					r = q[3];
					a = (a << 6) + r - 0x682080 - 0x10000;
					UTF16_PUT(b++, (utf16_char_t)(a >> 10)); /* 110110aaaabbbbbb */
					a = (a & 0x3FF) + 0xDC00;                /* 110111bbcccccccc */
					q += 4;
				}
				else {
					a -= 0xE2080;
					q += 3;
				}
			}
			else {
				a -= 0x3080;
				q += 2;
			}
		}
		else
			q++;
		UTF16_PUT(b++, (utf16_char_t)a);
	} while (q != qe);
}
