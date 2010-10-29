/*
 * \brief   DOpE shared memory management module
 *
 * This implementation is meant be used if DOpE and its client
 * reside in the same address space. Here, shared memory is
 * just allocated via malloc and the textual identifier contains
 * the start address.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "module.h"
#include "sharedmem.h"


struct shared_memory {
	int   fh;
	s32   size;
	char  fname[64];
	void *addr;
};

int init_sharedmem(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Allocate shared memory block of specified size
 */
static SHAREDMEM *shm_alloc(s32 size)
{
	SHAREDMEM *new = malloc(sizeof(SHAREDMEM));
	if (!new) {
		ERROR(printf("SharedMemory(alloc): out of memory.\n"));
		return NULL;
	}
	new->size = size;
	new->addr = malloc(size);
	return new;
}


/**
 * Free shared memory block
 */
static void shm_destroy(SHAREDMEM *sm)
{
	if (!sm) return;
	if (sm->addr) free(sm->addr);
	free(sm);
}


/**
 * Return the adress of the shared memory block
 */
static void *shm_get_adr(SHAREDMEM *sm)
{
	if (!sm) return NULL;
	return sm->addr;
}


/**
 * Generate a global identifier for the specified shared memory block
 */
static void shm_get_ident(SHAREDMEM *sm, char *dst)
{
	if (!sm) return;
	snprintf(dst, 32, "%x", (int)sm->addr);
}

/**************************************
 ** Service structure of this module **
 **************************************/

static struct sharedmem_services sharedmem = {
	shm_alloc,
	shm_destroy,
	shm_get_adr,
	shm_get_ident,
};


/************************
 ** Module entry point **
 ************************/

int init_sharedmem(struct dope_services *d)
{
	d->register_module("SharedMemory 1.0",&sharedmem);
	return 1;
}
