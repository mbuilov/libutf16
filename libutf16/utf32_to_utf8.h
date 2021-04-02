#ifndef UTF32_TO_UTF8_H_INCLUDED
#define UTF32_TO_UTF8_H_INCLUDED

/**********************************************************************************
* UTF-32 -> UTF-8 characters conversion
* Copyright (C) 2020-2021 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf32_to_utf8.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  group of functions for converting utf32 string to utf8 string:

  utf32{,u}{,x}_to_utf8{,_z}{,_size,_partial,_unsafe}

  such as:

  utf32_to_utf8
  utf32_to_utf8_size
  utf32_to_utf8_partial
  utf32_to_utf8_unsafe
  utf32_to_utf8_z
  utf32_to_utf8_z_size
  utf32_to_utf8_z_partial
  utf32_to_utf8_z_unsafe
  ...

  functions modifiers:
   x - exchange byte order when reading/writing to buffer,
   u - assume buffer is unaligned
*/

/* ------------------------------------------------------------------------------------------ */

/* convert utf32 0-terminated string to utf8 0-terminated one,
 input:
  w  - address of the pointer to the beginning of input 0-terminated utf32 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf8_char_t's, if zero - output buffer is not used.
 returns number of stored utf8_char_t's, including terminating 0:
  0     - if utf32 string is invalid,
  <= sz - 0-terminated utf8 string was successfully stored in the output buffer,
  > sz  - output buffer is too small:
   . if determ_req_size is non-zero, then return value is the required buffer size to store whole
   converted utf8 0-terminated string, including the part that was already converted and stored
   in the output buffer, including 0-terminator, in utf8_char_t's;
   . if determ_req_size is zero and output buffer is not empty and is too small, do not determine
   its required size - return value is a number > sz;
 - on success (0 < return <= sz):
  (*w) - points beyond the 0-terminator of input utf32 string,
  (*b) - points beyond the 0-terminator stored in the output buffer;
 - if output buffer is too small (return > sz):
  (*w) - if sz == 0, not changed, else - points beyond last converted (non-0) utf32_char_t,
  (*b) - if sz == 0, not changed, else - points beyond last stored (non-0) utf8_char_t;
 - if input utf32 string is invalid (return == 0):
  (*w) - points beyond last valid utf32_char_t,
   . if output buffer is too small, last valid utf32_char_t may be beyond last converted one,
   . last valid utf32_char_t is _not_ 0;
  (*b) - if sz > 0, points beyond last successfully converted and stored (non-0) utf8_char_t */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF32_TO_UTF8_Z_(ia) \
A_Check_return \
A_Nonnull_arg(1) \
A_At(w, A_Always(A_Inout)) \
A_At(*w, ia/*A_In_z,A_In*/ A_Always(A_Post_notnull)) \
A_When(!sz, A_Unchanged(*w)) \
A_When(!sz, A_At(b, A_Maybenull)) \
A_When(!sz, A_Unchanged(*b)) \
A_When(sz, A_At(b, A_Always(A_Outptr))) \
A_When(sz, A_At(*b, A_Pre_writable_size(sz) A_Post_readable_size(0))) \
A_Success(return) \
A_When(return <= sz, A_At(A_Old(*b), A_Post_notnull A_Post_z A_Post_readable_size(return)))
#else
#define ANNS_UTF32_TO_UTF8_Z_(ia)
#endif

#define TEMPL_UTF32_TO_UTF8_Z_(name, it, ia) \
ANNS_UTF32_TO_UTF8_Z_(ia) \
size_t name( \
	const it/*utf32_char_t,utf32_char_unaligned_t*/ **const w/*in,out,!=NULL*/, \
	utf8_char_t **const b/*in,out,!=NULL if sz>0*/, \
	size_t sz/*0?*/, \
	const int determ_req_size)

TEMPL_UTF32_TO_UTF8_Z_(utf32_to_utf8_z_, utf32_char_t, A_In_z);
TEMPL_UTF32_TO_UTF8_Z_(utf32x_to_utf8_z_, utf32_char_t, A_In_z);
TEMPL_UTF32_TO_UTF8_Z_(utf32u_to_utf8_z_, utf32_char_unaligned_t, A_In);
TEMPL_UTF32_TO_UTF8_Z_(utf32ux_to_utf8_z_, utf32_char_unaligned_t, A_In);

