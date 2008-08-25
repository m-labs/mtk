/*
 * \brief   Interface of hash table module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_HASHTAB_H_
#define _DOPE_HASHTAB_H_

#define HASHTAB struct hashtab
struct hashtab;

struct hashtab_services {
	HASHTAB *(*create)      (u32 tab_size, u32 max_hash_length);
	void     (*inc_ref)     (HASHTAB *h);
	void     (*dec_ref)     (HASHTAB *h);
	void     (*add_elem)    (HASHTAB *h, char *ident, void *value);
	void    *(*get_elem)    (HASHTAB *h, char *ident, int max_len);
	void     (*remove_elem) (HASHTAB *h, char *ident);
	void    *(*get_first)   (HASHTAB *h);
	void    *(*get_next)    (HASHTAB *h, void *value);
};


#endif /* _DOPE_HASHTAB_H_ */
