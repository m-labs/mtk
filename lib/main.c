/*
 * \brief   DOpE entry function
 *
 * This file describes the startup of DOpE. It
 * initialises all needed modules and calls the
 * eventloop (see eventloop.c) of DOpE.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "scheduler.h"
#include "gfx.h"
#include "userstate.h"
#include "screen.h"

static struct scheduler_services *sched;
static struct gfx_services       *gfx;
static struct screen_services    *screen;
static struct userstate_services *userstate;

extern SCREEN *curr_scr;

/**
 * Prototypes from startup.c (in system dependent directory)
 */
extern void native_startup(int argc, char **argv);

/**
 * Prototypes from pool.c
 */
extern long  pool_add(char *name,void *structure);
extern void *pool_get(char *name);

/**
 * Prototypes from 'modules'
 */
extern int init_keymap           (struct dope_services *);
extern int init_clipping         (struct dope_services *);
extern int init_scrdrv           (struct dope_services *);
extern int init_input            (struct dope_services *);
extern int init_viewman          (struct dope_services *);
extern int init_widman           (struct dope_services *);
extern int init_screen           (struct dope_services *);
extern int init_timer            (struct dope_services *);
extern int init_tick             (struct dope_services *);
extern int init_relax            (struct dope_services *);
extern int init_button           (struct dope_services *);
extern int init_entry            (struct dope_services *);
extern int init_edit             (struct dope_services *);
extern int init_loaddisplay      (struct dope_services *);
extern int init_variable         (struct dope_services *);
extern int init_label            (struct dope_services *);
extern int init_list             (struct dope_services *);
extern int init_separator        (struct dope_services *);
extern int init_background       (struct dope_services *);
extern int init_container        (struct dope_services *);
extern int init_window           (struct dope_services *);
extern int init_userstate        (struct dope_services *);
extern int init_conv_fnt         (struct dope_services *);
extern int init_conv_tff         (struct dope_services *);
extern int init_fontman          (struct dope_services *);
extern int init_gfx              (struct dope_services *);
extern int init_gfxscr16         (struct dope_services *);
extern int init_gfximg16         (struct dope_services *);
extern int init_gfximg32         (struct dope_services *);
extern int init_cache            (struct dope_services *);
extern int init_scale            (struct dope_services *);
extern int init_scrollbar        (struct dope_services *);
extern int init_frame            (struct dope_services *);
extern int init_grid             (struct dope_services *);
extern int init_redraw           (struct dope_services *);
extern int init_simple_scheduler (struct dope_services *);
extern int init_hashtable        (struct dope_services *);
extern int init_tokenizer        (struct dope_services *);
extern int init_scope            (struct dope_services *);
extern int init_script           (struct dope_services *);
extern int init_appman           (struct dope_services *);
extern int init_winlayout        (struct dope_services *);
extern int init_messenger        (struct dope_services *);
extern int init_sharedmem        (struct dope_services *);
extern int init_clipboard        (struct dope_services *);

/**
 * Prototypes from eventloop.c
 */
extern void eventloop(struct dope_services *);

struct dope_services dope = {
	pool_get,
	pool_add,
};

int config_transparency  = 0;   /* use translucent effects                */
int config_clackcommit   = 0;   /* deliver commit events on mouse release */
int config_winborder     = 5;   /* size of window resize border           */
int config_menubar       = 0;   /* menubar visibility                     */
int config_dropshadows   = 0;   /* draw dropshadows behind windows        */
int config_adapt_redraw  = 0;   /* adapt redraw to duration time          */

