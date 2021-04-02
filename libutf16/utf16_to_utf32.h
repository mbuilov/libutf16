#ifndef UTF16_TO_UTF32_H_INCLUDED
#define UTF16_TO_UTF32_H_INCLUDED

/**********************************************************************************
* UTF-16 -> UTF-32 characters conversion
* Copyright (C) 2020-2021 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf32.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  group of functions for converting utf16 string to utf32 string:

  utf16{,u}{,x}_to_utf32{,u}{,x}{,_z}{,_size,_partial,_unsafe}

  such as:

  utf16_to_utf32
  utf16_to_utf32_size
  utf16_to_utf32_partial
  utf16_to_utf32_unsafe
  utf16_to_utf32_z
  utf16_to_utf32_z_size
  utf16_to_utf32_z_partial
  utf16_to_utf32_z_unsafe
  ...

  functions modifiers:
   x - exchange byte order when reading/writing to buffer,
   u - assume buffer is unaligned
*/

/* ------------------------------------------------------------------------------------------ */

/* convert utf16 0-terminated string to utf32 0-terminated one,
 input:
  q  - address of the pointer to the beginning of input 0-terminated utf16 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf32_char_t's, if zero - output buffer is not used.
 returns number of stored utf32_char_t's, including terminating 0:
  0     - if utf16 string is invalid,
  <= sz - 0-terminated utf32 string was successfully stored in the output buffer,
  > sz  - output buffer is too small:
   . if determ_req_size is non-zero, then return value is the required buffer size to store whole
   converted utf32 0-terminated string, including the part that was already converted and stored
   in the output buffer, including 0-terminator, in utf32_char_t's;
   . if determ_req_size is zero and output buffer is not empty and is too small, do not determine
   its required size - return value is a number > sz;
 - on success (0 < return <= sz):
  (*q) - points beyond the 0-terminator of input utf16 string,
  (*b) - points beyond the 0-terminator stored in the output buffer;
 - if output buffer is too small (return > sz):
  (*q) - if sz == 0, not changed, else - points beyond last converted (non-0) utf16_char_t,
  (*b) - if sz == 0, not changed, else - points beyond last stored (non-0) utf32_char_t;
 - if input utf16 string is invalid (return == 0):
  (*q) - points beyond last valid utf16_char_t,
   . if output buffer is too small, last valid utf16_char_t may be beyond last converted one,
   . last valid utf16_char_t is _not_ 0;
  (*b) - if sz > 0, points beyond last successfully converted and stored (non-0) utf32_char_t */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF16_TO_UTF32_Z_(ia, oa) \
A_Check_return \
A_Nonnull_arg(1) \
A_At(q, A_Always(A_Inout)) \
A_At(*q, ia/*A_In_z,A_In*/ A_Always(A_Post_notnull)) \
A_When(!sz, A_Unchanged(*q)) \
A_When(!sz, A_At(b, A_Maybenull)) \
A_When(!sz, A_Unchanged(*b)) \
A_When(sz, A_At(b, A_Always(A_Outptr))) \
A_When(sz, A_At(*b, A_Pre_writable_size(sz) A_Post_readable_size(0))) \
A_Success(return) \
A_When(return <= sz, A_At(A_Old(*b), A_Post_notnull oa/*A_Post_z,*/ A_Post_readable_size(return)))
#else
#define ANNS_UTF16_TO_UTF32_Z_(ia, oa)
#endif

#define TEMPL_UTF16_TO_UTF32_Z_(name, it, ot, ia, oa) \
ANNS_UTF16_TO_UTF32_Z_(ia, oa) \
size_t name( \
	const it/*utf16_char_t,utf16_char_unaligned_t*/ **const q/*in,out,!=NULL*/, \
	ot/*utf32_char_t,utf32_char_unaligned_t*/ **const b/*in,out,!=NULL if sz>0*/, \
	size_t sz/*0?*/, \
	const int determ_req_size)

