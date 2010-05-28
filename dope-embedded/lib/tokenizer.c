/*
 * \brief   DOpE tokenizer module
 *
 * This module splits a given DOpE command string
 * into its tokens. It returns a table of offsets
 * and lengths of the tokens.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "tokenizer.h"

int init_tokenizer(struct dope_services *d);

/********************************
 ** Functions for internal use **
 ********************************/

/**
 * Returns 1 if the specified character is a number-character
 */
static s16 is_number_char(char c)
{
	if ((c >= '0') && (c <= '9')) return 1;
	if (c == '.') return 1;
	return 0;
}

/**
 * Returns 1 if the specified character is a identifier-character
 */
static s16 is_ident_char(char c)
{
	if ((c >= 'a') && (c <= 'z')) return 1;
	if ((c >= 'A') && (c <= 'Z')) return 1;
	if ((c >= '0') && (c <= '9')) return 1;
	if ( c == '_') return 1;
	return 0;
}

/**
 * Returns type of token in the given string at a given offset
 */
static int token_type(const char *s, u32 offset)
{
	if (!s) return TOKEN_WEIRD;

	/* check if first token character is a 'special' character */
	switch (s[offset]) {
		case '(':
		case ')':
		case '.':
		case ',':
		case '=':
			return TOKEN_STRUCT;
		case 0:
			return TOKEN_EOS;
		case '"':
			return TOKEN_STRING;
		case ' ':
		case '\t':
			return TOKEN_EMPTY;
		case '-':
			if (is_number_char(s[offset+1])) return TOKEN_NUMBER;
			if (is_ident_char(s[offset+1])) return TOKEN_IDENT;
	}

	/* check if first character is a number */
	if (is_number_char(s[offset])) return TOKEN_NUMBER;

	/* check if first character is a identifier-character */
	if (is_ident_char(s[offset])) return TOKEN_IDENT;

	return TOKEN_WEIRD;
}


static int ident_size(const char *s)
{
	int result=1;
	s++;
	while (is_ident_char(*(s++))) result++;
	return result;
}


static int number_size(const char *s)
{
	int result=1;
	s++;
	while (is_number_char(*(s++))) result++;
	return result;
}


static int string_size(const char *s)
{
	int result=1;
	s++;
	while (((*s) != 0) && (*s != '"')) {
		if (*s == '\\') {
			if (*(s+1)=='"') {
				s+=2;
				result+=2;
				continue;
			}
		}
		s++;
		result++;
	}
	if ((*s) == 0) return 1;    /* unclosed string error */
	return result+1;
}


static int token_size(const char *s, u32 offset)
{
	switch (token_type(s,offset)) {
		case TOKEN_STRUCT:  return 1;
		case TOKEN_IDENT:   return ident_size(s+offset);
		case TOKEN_NUMBER:  return number_size(s+offset);
		case TOKEN_STRING:  return string_size(s+offset);
		default:            return 1;
	}
}


static int skip_space(const char *s, u32 offset)
{
	while ((s[offset] == ' ') || (s[offset] == '\t')) offset++;
	return offset;
}


/***********************
 ** Service functions **
 ***********************/

static int parse(const char *s, u32 max_tok, u32 *offbuf, u32 *lenbuf)
{
	u32 num_tok = 0;
	u32 offset  = 0;

	/* go to first token of the string */
	while ((*(s + offset)) != 0) {
		offset = skip_space(s, offset);
		*offbuf = offset;
		*lenbuf = token_size(s, offset);
		offset += *lenbuf;
		lenbuf++;
		offbuf++;
		num_tok++;
	}

	return num_tok;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct tokenizer_services services = {
	parse,
	token_type
};


/************************
 ** Module entry point **
 ************************/

int init_tokenizer(struct dope_services *d)
{
	d->register_module("Tokenizer 1.0",&services);
	return 1;
}