#undef ANNS_UTF32_TO_UTF8_Z_
#undef TEMPL_UTF32_TO_UTF8_Z_

#define utf32_to_utf8_z(w, b, sz)             utf32_to_utf8_z_(w, b, sz, /*determ_req_size:*/1)
#define utf32x_to_utf8_z(w, b, sz)            utf32x_to_utf8_z_(w, b, sz, /*determ_req_size:*/1)
#define utf32u_to_utf8_z(w, b, sz)            utf32u_to_utf8_z_(w, b, sz, /*determ_req_size:*/1)
#define utf32ux_to_utf8_z(w, b, sz)           utf32ux_to_utf8_z_(w, b, sz, /*determ_req_size:*/1)

#define utf32_to_utf8_z_partial(w, b, sz)     utf32_to_utf8_z_(w, b, sz, /*determ_req_size:*/0)
#define utf32x_to_utf8_z_partial(w, b, sz)    utf32x_to_utf8_z_(w, b, sz, /*determ_req_size:*/0)
#define utf32u_to_utf8_z_partial(w, b, sz)    utf32u_to_utf8_z_(w, b, sz, /*determ_req_size:*/0)
#define utf32ux_to_utf8_z_partial(w, b, sz)   utf32ux_to_utf8_z_(w, b, sz, /*determ_req_size:*/0)

/* determine the size (in utf8_char_t's) of resulting converted from
  utf32 to utf8 0-terminated string, including terminating 0,
 input:
  w - address of the pointer to the beginning of input 0-terminated utf32 string.
 returns non-zero on success:
  (*w) - not changed;
 returns 0 on error:
  utf32 string is invalid,
  (*w) - points beyond last valid utf32_char_t,
   . last valid utf32_char_t is _not_ 0 */
#define utf32_to_utf8_z_size(w/*in,out,!=NULL*/)   utf32_to_utf8_z(w, /*b:*/NULL, /*sz:*/0)
#define utf32x_to_utf8_z_size(w/*in,out,!=NULL*/)  utf32x_to_utf8_z(w, /*b:*/NULL, /*sz:*/0)
#define utf32u_to_utf8_z_size(w/*in,out,!=NULL*/)  utf32u_to_utf8_z(w, /*b:*/NULL, /*sz:*/0)
#define utf32ux_to_utf8_z_size(w/*in,out,!=NULL*/) utf32ux_to_utf8_z(w, /*b:*/NULL, /*sz:*/0)

/* ------------------------------------------------------------------------------------------ */

/* convert 'n' utf32_char_t's to utf8 ones,
 input:
  w  - address of the pointer to the beginning of input utf32 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf8_char_t's, if zero - output buffer is not used,
  n  - number of utf32_char_t's to convert, if zero - input and output buffers are not used.
 returns number of stored utf8_char_t's:
  0     - if 'n' is zero or an invalid utf32 character is encountered,
  <= sz - all 'n' utf32_char_t's were successfully converted to utf8 ones and stored in the output buffer,
  > sz  - output buffer is too small:
   . if determ_req_size is non-zero, then return value is the required buffer size to store whole converted
   utf8 string, including the part that was already converted and stored in the output buffer, in utf8_char_t's;
   . if determ_req_size is zero and output buffer is not empty and is too small, do not determine
   its required size - return value is a number > sz;
 - on success (0 < return <= sz):
  (*w) - points beyond last source utf32_char_t of input string,
  (*b) - points beyond last converted utf8_char_t stored in the output buffer;
 - if output buffer is too small (return > sz):
  (*w) - if sz == 0, not changed, else - points beyond last converted utf32_char_t,
  (*b) - if sz == 0, not changed, else - points beyond last stored utf8_char_t;
 - if input utf32 string is invalid (return == 0):
  (*w) - points beyond last valid utf32_char_t,
   . if output buffer is too small, last valid utf32_char_t may be beyond last converted one,
   . last valid utf32_char_t is _not_ the last character of utf32 string;
  (*b) - if sz > 0, points beyond last successfully converted and stored utf8_char_t */
