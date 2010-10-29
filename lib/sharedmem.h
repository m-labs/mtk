/*
 * \brief   Interface of shared memory abstraction of DOpE
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_SHAREDMEM_H_
#define _DOPE_SHAREDMEM_H_

#define SHAREDMEM struct shared_memory
struct shared_memory;

struct sharedmem_services {
	SHAREDMEM *(*alloc)       (long size);
	void       (*destroy)     (SHAREDMEM *sm);
	void      *(*get_address) (SHAREDMEM *sm);
	void       (*get_ident)   (SHAREDMEM *sm, char *dst_ident_buf);
};


#endif /* _DOPE_SHAREDMEM_H_ */