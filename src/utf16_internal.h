#ifndef UTF16_INTERNAL_H_INCLUDED
#define UTF16_INTERNAL_H_INCLUDED

/**********************************************************************************
* UTF-16 <-> UTF-32 characters conversion
* Copyright (C) 2021 Michael M. Builov, https://github.com/mbuilov/libutf16
* Licensed under Apache License v2.0, see LICENSE.TXT
**********************************************************************************/

/* utf16_internal.h */

#ifdef SWAP_UTF16
# ifdef UTF_GET_UNALIGNED
#  define UTF16_GET(s)      utf16_swap_bytes(utf16_get_unaligned(s))
# else
#  define UTF16_GET(s)      utf16_swap_bytes(*(s))
# endif
# ifdef UTF_PUT_UNALIGNED
#  define UTF16_PUT(d, s)   utf16_put_unaligned(d, utf16_swap_bytes(s))
# else
#  define UTF16_PUT(d, s)   (*(d) = utf16_swap_bytes(s))
# endif
#else /* !SWAP_UTF16 */
# ifdef UTF_GET_UNALIGNED
#  define UTF16_GET(s)      utf16_get_unaligned(s)
# else
#  define UTF16_GET(s)      (*(s))
# endif
# ifdef UTF_PUT_UNALIGNED
#  define UTF16_PUT(d, s)   utf16_put_unaligned(d, s)
# else
#  define UTF16_PUT(d, s)   (*(d) = (s))
# endif
#endif /* !SWAP_UTF16 */

#ifdef SWAP_UTF32
# ifdef UTF_GET_UNALIGNED
#  define UTF32_GET(s)      utf32_swap_bytes(utf32_get_unaligned(s))
# else
#  define UTF32_GET(s)      utf32_swap_bytes(*(s))
# endif
# ifdef UTF_PUT_UNALIGNED
#  define UTF32_PUT(d, s)   utf32_put_unaligned(d, utf32_swap_bytes(s))
# else
#  define UTF32_PUT(d, s)   (*(d) = utf32_swap_bytes(s))
# endif
#else /* !SWAP_UTF32 */
# ifdef UTF_GET_UNALIGNED
#  define UTF32_GET(s)      utf32_get_unaligned(s)
# else
#  define UTF32_GET(s)      (*(s))
# endif
# ifdef UTF_PUT_UNALIGNED
#  define UTF32_PUT(d, s)   utf32_put_unaligned(d, s)
# else
#  define UTF32_PUT(d, s)   (*(d) = (s))
# endif
#endif /* !SWAP_UTF32 */

/* for generation of function names */

#ifdef SWAP_UTF16
# define UTF16_X x
#else
# define UTF16_X
#endif

#ifdef SWAP_UTF32
# define UTF32_X x
#else
# define UTF32_X
#endif

#ifdef UTF_GET_UNALIGNED
# define UTF_GET_U u
#else
# define UTF_GET_U
#endif

#ifdef UTF_PUT_UNALIGNED
# define UTF_PUT_U u
#else
# define UTF_PUT_U
#endif

#endif /* UTF16_INTERNAL_H_INCLUDED */
