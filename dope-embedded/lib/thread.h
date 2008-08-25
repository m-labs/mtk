/*
 * \brief   Interface of the thread abstraction of DOpE
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

#ifndef _DOPE_THREAD_H_
#define _DOPE_THREAD_H_

#if !defined(THREAD)
#define THREAD struct thread
#endif

#if !defined(MUTEX)
#define MUTEX struct mutex
#endif

struct thread;
struct mutex;

struct thread_services {
	THREAD *(*alloc_thread)  (void);
	void    (*free_thread)   (THREAD *);
	void    (*copy_thread)   (THREAD *src, THREAD *dst);
	int     (*start_thread)  (THREAD *dst_tid, void (*entry)(void *), void *arg);
	void    (*kill_thread)   (THREAD *tid);
	MUTEX  *(*create_mutex)  (int init_locked);
	void    (*destroy_mutex) (MUTEX *);
	void    (*mutex_down)    (MUTEX *);
	void    (*mutex_up)      (MUTEX *);
	s8      (*mutex_is_down) (MUTEX *);
	int     (*ident2thread)  (const char *thread_ident, THREAD *dst);
	int     (*thread_equal)  (THREAD *thread, THREAD *another_thread);
};


#endif /* _DOPE_THREAD_H_ */
