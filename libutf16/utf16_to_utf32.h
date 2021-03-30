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

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_At(q, A_Always(A_Inout))
A_At(*q, A_In_z A_Always(A_Post_notnull))
A_When(!sz, A_Unchanged(*q))
A_When(!sz, A_At(b, A_Maybenull))
A_When(!sz, A_Unchanged(*b))
A_When(sz, A_At(b, A_Always(A_Outptr)))
A_When(sz, A_At(*b, A_Pre_writable_size(sz) A_Post_readable_size(0)))
A_Success(return)
A_When(return <= sz, A_At(A_Old(*b), A_Post_notnull A_Post_z A_Post_readable_size(return)))
#endif
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
(
	const utf16_char_t **const q/*in,out,!=NULL*/,
	utf32_char_t **const b/*in,out,!=NULL if sz>0*/,
	size_t sz/*0?*/,
	const int determ_req_size
);

/* convert utf16 0-terminated string to utf32 0-terminated one,
 input:
  q  - address of the pointer to the beginning of input 0-terminated utf16 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf32_char_t's, if zero - output buffer is not used.
 returns number of stored utf32_char_t's, including terminating 0:
  0     - if utf16 string is invalid,
  <= sz - 0-terminated utf32 string was successfully stored in the output buffer,
  > sz  - output buffer is too small, return value is the required buffer size to store
   whole converted utf32 0-terminated string, including the part that is already converted
   and stored in the output buffer, including 0-terminator, in utf32_char_t's;
 - on success (0 < return <= sz):
  (*q) - points beyond the 0-terminator of input utf16 string,
  (*b) - points beyond the 0-terminator stored in the output buffer;
 - if output buffer is too small (return > sz):
  (*q) - if sz == 0, not changed, else - points beyond last converted (non-0) utf16_char_t,
  (*b) - if sz == 0, not changed, else - points beyond last stored (non-0) utf32_char_t;
 - if input utf16 string is invalid (return == 0):
  (*q) - points beyond the last valid utf16_char_t,
   . if output buffer is too small, last valid utf16_char_t may be beyond last converted one,
   . last valid utf16_char_t is _not_ 0;
  (*b) - if sz > 0, points beyond last successfully converted and stored (non-0) utf32_char_t */
#define utf16x_to_utf32x_z(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16x_to_utf32x_z_(q, b, sz, /*determ_req_size:*/1)

#define utf16x_to_utf32_z(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16x_to_utf32_z_(q, b, sz, /*determ_req_size:*/1)

#define utf16_to_utf32x_z(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16_to_utf32x_z_(q, b, sz, /*determ_req_size:*/1)

#define utf16_to_utf32_z(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16_to_utf32_z_(q, b, sz, /*determ_req_size:*/1)

/* same as utf16_to_utf32_z(), but if output buffer is not empty and is too small, do not
  determine its required size. */
#define utf16x_to_utf32x_z_partial(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16x_to_utf32x_z_(q, b, sz, /*determ_req_size:*/0)

#define utf16x_to_utf32_z_partial(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16x_to_utf32_z_(q, b, sz, /*determ_req_size:*/0)

#define utf16_to_utf32x_z_partial(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16_to_utf32x_z_(q, b, sz, /*determ_req_size:*/0)

#define utf16_to_utf32_z_partial(q/*in,out,!=NULL*/, b/*in,out,!=NULL if sz>0*/, sz/*0?*/) \
	utf16_to_utf32_z_(q, b, sz, /*determ_req_size:*/0)

/* determine the size (in utf32_char_t's) of resulting converted from
  utf16 to utf32 0-terminated string, including terminating 0,
 input:
  q  - address of the pointer to the beginning of input 0-terminated utf16 string.
 returns non-zero on success:
  (*q) - not changed,
 returns 0 on error:
  utf16 string is invalid,
  (*q) - points beyond last valid utf16_char_t,
   . last valid utf16_char_t is _not_ 0 */
#define utf16x_to_utf32x_z_size(q/*in,out,!=NULL*/) utf16x_to_utf32x_z(q, NULL, 0)
#define utf16x_to_utf32_z_size(q/*in,out,!=NULL*/) utf16x_to_utf32_z(q, NULL, 0)
#define utf16_to_utf32x_z_size(q/*in,out,!=NULL*/) utf16_to_utf32x_z(q, NULL, 0)
#define utf16_to_utf32_z_size(q/*in,out,!=NULL*/) utf16_to_utf32_z(q, NULL, 0)

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_When(!n, A_Ret_range(==,0))
A_When(!n, A_At(q, A_Maybenull))
A_When(n, A_At(q, A_Always(A_Inout)))
A_When(n, A_At(*q, A_In_reads(n) A_Always(A_Post_notnull)))
A_When(n && !sz, A_Unchanged(*q))
A_When(!sz || !n, A_At(b, A_Maybenull))
A_When(!sz || !n, A_Unchanged(*b))
A_When(n && sz, A_At(b, A_Always(A_Outptr)))
A_When(n && sz, A_At(*b, A_Pre_writable_size(sz) A_Post_readable_size(0)))
A_Success(return)
A_When(return <= sz, A_At(A_Old(*b), A_Post_notnull A_Post_readable_size(return)))
#endif
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
(
	const utf16_char_t **const q/*in,out,!=NULL if n>0*/,
	utf32_char_t **const b/*in,out,!=NULL if n>0 && sz>0*/,
	size_t sz/*0?*/,
	const size_t n/*0?*/,
	const int determ_req_size
);