/* Note: zero utf32_char_t is not treated specially, i.e. conversion do not stops */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF32_TO_UTF8_ \
A_Check_return \
A_When(!n, A_Ret_range(==,0)) \
A_When(!n, A_At(w, A_Maybenull)) \
A_When(n, A_At(w, A_Always(A_Inout))) \
A_When(n, A_At(*w, A_In_reads(n) A_Always(A_Post_notnull))) \
A_When(n && !sz, A_Unchanged(*w)) \
A_When(!sz || !n, A_At(b, A_Maybenull)) \
A_When(!sz || !n, A_Unchanged(*b)) \
A_When(n && sz, A_At(b, A_Always(A_Outptr))) \
A_When(n && sz, A_At(*b, A_Pre_writable_size(sz) A_Post_readable_size(0))) \
A_Success(return) \
A_When(return <= sz, A_At(A_Old(*b), A_Post_notnull A_Post_readable_size(return)))
#else
#define ANNS_UTF32_TO_UTF8_
#endif

#define TEMPL_UTF32_TO_UTF8_(name, it) \
ANNS_UTF32_TO_UTF8_ \
size_t name( \
	const it/*utf32_char_t,utf32_char_unaligned_t*/ **const w/*in,out,!=NULL if n>0*/, \
	utf8_char_t **const b/*in,out,!=NULL if n>0 && sz>0*/, \
	size_t sz/*0?*/, \
	const size_t n/*0?*/, \
	const int determ_req_size)

TEMPL_UTF32_TO_UTF8_(utf32_to_utf8_, utf32_char_t);
TEMPL_UTF32_TO_UTF8_(utf32x_to_utf8_, utf32_char_t);
TEMPL_UTF32_TO_UTF8_(utf32u_to_utf8_, utf32_char_unaligned_t);
TEMPL_UTF32_TO_UTF8_(utf32ux_to_utf8_, utf32_char_unaligned_t);

#undef ANNS_UTF32_TO_UTF8_
#undef TEMPL_UTF32_TO_UTF8_

#define utf32_to_utf8(w, b, sz, n)             utf32_to_utf8_(w, b, sz, n, /*determ_req_size:*/1)
#define utf32x_to_utf8(w, b, sz, n)            utf32x_to_utf8_(w, b, sz, n, /*determ_req_size:*/1)
#define utf32u_to_utf8(w, b, sz, n)            utf32u_to_utf8_(w, b, sz, n, /*determ_req_size:*/1)
#define utf32ux_to_utf8(w, b, sz, n)           utf32ux_to_utf8_(w, b, sz, n, /*determ_req_size:*/1)

#define utf32_to_utf8_partial(w, b, sz, n)     utf32_to_utf8_(w, b, sz, n, /*determ_req_size:*/0)
#define utf32x_to_utf8_partial(w, b, sz, n)    utf32x_to_utf8_(w, b, sz, n, /*determ_req_size:*/0)
#define utf32u_to_utf8_partial(w, b, sz, n)    utf32u_to_utf8_(w, b, sz, n, /*determ_req_size:*/0)
#define utf32ux_to_utf8_partial(w, b, sz, n)   utf32ux_to_utf8_(w, b, sz, n, /*determ_req_size:*/0)

/* determine the size (in utf8_char_t's) of resulting buffer needed for converting 'n' utf32_char_t's to utf8 ones,
 input:
  w - address of the pointer to the beginning of input utf32 string,
  n - number of utf32_char_t's to convert, if zero - input buffer is not used.
 returns non-zero on success:
  (*w) - not changed;
 returns 0 if 'n' is zero or there is an error:
  utf32 string is invalid,
  (*w) - points beyond last valid utf32_char_t,
   . last valid utf32_char_t is _not_ the last character of utf32 string */
