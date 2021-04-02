#ifndef UTF16_SWAP_H_INCLUDED
#define UTF16_SWAP_H_INCLUDED

/**********************************************************************************
* UTF-16 <-> UTF-32 characters conversion
* Copyright (C) 2021 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_swap.h */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
static inline utf16_char_t utf16_swap_bytes(const utf16_char_t x)
{
#ifdef _MSC_VER
	/* note: #include <stdlib.h> for _byteswap_ushort() */
	return (utf16_char_t)_byteswap_ushort(x);
#else
	/* gcc, clang, icc optimizes this to simple ror, cl - can't */
	return (utf16_char_t)(
		((utf16_char_t)(x & 0xFF00u) >> 8) |
		((utf16_char_t)(x & 0x00FFu) << 8)
	);
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Check_return
#endif
static inline utf32_char_t utf32_swap_bytes(const utf32_char_t x)
{
#ifdef _MSC_VER
	/* note: #include <stdlib.h> for _byteswap_ulong() */
	return (utf16_char_t)_byteswap_ulong(x);
#else
	/* gcc, clang, icc optimizes this to simple bswap, cl - can't */
	return (
		((x & 0xFF000000u) >> 24) |
		((x & 0x00FF0000u) >>  8) |
		((x & 0x0000FF00u) <<  8) |
		((x & 0x000000FFu) << 24)
	);
#endif
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Check_return
A_At(s, A_In)
#endif
static inline utf16_char_t utf16_get_unaligned(const utf16_char_unaligned_t *const s)
{
	utf16_char_t c = 0;
	memcpy(&c, s, sizeof(*s));
	return c;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_Check_return
A_At(s, A_In)
#endif
static inline utf32_char_t utf32_get_unaligned(const utf32_char_unaligned_t *const s)
{
	utf32_char_t c = 0;
	memcpy(&c, s, sizeof(*s));
	return c;
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(d, A_Out)
#endif
static inline void utf16_put_unaligned(utf16_char_unaligned_t *const d, utf16_char_t s)
{
	memcpy(d, &s, sizeof(s));
}

#ifdef SAL_DEFS_H_INCLUDED /* include "sal_defs.h" for the annotations */
A_Nonnull_all_args
A_At(d, A_Out)
#endif
static inline void utf32_put_unaligned(utf32_char_unaligned_t *const d, utf32_char_t s)
{
	memcpy(d, &s, sizeof(s));
}

#ifdef __cplusplus
}
#endif

#endif /* UTF16_SWAP_H_INCLUDED */
