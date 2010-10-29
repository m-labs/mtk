/*
 * \brief   Interface of shared memory abstraction of MTK
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_SHAREDMEM_H_
#define _MTK_SHAREDMEM_H_

#define SHAREDMEM struct shared_memory
struct shared_memory;

struct sharedmem_services {
	SHAREDMEM *(*alloc)       (long size);
	void       (*destroy)     (SHAREDMEM *sm);
	void      *(*get_address) (SHAREDMEM *sm);
	void       (*get_ident)   (SHAREDMEM *sm, char *dst_ident_buf);
};


#endif /* _MTK_SHAREDMEM_H_ */