TEMPL_UTF16_TO_UTF32_Z_(utf16_to_utf32_z_, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16_to_utf32x_z_, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16_to_utf32u_z_, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_(utf16_to_utf32ux_z_, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_(utf16x_to_utf32_z_, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16x_to_utf32x_z_, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16x_to_utf32u_z_, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_(utf16x_to_utf32ux_z_, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_(utf16u_to_utf32_z_, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16u_to_utf32x_z_, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16u_to_utf32u_z_, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);
TEMPL_UTF16_TO_UTF32_Z_(utf16u_to_utf32ux_z_, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);
TEMPL_UTF16_TO_UTF32_Z_(utf16ux_to_utf32_z_, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16ux_to_utf32x_z_, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_(utf16ux_to_utf32u_z_, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);
TEMPL_UTF16_TO_UTF32_Z_(utf16ux_to_utf32ux_z_, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);

#undef ANNS_UTF16_TO_UTF32_Z_
#undef TEMPL_UTF16_TO_UTF32_Z_

#define utf16_to_utf32_z(q, b, sz)             utf16_to_utf32_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16_to_utf32x_z(q, b, sz)            utf16_to_utf32x_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16_to_utf32u_z(q, b, sz)            utf16_to_utf32u_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16_to_utf32ux_z(q, b, sz)           utf16_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16x_to_utf32_z(q, b, sz)            utf16x_to_utf32_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16x_to_utf32x_z(q, b, sz)           utf16x_to_utf32x_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16x_to_utf32u_z(q, b, sz)           utf16x_to_utf32u_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16x_to_utf32ux_z(q, b, sz)          utf16x_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16u_to_utf32_z(q, b, sz)            utf16u_to_utf32_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16u_to_utf32x_z(q, b, sz)           utf16u_to_utf32x_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16u_to_utf32u_z(q, b, sz)           utf16u_to_utf32u_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16u_to_utf32ux_z(q, b, sz)          utf16u_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16ux_to_utf32_z(q, b, sz)           utf16ux_to_utf32_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16ux_to_utf32x_z(q, b, sz)          utf16ux_to_utf32x_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16ux_to_utf32u_z(q, b, sz)          utf16ux_to_utf32u_z_(q, b, sz, /*determ_req_size:*/1)
#define utf16ux_to_utf32ux_z(q, b, sz)         utf16ux_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/1)

#define utf16_to_utf32_z_partial(q, b, sz)     utf16_to_utf32_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16_to_utf32x_z_partial(q, b, sz)    utf16_to_utf32x_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16_to_utf32u_z_partial(q, b, sz)    utf16_to_utf32u_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16_to_utf32ux_z_partial(q, b, sz)   utf16_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16x_to_utf32_z_partial(q, b, sz)    utf16x_to_utf32_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16x_to_utf32x_z_partial(q, b, sz)   utf16x_to_utf32x_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16x_to_utf32u_z_partial(q, b, sz)   utf16x_to_utf32u_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16x_to_utf32ux_z_partial(q, b, sz)  utf16x_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16u_to_utf32_z_partial(q, b, sz)    utf16u_to_utf32_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16u_to_utf32x_z_partial(q, b, sz)   utf16u_to_utf32x_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16u_to_utf32u_z_partial(q, b, sz)   utf16u_to_utf32u_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16u_to_utf32ux_z_partial(q, b, sz)  utf16u_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16ux_to_utf32_z_partial(q, b, sz)   utf16ux_to_utf32_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16ux_to_utf32x_z_partial(q, b, sz)  utf16ux_to_utf32x_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16ux_to_utf32u_z_partial(q, b, sz)  utf16ux_to_utf32u_z_(q, b, sz, /*determ_req_size:*/0)
#define utf16ux_to_utf32ux_z_partial(q, b, sz) utf16ux_to_utf32ux_z_(q, b, sz, /*determ_req_size:*/0)

/* determine the size (in utf32_char_t's) of resulting converted from
  utf16 to utf32 0-terminated string, including terminating 0,
 input:
  q - address of the pointer to the beginning of input 0-terminated utf16 string.
 returns non-zero on success:
  (*q) - not changed;
 returns 0 on error:
  utf16 string is invalid,
  (*q) - points beyond last valid utf16_char_t,
   . last valid utf16_char_t is _not_ 0 */
#define utf16_to_utf32_z_size(q/*in,out,!=NULL*/)   utf16_to_utf32_z(q, /*b:*/NULL, /*sz:*/0)
#define utf16x_to_utf32_z_size(q/*in,out,!=NULL*/)  utf16x_to_utf32_z(q, /*b:*/NULL, /*sz:*/0)
#define utf16u_to_utf32_z_size(q/*in,out,!=NULL*/)  utf16u_to_utf32_z(q, /*b:*/NULL, /*sz:*/0)
#define utf16ux_to_utf32_z_size(q/*in,out,!=NULL*/) utf16ux_to_utf32_z(q, /*b:*/NULL, /*sz:*/0)

/* ------------------------------------------------------------------------------------------ */

/* convert 'n' utf16_char_t's to utf32 ones,
 input:
  q  - address of the pointer to the beginning of input utf16 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf32_char_t's, if zero - output buffer is not used.
  n  - number of utf16_char_t's to convert, if zero - input and output buffers are not used.
 returns number of stored utf32_char_t's:
  0     - if 'n' is zero or an invalid utf16 character is encountered,
  <= sz - all 'n' utf16_char_t's were successfully converted to utf32 ones and stored in the output buffer,
  > sz  - output buffer is too small:
   . if determ_req_size is non-zero, then return value is the required buffer size to store whole converted
   utf32 string, including the part that was already converted and stored in the output buffer, in utf32_char_t's;
   . if determ_req_size is zero and output buffer is not empty and is too small, do not determine
   its required size - return value is a number > sz;
 - on success (0 < return <= sz):
  (*q) - points beyond last source utf16_char_t of input string,
  (*b) - points beyond last converted utf32_char_t stored in the output buffer;
 - if output buffer is too small (return > sz):
  (*q) - if sz == 0, not changed, else - points beyond last converted utf16_char_t,
  (*b) - if sz == 0, not changed, else - points beyond last stored utf32_char_t;
 - if input utf16 string is invalid (return == 0):
  (*q) - points beyond last valid utf16_char_t,
   . if output buffer is too small, last valid utf16_char_t may be beyond last converted one,
   . last valid utf16_char_t is _not_ the last character of utf16 string;
  (*b) - if sz > 0, points beyond last successfully converted and stored utf32_char_t */
/* Note: zero utf16_char_t is not treated specially, i.e. conversion do not stops */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF16_TO_UTF32_ \
A_Check_return \
A_When(!n, A_Ret_range(==,0)) \
A_When(!n, A_At(q, A_Maybenull)) \
A_When(n, A_At(q, A_Always(A_Inout))) \
A_When(n, A_At(*q, A_In_reads(n) A_Always(A_Post_notnull))) \
A_When(n && !sz, A_Unchanged(*q)) \
A_When(!sz || !n, A_At(b, A_Maybenull)) \
A_When(!sz || !n, A_Unchanged(*b)) \
A_When(n && sz, A_At(b, A_Always(A_Outptr))) \
A_When(n && sz, A_At(*b, A_Pre_writable_size(sz) A_Post_readable_size(0))) \
A_Success(return) \
A_When(return <= sz, A_At(A_Old(*b), A_Post_notnull A_Post_readable_size(return)))
#else
#define ANNS_UTF16_TO_UTF32_
#endif

#define TEMPL_UTF16_TO_UTF32_(name, it, ot) \
ANNS_UTF16_TO_UTF32_ \
size_t name( \
	const it/*utf16_char_t,utf16_char_unaligned_t*/ **const q/*in,out,!=NULL if n>0*/, \
	ot/*utf32_char_t,utf32_char_unaligned_t*/ **const b/*in,out,!=NULL if n>0 && sz>0*/, \
	size_t sz/*0?*/, \
	const size_t n/*0?*/, \
	const int determ_req_size)

TEMPL_UTF16_TO_UTF32_(utf16_to_utf32_, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16_to_utf32x_, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16_to_utf32u_, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_(utf16_to_utf32ux_, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_(utf16x_to_utf32_, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16x_to_utf32x_, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16x_to_utf32u_, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_(utf16x_to_utf32ux_, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_(utf16u_to_utf32_, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16u_to_utf32x_, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16u_to_utf32u_, utf16_char_unaligned_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_(utf16u_to_utf32ux_, utf16_char_unaligned_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_(utf16ux_to_utf32_, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16ux_to_utf32x_, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_(utf16ux_to_utf32u_, utf16_char_unaligned_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_(utf16ux_to_utf32ux_, utf16_char_unaligned_t, utf32_char_unaligned_t);

#undef ANNS_UTF16_TO_UTF32_
#undef TEMPL_UTF16_TO_UTF32_

#define utf16_to_utf32(q, b, sz, n)             utf16_to_utf32_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16_to_utf32x(q, b, sz, n)            utf16_to_utf32x_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16_to_utf32u(q, b, sz, n)            utf16_to_utf32u_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16_to_utf32ux(q, b, sz, n)           utf16_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16x_to_utf32(q, b, sz, n)            utf16x_to_utf32_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16x_to_utf32x(q, b, sz, n)           utf16x_to_utf32x_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16x_to_utf32u(q, b, sz, n)           utf16x_to_utf32u_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16x_to_utf32ux(q, b, sz, n)          utf16x_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16u_to_utf32(q, b, sz, n)            utf16u_to_utf32_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16u_to_utf32x(q, b, sz, n)           utf16u_to_utf32x_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16u_to_utf32u(q, b, sz, n)           utf16u_to_utf32u_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16u_to_utf32ux(q, b, sz, n)          utf16u_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16ux_to_utf32(q, b, sz, n)           utf16ux_to_utf32_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16ux_to_utf32x(q, b, sz, n)          utf16ux_to_utf32x_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16ux_to_utf32u(q, b, sz, n)          utf16ux_to_utf32u_(q, b, sz, n, /*determ_req_size:*/1)
#define utf16ux_to_utf32ux(q, b, sz, n)         utf16ux_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/1)

#define utf16_to_utf32_partial(q, b, sz, n)     utf16_to_utf32_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16_to_utf32x_partial(q, b, sz, n)    utf16_to_utf32x_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16_to_utf32u_partial(q, b, sz, n)    utf16_to_utf32u_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16_to_utf32ux_partial(q, b, sz, n)   utf16_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16x_to_utf32_partial(q, b, sz, n)    utf16x_to_utf32_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16x_to_utf32x_partial(q, b, sz, n)   utf16x_to_utf32x_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16x_to_utf32u_partial(q, b, sz, n)   utf16x_to_utf32u_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16x_to_utf32ux_partial(q, b, sz, n)  utf16x_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16u_to_utf32_partial(q, b, sz, n)    utf16u_to_utf32_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16u_to_utf32x_partial(q, b, sz, n)   utf16u_to_utf32x_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16u_to_utf32u_partial(q, b, sz, n)   utf16u_to_utf32u_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16u_to_utf32ux_partial(q, b, sz, n)  utf16u_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16ux_to_utf32_partial(q, b, sz, n)   utf16ux_to_utf32_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16ux_to_utf32x_partial(q, b, sz, n)  utf16ux_to_utf32x_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16ux_to_utf32u_partial(q, b, sz, n)  utf16ux_to_utf32u_(q, b, sz, n, /*determ_req_size:*/0)
#define utf16ux_to_utf32ux_partial(q, b, sz, n) utf16ux_to_utf32ux_(q, b, sz, n, /*determ_req_size:*/0)

/* determine the size (in utf32_char_t's) of resulting buffer needed for converting 'n' utf16_char_t's to utf32 ones,
 input:
  q - address of the pointer to the beginning of input utf16 string,
  n - number of utf16_char_t's to convert, if zero - input buffer is not used.
 returns non-zero on success:
  (*q) - not changed;
 returns 0 if 'n' is zero or there is an error:
  utf16 string is invalid,
  (*q) - points beyond last valid utf16_char_t,
   . last valid utf16_char_t is _not_ the last character of utf16 string */
/* Note: zero utf16_char_t is not treated specially, i.e. conversion do not stops */
#define utf16_to_utf32_size(q/*in,out,!=NULL if n>0*/, n/*0?*/)   utf16_to_utf32(q, /*b:*/NULL, /*sz:*/0, n)
#define utf16x_to_utf32_size(q/*in,out,!=NULL if n>0*/, n/*0?*/)  utf16x_to_utf32(q, /*b:*/NULL, /*sz:*/0, n)
#define utf16u_to_utf32_size(q/*in,out,!=NULL if n>0*/, n/*0?*/)  utf16u_to_utf32(q, /*b:*/NULL, /*sz:*/0, n)
#define utf16ux_to_utf32_size(q/*in,out,!=NULL if n>0*/, n/*0?*/) utf16ux_to_utf32(q, /*b:*/NULL, /*sz:*/0, n)

/* ------------------------------------------------------------------------------------------ */

/* for converting remaining part of the source utf16 0-terminated string after calling utf16_to_utf32_z():
  - assume source string is valid,
  - do not check if there is enough space in output buffer, assume it is large enough.
 returns pointer beyond last converted source 0-terminator */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF16_TO_UTF32_Z_UNSAFE(ia, oa) \
A_Nonnull_all_args \
A_Ret_never_null \
A_At(q, ia/*A_In_z,A_In*/) \
A_At(buf, A_Out oa/*A_Post_z,*/)
#else
#define ANNS_UTF16_TO_UTF32_Z_UNSAFE(ia, oa)
#endif

#define TEMPL_UTF16_TO_UTF32_Z_UNSAFE(name, it, ot, ia, oa) \
ANNS_UTF16_TO_UTF32_Z_UNSAFE(ia, oa) \
const it/*utf16_char_t,utf16_char_unaligned_t*/ *name( \
	const it/*utf16_char_t,utf16_char_unaligned_t*/ *q/*!=NULL,0-terminated*/, \
	ot/*utf32_char_t,utf32_char_unaligned_t*/ buf[]/*out,!=NULL*/)

TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16_to_utf32_z_unsafe, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16_to_utf32x_z_unsafe, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16_to_utf32u_z_unsafe, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16_to_utf32ux_z_unsafe, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16x_to_utf32_z_unsafe, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16x_to_utf32x_z_unsafe, utf16_char_t, utf32_char_t, A_In_z, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16x_to_utf32u_z_unsafe, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16x_to_utf32ux_z_unsafe, utf16_char_t, utf32_char_unaligned_t, A_In_z,);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16u_to_utf32_z_unsafe, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16u_to_utf32x_z_unsafe, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16u_to_utf32u_z_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16u_to_utf32ux_z_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16ux_to_utf32_z_unsafe, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16ux_to_utf32x_z_unsafe, utf16_char_unaligned_t, utf32_char_t, A_In, A_Post_z);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16ux_to_utf32u_z_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);
TEMPL_UTF16_TO_UTF32_Z_UNSAFE(utf16ux_to_utf32ux_z_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t, A_In,);

#undef ANNS_UTF16_TO_UTF32_Z_UNSAFE
#undef TEMPL_UTF16_TO_UTF32_Z_UNSAFE

/* ------------------------------------------------------------------------------------------ */

/* for converting remaining part of the source utf16 string after calling utf16_to_utf32():
  - assume source string is valid and 'n' is not zero,
  - do not check if there is enough space in output buffer, assume it is large enough */
/* Note: zero utf16_char_t is not treated specially, i.e. conversion do not stops */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF16_TO_UTF32_UNSAFE \
A_Nonnull_all_args \
A_At(q, A_In_reads(n)) \
A_At(buf, A_Out) \
A_At(n, A_In_range(>,0))
#else
#define ANNS_UTF16_TO_UTF32_UNSAFE
#endif

#define TEMPL_UTF16_TO_UTF32_UNSAFE(name, it, ot) \
ANNS_UTF16_TO_UTF32_UNSAFE \
void name( \
	const it/*utf16_char_t,utf16_char_unaligned_t*/ *q/*!=NULL*/, \
	ot/*utf32_char_t,utf32_char_unaligned_t*/ buf[]/*out,!=NULL*/, \
	const size_t n/*>0*/)

TEMPL_UTF16_TO_UTF32_UNSAFE(utf16_to_utf32_unsafe, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16_to_utf32x_unsafe, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16_to_utf32u_unsafe, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16_to_utf32ux_unsafe, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16x_to_utf32_unsafe, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16x_to_utf32x_unsafe, utf16_char_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16x_to_utf32u_unsafe, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16x_to_utf32ux_unsafe, utf16_char_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16u_to_utf32_unsafe, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16u_to_utf32x_unsafe, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16u_to_utf32u_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16u_to_utf32ux_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16ux_to_utf32_unsafe, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16ux_to_utf32x_unsafe, utf16_char_unaligned_t, utf32_char_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16ux_to_utf32u_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t);
TEMPL_UTF16_TO_UTF32_UNSAFE(utf16ux_to_utf32ux_unsafe, utf16_char_unaligned_t, utf32_char_unaligned_t);

#undef ANNS_UTF16_TO_UTF32_UNSAFE
#undef TEMPL_UTF16_TO_UTF32_UNSAFE

#ifdef __cplusplus
}
#endif

#endif /* UTF16_TO_UTF32_H_INCLUDED */
