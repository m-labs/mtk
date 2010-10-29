/*
 * \brief   Standard types and functions used by MTK
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_MTKSTD_H_
#define _MTK_MTKSTD_H_

#define SHOW_INFOS     0
#define SHOW_WARNINGS  1
#define SHOW_ERRORS    1

#include <string.h>


/************************
 ** Types used by mtk **
 ************************/

#define u8  unsigned char
#define s8    signed char
#define u16 unsigned short
#define s16   signed short
#define u32 unsigned long
#define s32   signed long
#define adr unsigned long

#if !defined(NULL)
#define NULL (void *)0
#endif


/******************************
 ** Implemented in mtkstd.c **
 ******************************/

/**
 * Convert a float into a string
 *
 * This function performs zero-termination of the string.
 *
 * \param v       float value to convert
 * \param prec    number of digits after comma
 * \param dst     destination buffer
 * \param max_len destination buffer size
 */
extern int mtk_ftoa(float v, int prec, char *dst, int max_len);


/**
 * Check if two strings are equal
 *
 * This function compares two strings s1 and s2. The length of string s1 can
 * be defined via max_s1.  If max_s1 characters are identical and strlen(s2)
 * equals max_s1, the two strings are considered to be equal. This way, s2
 * can be compared against a substring without null-termination.
 *
 * \param s1      string (length is bounded by max_s1)
 * \param s2      null-terminated second string
 * \param max_s1  max length of string s1
 * \return        1 if the two strings are equal.
 */
extern int mtk_streq(const char *s1, const char *s2, int max_len);

void         *zalloc(unsigned long size);

/*******************************
 ** Debug macros used in mtk **
 *******************************/

/*
 * Within the code of MTK the following macros for filtering
 * debug output are used.
 *
 * INFO    - for presenting status information
 * WARNING - for informing the user about non-serious problems
 * ERROR   - for reporting really worse stuff
 */

#if SHOW_WARNINGS
	#define WARNING(x) x
#else
	#define WARNING(x) /* x */
#endif

#undef INFO
#if SHOW_INFOS
	#define INFO(x) x
#else
	#define INFO(x) /* x */
#endif

#undef ERROR
#if SHOW_ERRORS
	#define ERROR(x) x
#else
	#define ERROR(x) /* x */
#endif


#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

/****************************
 ** MTK service structure **
 ****************************/

/*
 * MTK provides the following service structure to all
 * its components. Via this structure components can
 * access functionality of other components or make an
 * interface available to other components.
 */

struct mtk_services {
	void *(*get_module)      (char *name);
	long  (*register_module) (char *name,void *services);
};

#endif /* _MTK_MTKSTD_H_ */
