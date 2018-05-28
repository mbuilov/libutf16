#ifndef UTF16_TO_UTF8_H_INCLUDED
#define UTF16_TO_UTF8_H_INCLUDED

/**********************************************************************************
* UTF-16 -> UTF-8 characters conversion
* Copyright (C) 2018 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_to_utf8.h */

#include "utf16_char.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
#define A_Restrict
#endif

/* convert utf16 0-terminated string to utf8 0-terminated one,
 input:
  w  - address of the pointer to the beginning of input 0-terminated utf16 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf8_char_t's, if zero - output buffer is not used.
 returns number of stored utf8_char_t's, including terminating 0:
  0     - if utf16 string is invalid or too long,
  <= sz - 0-terminated utf8 string was successfully stored in output buffer,
  > sz  - output buffer is too small, return value is the required buffer size to store
   whole converted utf8 0-terminated string, including the part that is already converted
   and stored in output buffer, including 0-terminator, in utf8_char_t's;
 - on success:
  (*w) - points beyond the 0-terminator of input utf16 string,
  (*b) - points beyond the 0-terminator stored in output buffer;
 - if output buffer is too small:
  (*w) - if sz == 0, not changed, else - points beyond the last converted (non-0) utf16_char_t,
  (*b) - if sz == 0, not changed, else - points beyond the last stored (non-0) utf8_char_t;
 - if input utf16 string is invalid or too long:
  (*w) - points beyond the last valid utf16_char_t, if output buffer is too small,
   it may be different from the last successfully converted utf16_char_t,
   if input utf16 string is too long, the last valid utf16_char_t is the 0-terminator,
   if input utf16 string is invalid, the last valid utf16_char_t is _not_ 0;
  (*b) - if sz > 0, points beyond the last stored (non-0) utf8_char_t */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_Success(return)
A_At(w, A_Inout) A_At(*w, A_In_z)
A_When(sz, A_At(b, A_Notnull) A_At(*b, A_Writable_elements(sz)))
#endif
size_t utf16_to_utf8_z(const utf16_char_t **const w, utf8_char_t **const b, size_t sz);

/* convert 'n' utf16_char_t's to utf8 ones,
 input:
  w  - address of the pointer to the beginning of input utf16 string,
  b  - optional address of the pointer to the beginning of output buffer,
  sz - free space in output buffer, in utf8_char_t's, if zero - output buffer is not used,
  n  - number of utf16_char_t's to convert, if zero - input and output buffers are not used.
 returns number of stored utf8_char_t's:
  0     - if 'n' is zero, an invalid utf16 character is encountered or utf16 string is too long,
  <= sz - all 'n' utf16_char_t's were successfully converted to utf8 ones and stored in the output buffer,
  > sz  - output buffer is too small, return value is the required buffer size to store whole converted
   utf8 string, including the part that is already converted and stored in the output buffer, in utf8_char_t's;
 - on success:
  (*w) - points beyond the last source utf16_char_t of input string,
  (*b) - points beyond the last converted utf8_char_t stored in output buffer;
 - if output buffer is too small:
  (*w) - if sz == 0, not changed, else - points beyond the last converted utf16_char_t,
  (*b) - if sz == 0, not changed, else - points beyond the last stored utf8_char_t;
 - if input utf16 string is invalid or too long:
  (*w) - points beyond the last valid utf16_char_t, if output buffer is too small,
   it may be different from the last successfully converted utf16_char_t,
   if input utf16 string is too long, the last valid utf16_char_t is the last character of utf16 string,
   if input utf16 string is invalid, the last valid utf16_char_t is _not_ the last character of utf16 string;
  (*b) - if sz > 0, points beyond the last successfully converted and stored utf8_char_t */
/* Note: zero utf16_char_t is not treated specially, i.e. conversion do not stops */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_arg(1)
A_When(!n, A_Ret_range(==,0))
A_At(w, A_Inout) A_At(*w, A_In_reads(n))
A_When(n && sz, A_At(b, A_Notnull) A_At(*b, A_Writable_elements(sz)))
#endif
size_t utf16_to_utf8(const utf16_char_t **const w, utf8_char_t **const b, size_t sz, const size_t n);

/* convert utf16 0-terminated string to utf8 0-terminated one,
  do not check if there is enough space in output buffer, assume it is large enough.
 returns number of stored utf8_char_t's, including terminating 0:
  0    - if utf16 string is invalid,
  else - stored utf8 string length, including 0-terminator, in utf8_char_t's */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_Success(return)
A_At(w, A_In_z)
A_At(buf, A_Out A_Post_z A_Post_readable_size(return))
#endif
size_t utf16_to_utf8_z_unsafe_out(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf);

/* same as utf16_to_utf8_z_unsafe_out(), but also do not check that input utf16 string is valid */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Ret_range(>,0)
A_At(w, A_In_z)
A_At(buf, A_Out A_Post_z A_Post_readable_size(return))
#endif
size_t utf16_to_utf8_z_unsafe(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf);

/* convert 'n' utf16_char_t's to utf8 ones, (if 'n' is zero - input and output buffers are not used),
  do not check if there is enough space in output buffer, assume it is large enough.
 returns number of stored utf8_char_t's:
  0    - if 'n' is zero or an invalid utf16 character is encountered,
  else - number of stored utf8_char_t's */
/* Note: zero utf16_char_t is not treated specially, i.e. conversion do not stops */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
A_Nonnull_all_args
A_When(!n, A_Ret_range(==,0))
A_At(w, A_In_reads(n))
A_At(buf, A_Out A_Post_readable_size(return))
#endif
size_t utf16_to_utf8_unsafe_out(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf, const size_t n);

/* same as utf16_to_utf8_unsafe_out(), but also do not check that input utf16 string is valid */
#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_When(!n, A_Ret_range(==,0))
A_When(n, A_Ret_range(>,0))
A_At(w, A_In_reads(n))
A_At(buf, A_Out A_Post_readable_size(return))
#endif
size_t utf16_to_utf8_unsafe(const utf16_char_t *A_Restrict w, utf8_char_t *const A_Restrict buf, const size_t n);

#ifndef SAL_DEFS_H_INCLUDED
#undef A_Restrict
#endif

#ifdef __cplusplus
}
#endif

#endif /* UTF16_TO_UTF8_H_INCLUDED */
