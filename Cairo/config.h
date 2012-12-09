/* config.h for Cairo to be compiled with Netlab's GCC compiler */

/* Make sure we'll use Intel's atomic inc, dec and test and cmpxchg */
/* Update: it's undef'd now, as it does not seem to compile... */
#undef CAIRO_HAS_INTEL_ATOMIC_PRIMITIVES

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <pthread.h> header file. */
#undef HAVE_PTHREAD_H

/* Define to 1 if you have the `sincos' function. */
#define HAVE_SINCOS

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
/* Modification: Just simply define, and don't define it to be 1! */
/* This is needed so libJPEG can be compiled with this config.h too! */
#define HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/int_types.h> header file. */
#undef HAVE_SYS_INT_TYPES_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if the system has the type `uint128_t'. */
#undef HAVE_UINT128_T

/* Define to 1 if the system has the type `uint64_t'. */
#define HAVE_UINT64_T 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vasnprintf' function. */
#undef HAVE_VASNPRINTF

/* Name of package */
#define PACKAGE "Cairo"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugs.freedesktop.org/enter_bug.cgi?product=cairo"

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
//#undef PACKAGE_VERSION

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Version number of package */
#undef VERSION

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

/* Define to 1 if the X Window System is missing or not being used. */
#undef X_DISPLAY_MISSING


/* Some defines for FontConfig */

/* FT_Bitmap_Size structure includes y_ppem field */
#define HAVE_FT_BITMAP_SIZE_Y_PPEM 1

/* Define to 1 if you have the `FT_Get_BDF_Property' function. */
#define HAVE_FT_GET_BDF_PROPERTY 1

/* Define to 1 if you have the `FT_Get_Next_Char' function. */
#define HAVE_FT_GET_NEXT_CHAR 1

/* Define to 1 if you have the `FT_Get_PS_Font_Info' function. */
#define HAVE_FT_GET_PS_FONT_INFO 1

/* Define to 1 if you have the `FT_Get_X11_Font_Format' function. */
#define HAVE_FT_GET_X11_FONT_FORMAT 1

/* Define to 1 if you have the `FT_Has_PS_Glyph_Names' function. */
#define HAVE_FT_HAS_PS_GLYPH_NAMES 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1
