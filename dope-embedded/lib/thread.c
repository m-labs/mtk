/*
 * \brief   DOpE dummy thread handling module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "thread.h"

int init_thread(struct dope_services *d);


/***********************
 ** Service functions **
 ***********************/

static THREAD *alloc_thread(void) { return NULL; }
static void free_thread(THREAD *t) {}
static void copy_thread(THREAD *src, THREAD *dst) {}
static int start_thread(THREAD *t, void (*entry)(void *), void *arg) { return -1; }
static void kill_thread(THREAD *tid) { }
static MUTEX *create_mutex(int init_locked) { return NULL; }
static void destroy_mutex(MUTEX *m) {}
static void lock_mutex(MUTEX *m) {}
static void unlock_mutex(MUTEX *m) {}
static s8 mutex_is_down(MUTEX *m) { return 0; }
static int ident2thread(const char *ident, THREAD *t) { return 0; }
static int thread_equal(THREAD *t1, THREAD *t2) { return 0; }


/**************************************
 ** Service structure of this module **
 **************************************/

static struct thread_services services = {
	alloc_thread,
	free_thread,
	copy_thread,
	start_thread,
	kill_thread,
	create_mutex,
	destroy_mutex,
	lock_mutex,
	unlock_mutex,
	mutex_is_down,
	ident2thread,
	thread_equal,
};


/************************
 ** Module entry point **
 ************************/

int init_thread(struct dope_services *d)
{
	d->register_module("Thread 1.0",&services);
	return 1;
}
