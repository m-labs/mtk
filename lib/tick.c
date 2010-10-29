/*
 * \brief   MTK timer tick module
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "timer.h"
#include "tick.h"

#define MAX_TICKS 32

struct tick;
struct tick {
	u32         deadline;            /* next deadline          */
	u32         usec;                /* duration between ticks */
	int       (*callback) (void *);  /* tick callback          */
	void        *arg;                /* callback argument      */
	struct tick *next;               /* next tick in ticklist  */
};

static struct tick ticks[MAX_TICKS];
static struct tick *head = NULL;     /* head of tick list */

static struct timer_services *timer;

int init_tick(struct mtk_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Schedule new tick event
 */
static void queue_tick(struct tick *t)
{
	struct tick *curr;
	
	/* if ticklist is empty add first element */
	if (!head) {
		t->next = 0;
		head    = t;
		return;
	}

	/* if deadline is smaller than any other deadline, put it on the head */
	if (t->deadline < head->deadline) {
		t->next = head;
		head    = t;
		return;
	}

	/* find list element with a higher deadline */
	curr = head;
	while (curr->next && (curr->next->deadline < t->deadline)) curr = curr->next;

	/* if end of list is reached, append new element */
	if (!curr->next) {
		curr->next = t;
		return;
	}

	/* insert element in middle of list */
	t->next    = curr->next;
	curr->next = t;
}


/**
 * Register new timer tick callback routine
 *
 * \param msec      duration between ticks
 * \param callback  routine that is called for every tick
 * \param arg       private argument for callback
 * \return          1 on success
 *
 * The return code of this function should be evaluated!
 */
static int tick_add(s32 msec, int (*callback)(void *), void *arg)
{
	int i;

	/* find empty tick slot */
	for (i=0; i<MAX_TICKS; i++)
		if (ticks[i].callback == NULL) break;

	/* no empty tick slot - return 0 */
	if (i == MAX_TICKS) return 0;

	/* put new tick at the beginning of the ticklist */
	ticks[i].usec     = msec * 1000;
	ticks[i].deadline = timer->get_time() + ticks[i].usec;
	ticks[i].callback = callback;
	ticks[i].arg      = arg;
	queue_tick(&ticks[i]);

	return 1;
}


/**
 * Check for due tick events and execute callbacks
 */
static void tick_handle(void)
{
	u32 now = timer->get_time();
	struct tick *curr = head;
	
	while ((curr = head) && ((s32)now > (s32)head->deadline)) {
		void *arg = head->arg;

		/* remove tick from head of the list */
		head = curr->next;
		curr->next = NULL;

		if (curr->callback(arg)) {
			/* tick is still valid - schedule next event */
			curr->deadline = now + curr->usec;
			queue_tick(curr);
		} else {
			/* mark tick slot as free */
			curr->callback = NULL;
		}
	}
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct tick_services services = {
	tick_add,
	tick_handle,
};


/************************
 ** Module entry point **
 ************************/

int init_tick(struct mtk_services *mtk)
{
	timer     = mtk->get_module("Timer 1.0");
	
	mtk->register_module("Tick 1.0",&services);
	return 1;
}