/* convert 'n' utf16_char_t's to utf32 ones,
 input:
  q  - address of the pointer to the beginning of input utf16 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf32_char_t's, if zero - output buffer is not used.
  n  - number of utf16_char_t's to convert, if zero - input and output buffers are not used.
 returns number of stored utf32_char_t's:
  0     - if 'n' is zero or an invalid utf16 character is encountered,
  <= sz - all 'n' utf16_char_t's were successfully converted to utf32 ones and stored in the output buffer,
  > sz  - output buffer is too small, return value is the required buffer size to store whole converted
   utf32 string, including the part that is already converted and stored in the output buffer, in utf32_char_t's;
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
#define utf16x_to_utf32x(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16x_to_utf32x_(q, b, sz, n, /*determ_req_size:*/1)

#define utf16x_to_utf32(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16x_to_utf32_(q, b, sz, n, /*determ_req_size:*/1)

#define utf16_to_utf32x(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16_to_utf32x_(q, b, sz, n, /*determ_req_size:*/1)

#define utf16_to_utf32(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16_to_utf32_(q, b, sz, n, /*determ_req_size:*/1)

/* same as utf16_to_utf32(), but if output buffer is not empty and is too small, do not
  determine its required size. */
#define utf16x_to_utf32x_partial(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16x_to_utf32x_(q, b, sz, n, /*determ_req_size:*/0)

#define utf16x_to_utf32_partial(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16x_to_utf32_(q, b, sz, n, /*determ_req_size:*/0)

#define utf16_to_utf32x_partial(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16_to_utf32x_(q, b, sz, n, /*determ_req_size:*/0)

#define utf16_to_utf32_partial(q/*in,out,!=NULL if n>0*/, b/*in,out,!=NULL if n>0 && sz>0*/, sz/*0?*/, n/*0?*/) \
	utf16_to_utf32_(q, b, sz, n, /*determ_req_size:*/0)

/* determine the size (in utf32_char_t's) of resulting buffer needed for converting 'n' utf16_char_t's to utf32 ones,
 input:
  q  - address of the pointer to the beginning of input utf16 string,
  n  - number of utf16_char_t's to convert, if zero - input buffer is not used.
 returns non-zero on success:
   (*q) - not changed,
 returns 0 if 'n' is zero or there is an error:
  utf16 string is invalid,
  (*q) - points beyond last valid utf16_char_t,
   . last valid utf16_char_t is _not_ the last character of utf16 string */
/* Note: zero utf16_char_t is not treated specially, i.e. conversion do not stops */
#define utf16x_to_utf32x_size(q/*in,out,!=NULL if n>0*/, n/*0?*/) utf16x_to_utf32x(q, NULL, 0, n)
#define utf16x_to_utf32_size(q/*in,out,!=NULL if n>0*/, n/*0?*/) utf16x_to_utf32(q, NULL, 0, n)
#define utf16_to_utf32x_size(q/*in,out,!=NULL if n>0*/, n/*0?*/) utf16_to_utf32x(q, NULL, 0, n)
#define utf16_to_utf32_size(q/*in,out,!=NULL if n>0*/, n/*0?*/) utf16_to_utf32(q, NULL, 0, n)

/* for converting remaining part of the source utf16 0-terminated string after calling utf16_to_utf32_z():
  - assume source string is valid,
  - do not check if there is enough space in output buffer, assume it is large enough.
 returns pointer beyond last converted source 0-terminator */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Ret_never_null
A_At(q, A_In_z)
A_At(buf, A_Out A_Post_z)
#endif
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
(
	const utf16_char_t *q/*!=NULL,0-terminated*/,
	utf32_char_t buf[]/*out,!=NULL*/
);

/* for converting remaining part of the source utf16 string after calling utf16_to_utf32():
  - assume source string is valid and 'n' is not zero,
  - do not check if there is enough space in output buffer, assume it is large enough */
/* Note: zero utf16_char_t is not treated specially, i.e. conversion do not stops */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(q, A_In_reads(n))
A_At(buf, A_Out)
A_At(n, A_In_range(>,0))
#endif
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
(
	const utf16_char_t *q/*!=NULL*/,
	utf32_char_t buf[]/*out,!=NULL*/,
	const size_t n/*>0*/
);

#ifdef __cplusplus
}
#endif

#endif /* UTF16_TO_UTF32_H_INCLUDED */
