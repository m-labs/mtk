/*
 * \brief   Interface of the cache module of DOpE
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_CACHE_H_
#define _DOPE_CACHE_H_

#define CACHE struct cache
struct cache;

struct cache_services {
	CACHE *(*create)      (s32 max_entries,s32 max_size);
	void   (*destroy)     (CACHE *c);
	s32    (*add_elem)    (CACHE *c,void *elem,s32 elemsize,s32 ident,void (*destroy)(void *));
	void  *(*get_elem)    (CACHE *c,s32 index,s32 ident);
	void   (*remove_elem) (CACHE *c,s32 index);
};


#endif /* _DOPE_CACHE_H_ */
