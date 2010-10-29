/*
 * \brief   Interface of tokenizer module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_TOKENIZER_H_
#define _DOPE_TOKENIZER_H_

#define TOKEN_EMPTY     0
#define TOKEN_IDENT     1       /* identifier */
#define TOKEN_STRUCT    2       /* structure element (brackets etc.) */
#define TOKEN_STRING    3       /* string */
#define TOKEN_WEIRD     4       /* weird */
#define TOKEN_NUMBER    5       /* number */
#define TOKEN_EOS       99      /* end of string */

struct tokenizer_services {
	int (*parse)  (const char *str, u32 max_tok, u32 *offbuf, u32 *lenbuf);
	int (*toktype)(const char *str, u32 offset);
};

#endif /* _DOPE_TOKENIZER_H_ */
