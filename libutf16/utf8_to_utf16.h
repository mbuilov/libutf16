#ifndef UTF8_TO_UTF16_H_INCLUDED
#define UTF8_TO_UTF16_H_INCLUDED

/**********************************************************************************
* UTF-8 -> UTF-16 characters conversion
* Copyright (C) 2018 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf8_to_utf16.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Restrict
#endif

/* convert utf8 0-terminated string to utf16 0-terminated one,
 input:
  q  - address of the pointer to the beginning of input 0-terminated utf8 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf16_char_t's, if zero - output buffer is not used.
 returns number of stored utf16_char_t's, including terminating 0:
  0     - if utf8 string is invalid,
  <= sz - 0-terminated utf16 string was successfully stored in the output buffer,
  > sz  - output buffer is too small, return value is the required buffer size to store
   whole converted utf16 0-terminated string, including the part that is already converted
   and stored in the output buffer, including 0-terminator, in utf16_char_t's;
 - on success:
  (*q) - points beyond the 0-terminator of input utf8 string,
  (*b) - points beyond the 0-terminator stored in the output buffer;
 - if output buffer is too small:
  (*q) - if sz == 0, not changed, else - points beyond the last converted (non-0) utf8_char_t,
  (*b) - if sz == 0, not changed, else - points beyond the last stored (non-0) utf16_char_utf16_char_t;
 - if input utf8 string is invalid:
  (*q) - points beyond the last valid utf8_char_t,
   . if output buffer is too small, the last valid utf8_char_t may be beyond last converted one,
   . the last valid utf8_char_t is _not_ 0;
  (*b) - if sz > 0, points beyond the last successfully converted and stored (non-0) utf16_char_t */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_At(q, A_Always(A_Inout))
A_At(*q, A_In_z)
A_When(sz, A_At(b, A_Always(A_Out)))
A_When(sz, A_At(*b, A_Writable_elements(sz)) A_Always(A_Notvalid))
A_Success(return)
A_When(return <= sz, A_At(A_Old(*b), A_Post_z A_Post_readable_size(return)))
#endif
size_t utf8_to_utf16_z(const utf8_char_t **const q, utf16_char_t **const b, size_t sz);

/* convert 'n' utf8_char_t's to utf16 ones,
 input:
  q  - address of the pointer to the beginning of input utf8 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf16_char_t's, if zero - output buffer is not used.
  n  - number of utf8_char_t's to convert, if zero - input and output buffers are not used.
 returns number of stored utf16_char_t's:
  0     - if 'n' is zero or an invalid utf8 character is encountered,
  <= sz - all 'n' utf8_char_t's were successfully converted to utf16 ones and stored in the output buffer,
  > sz  - output buffer is too small, return value is the required buffer size to store whole converted
   utf16 string, including the part that is already converted and stored in the output buffer, in utf16_char_t's;
 - on success:
  (*q) - points beyond the last source utf8_char_t of input string,
  (*b) - points beyond the last converted utf16_char_t stored in the output buffer;
 - if output buffer is too small:
  (*q) - if sz == 0, not changed, else - points beyond the last converted utf8_char_t,
  (*b) - if sz == 0, not changed, else - points beyond the last stored utf16_char_t;
 - if input utf8 string is invalid:
  (*q) - points beyond the last valid utf8_char_t,
   . if output buffer is too small, the last valid utf8_char_t may be beyond last converted one,
   . the last valid utf8_char_t is _not_ the last character of utf8 string;
  (*b) - if sz > 0, points beyond the last successfully converted and stored utf16_char_t */
/* Note: zero utf8_char_t is not treated specially, i.e. conversion do not stops */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_When(!n, A_Ret_range(==,0))
A_At(q, A_Always(A_Inout))
A_At(*q, A_In_reads(n))
A_When(n && sz, A_At(b, A_Always(A_Out)))
A_When(n && sz, A_At(*b, A_Writable_elements(sz)) A_Always(A_Notvalid))
A_Success(return)
A_When(return <= sz, A_At(A_Old(*b), A_Post_readable_size(return)))
#endif
size_t utf8_to_utf16(const utf8_char_t **const q, utf16_char_t **const b, size_t sz, const size_t n);

/* convert utf8 0-terminated string to utf16 0-terminated one,
  do not check if there is enough space in output buffer, assume it is large enough.
 returns number of stored utf16_char_t's, including terminating 0:
  0    - if utf8 string is invalid,
  else - stored utf16 string length, including 0-terminator, in utf16_char_t's */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_Success(return)
A_At(q, A_In_z)
A_At(buf, A_Out A_Post_z A_Post_readable_size(return))
#endif
size_t utf8_to_utf16_z_unsafe_out(const utf8_char_t *A_Restrict q, utf16_char_t *const A_Restrict buf);

/* same as utf8_to_utf16_z_unsafe_out(), but also do not check that input utf8 string is valid */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Ret_range(>,0)
A_At(q, A_In_z)
A_At(buf, A_Out A_Post_z A_Post_readable_size(return))
#endif
size_t utf8_to_utf16_z_unsafe(const utf8_char_t *A_Restrict q, utf16_char_t *const A_Restrict buf);

/* convert 'n' utf8_char_t's to utf16 ones (if 'n' is zero - input and output buffers are not used),
  do not check if there is enough space in output buffer, assume it is large enough.
 returns number of stored utf16_char_t's:
  0    - if 'n' is zero or an invalid utf8 character is encountered,
  else - number of stored utf16_char_t's */
/* Note: zero utf8_char_t is not treated specially, i.e. conversion do not stops */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_When(!n, A_Ret_range(==,0))
A_At(q, A_In_reads(n))
A_At(buf, A_Out A_Post_readable_size(return))
#endif
size_t utf8_to_utf16_unsafe_out(const utf8_char_t *A_Restrict q, utf16_char_t *const A_Restrict buf, const size_t n);

/* same as utf8_to_utf16_unsafe_out(), but also do not check that input utf8 string is valid */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_When(!n, A_Ret_range(==,0))
A_When(n, A_Ret_range(>,0))
A_At(q, A_In_reads(n))
A_At(buf, A_Out A_Post_readable_size(return))
#endif
size_t utf8_to_utf16_unsafe(const utf8_char_t *A_Restrict q, utf16_char_t *const A_Restrict buf, const size_t n);

#ifndef SAL_DEFS_H_INCLUDED
#undef A_Restrict
#endif

#ifdef __cplusplus
}
#endif

#endif /* UTF8_TO_UTF16_H_INCLUDED */
