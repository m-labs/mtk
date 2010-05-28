/*
 * \brief   Interface of the real-time scheduler of DOpE
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_SCHEDULER_H_
#define _DOPE_SCHEDULER_H_

#include "widget.h"
#include "thread.h"

struct scheduler_services {

	/**
	 * Schedule new real-time widget
	 *
	 * \param w       widget to redraw periodically
	 * \param period  period length in milliseconds
	 * \return        0 on success
	 */
	s32  (*add) (WIDGET *w, u32 period);


	/**
	 * Stop real-time redraw of specified widget
	 */
	void (*remove) (WIDGET *w);


	/**
	 * Stop real-time redraw of all widgets of specified application
	 */
	void (*release_app) (int app_id);


	/**
	 * Register synchronisation mutex
	 *
	 * After each periodic redraw, the scheduler unlocks
	 * this semaphore. This mechanism can be used to provide
	 * redraw-synchronisation with a client application.
	 */
	void (*set_sync_mutex) (WIDGET *w, MUTEX *);


	/**
	 * Mainloop of dope
	 *
	 * Within the mainloop we must update real-time widgets,
	 * handle non-realtime redraw operations and pay some
	 * attention to the user.
	 */
	void (*process_mainloop) (void);
};


#endif /* _DOPE_SCHEDULER_H_ */