int dope_main()
{
	INFO(char *dbg="Main(init): ");

	/**
	 * init modules
	 */

	INFO(printf("%sSharedMemory\n",dbg));
	init_sharedmem(&dope);

	INFO(printf("%sTimer\n",dbg));
	init_timer(&dope);

	INFO(printf("%sTick\n",dbg));
	init_tick(&dope);

	INFO(printf("%sRelax\n",dbg));
	init_relax(&dope);

	INFO(printf("%sKeymap\n",dbg));
	init_keymap(&dope);

	INFO(printf("%sCache\n",dbg));
	init_cache(&dope);

	INFO(printf("%sHashTable\n",dbg));
	init_hashtable(&dope);

	INFO(printf("%sClipboard\n",dbg));
	init_clipboard(&dope);

	INFO(printf("%sApplication Manager\n",dbg));
	init_appman(&dope);

	INFO(printf("%sTokenizer\n",dbg));
	init_tokenizer(&dope);

	INFO(printf("%sMessenger\n",dbg));
	init_messenger(&dope);

	INFO(printf("%sScript\n",dbg));
	init_script(&dope);

	INFO(printf("%sClipping\n",dbg));
	init_clipping(&dope);

	INFO(printf("%sScreen Driver\n",dbg));
	init_scrdrv(&dope);

	INFO(printf("%sInput\n",dbg));
	init_input(&dope);

	INFO(printf("%sViewManager\n",dbg));
	init_viewman(&dope);

	INFO(printf("%sConvertFNT\n",dbg));
	init_conv_fnt(&dope);

	INFO(printf("%sConvertTFF\n",dbg));
	init_conv_tff(&dope);

	INFO(printf("%sFontManager\n",dbg));
	init_fontman(&dope);

	INFO(printf("%sGfxScreen16\n",dbg));
	init_gfxscr16(&dope);

	INFO(printf("%sGfxImage16\n",dbg));
	init_gfximg16(&dope);

	INFO(printf("%sGfxImage32\n",dbg));
	init_gfximg32(&dope);

	INFO(printf("%sGfx\n",dbg));
	init_gfx(&dope);

	INFO(printf("%sRedrawManager\n",dbg));
	init_redraw(&dope);

	INFO(printf("%sUserState\n",dbg));
	init_userstate(&dope);

	INFO(printf("%sWidgetManager\n",dbg));
	init_widman(&dope);

	INFO(printf("%sScope\n",dbg));
	init_scope(&dope);

	INFO(printf("%sButton\n",dbg));
	init_button(&dope);

	INFO(printf("%sEntry\n",dbg));
	init_entry(&dope);

	INFO(printf("%sEdit\n",dbg));
	init_edit(&dope);

	INFO(printf("%sVariable\n",dbg));
	init_variable(&dope);

	INFO(printf("%sLabel\n",dbg));
	init_label(&dope);

	INFO(printf("%sLabel\n",dbg));
	init_list(&dope);

	INFO(printf("%sSeparator\n",dbg));
	init_separator(&dope);

	INFO(printf("%sLoadDisplay\n",dbg));
	init_loaddisplay(&dope);

	INFO(printf("%sBackground\n",dbg));
	init_background(&dope);

	INFO(printf("%sScrollbar\n",dbg));
	init_scrollbar(&dope);

	INFO(printf("%sScale\n",dbg));
	init_scale(&dope);

	INFO(printf("%sFrame\n",dbg));
	init_frame(&dope);

	INFO(printf("%sContainer\n",dbg));
	init_container(&dope);

	INFO(printf("%sGrid\n",dbg));
	init_grid(&dope);

	INFO(printf("%sWinLayout\n",dbg));
	init_winlayout(&dope);

	INFO(printf("%sWindow\n",dbg));
	init_window(&dope);

	INFO(printf("%sScreen\n",dbg));
	init_screen(&dope);

	INFO(printf("%sScheduler\n",dbg));
	init_simple_scheduler(&dope);

	INFO(printf("%screate screen\n",dbg));
	{
		static GFX_CONTAINER *scr_ds;
		gfx       = pool_get("Gfx 1.0");
		screen    = pool_get("Screen 1.0");
		userstate = pool_get("UserState 1.0");

		scr_ds = gfx->alloc_scr("default");
		curr_scr = screen->create();
		curr_scr->scr->set_gfx(curr_scr, scr_ds);
		userstate->set_max_mx(gfx->get_width(scr_ds));
		userstate->set_max_my(gfx->get_height(scr_ds));
	}

	INFO(printf("%sstarting scheduler\n",dbg));
	if ((sched  = pool_get("Scheduler 1.0")))
		sched->process_mainloop();

	return 0;
}