/* Note: zero utf32_char_t is not treated specially, i.e. conversion do not stops */
#define utf32_to_utf8_size(w/*in,out,!=NULL if n>0*/, n/*0?*/)   utf32_to_utf8(w, /*b:*/NULL, /*sz:*/0, n)
#define utf32x_to_utf8_size(w/*in,out,!=NULL if n>0*/, n/*0?*/)  utf32x_to_utf8(w, /*b:*/NULL, /*sz:*/0, n)
#define utf32u_to_utf8_size(w/*in,out,!=NULL if n>0*/, n/*0?*/)  utf32u_to_utf8(w, /*b:*/NULL, /*sz:*/0, n)
#define utf32ux_to_utf8_size(w/*in,out,!=NULL if n>0*/, n/*0?*/) utf32ux_to_utf8(w, /*b:*/NULL, /*sz:*/0, n)

/* ------------------------------------------------------------------------------------------ */

/* for converting remaining part of the source utf32 0-terminated string after calling utf32_to_utf8_z():
  - assume source string is valid,
  - do not check if there is enough space in output buffer, assume it is large enough.
 returns pointer beyond last converted source 0-terminator */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF32_TO_UTF8_Z_UNSAFE(ia) \
A_Nonnull_all_args \
A_Ret_never_null \
A_At(w, ia/*A_In_z,A_In*/) \
A_At(buf, A_Out A_Post_z)
#else
#define ANNS_UTF32_TO_UTF8_Z_UNSAFE(ia)
#endif

#define TEMPL_UTF32_TO_UTF8_Z_UNSAFE(name, it, ia) \
ANNS_UTF32_TO_UTF8_Z_UNSAFE(ia) \
const it/*utf32_char_t,utf32_char_unaligned_t*/ *name( \
	const it/*utf32_char_t,utf32_char_unaligned_t*/ *w/*!=NULL,0-terminated*/, \
	utf8_char_t buf[]/*out,!=NULL*/)

TEMPL_UTF32_TO_UTF8_Z_UNSAFE(utf32_to_utf8_z_unsafe, utf32_char_t, A_In_z);
TEMPL_UTF32_TO_UTF8_Z_UNSAFE(utf32x_to_utf8_z_unsafe, utf32_char_t, A_In_z);
TEMPL_UTF32_TO_UTF8_Z_UNSAFE(utf32u_to_utf8_z_unsafe, utf32_char_unaligned_t, A_In);
TEMPL_UTF32_TO_UTF8_Z_UNSAFE(utf32ux_to_utf8_z_unsafe, utf32_char_unaligned_t, A_In);

#undef ANNS_UTF32_TO_UTF8_Z_UNSAFE
#undef TEMPL_UTF32_TO_UTF8_Z_UNSAFE

/* ------------------------------------------------------------------------------------------ */

/* for converting remaining part of the source utf32 string after calling utf32_to_utf8():
  - assume source string is valid and 'n' is not zero,
  - do not check if there is enough space in output buffer, assume it is large enough */
/* Note: zero utf32_char_t is not treated specially, i.e. conversion do not stops */

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define ANNS_UTF32_TO_UTF8_UNSAFE \
A_Nonnull_all_args \
A_At(w, A_In_reads(n)) \
A_At(buf, A_Out) \
A_At(n, A_In_range(>,0))
#else
#define ANNS_UTF32_TO_UTF8_UNSAFE
#endif

#define TEMPL_UTF32_TO_UTF8_UNSAFE(name, it) \
ANNS_UTF32_TO_UTF8_UNSAFE \
void name( \
	const it/*utf32_char_t,utf32_char_unaligned_t*/ *w/*!=NULL*/, \
	utf8_char_t buf[]/*out,!=NULL*/, \
	const size_t n/*>0*/)

TEMPL_UTF32_TO_UTF8_UNSAFE(utf32_to_utf8_unsafe, utf32_char_t);
TEMPL_UTF32_TO_UTF8_UNSAFE(utf32x_to_utf8_unsafe, utf32_char_t);
TEMPL_UTF32_TO_UTF8_UNSAFE(utf32u_to_utf8_unsafe, utf32_char_unaligned_t);
TEMPL_UTF32_TO_UTF8_UNSAFE(utf32ux_to_utf8_unsafe, utf32_char_unaligned_t);

#undef ANNS_UTF32_TO_UTF8_UNSAFE
#undef TEMPL_UTF32_TO_UTF8_UNSAFE

#ifdef __cplusplus
}
#endif

#endif /* UTF32_TO_UTF8_H_INCLUDED */
