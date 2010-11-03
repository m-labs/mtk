/*
 * \brief   MTK entry function
 *
 * This file describes the startup of MTK. It
 * initialises all needed modules and calls the
 * eventloop (see eventloop.c) of MTK.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "gfx.h"
#include "userstate.h"
#include "screen.h"

static struct gfx_services       *gfx;
static struct screen_services    *screen;
static struct userstate_services *userstate;

extern SCREEN *curr_scr;

/**
 * Prototypes from pool.c
 */
extern int  pool_add(char *name,void *structure);
extern void *pool_get(char *name);

/**
 * Prototypes from 'modules'
 */
extern int init_keymap           (struct mtk_services *);
extern int init_clipping         (struct mtk_services *);
extern int init_scrdrv           (struct mtk_services *);
extern int init_viewman          (struct mtk_services *);
extern int init_widman           (struct mtk_services *);
extern int init_screen           (struct mtk_services *);
extern int init_timer            (struct mtk_services *);
extern int init_tick             (struct mtk_services *);
extern int init_relax            (struct mtk_services *);
extern int init_button           (struct mtk_services *);
extern int init_entry            (struct mtk_services *);
extern int init_edit             (struct mtk_services *);
extern int init_loaddisplay      (struct mtk_services *);
extern int init_variable         (struct mtk_services *);
extern int init_label            (struct mtk_services *);
extern int init_list             (struct mtk_services *);
extern int init_separator        (struct mtk_services *);
extern int init_background       (struct mtk_services *);
extern int init_container        (struct mtk_services *);
extern int init_window           (struct mtk_services *);
extern int init_userstate        (struct mtk_services *);
extern int init_conv_fnt         (struct mtk_services *);
extern int init_conv_tff         (struct mtk_services *);
extern int init_fontman          (struct mtk_services *);
extern int init_gfx              (struct mtk_services *);
extern int init_gfxscr16         (struct mtk_services *);
extern int init_gfximg16         (struct mtk_services *);
extern int init_gfximg32         (struct mtk_services *);
extern int init_cache            (struct mtk_services *);
extern int init_scale            (struct mtk_services *);
extern int init_scrollbar        (struct mtk_services *);
extern int init_frame            (struct mtk_services *);
extern int init_grid             (struct mtk_services *);
extern int init_redraw           (struct mtk_services *);
extern int init_simple_scheduler (struct mtk_services *);
extern int init_hashtable        (struct mtk_services *);
extern int init_tokenizer        (struct mtk_services *);
extern int init_scope            (struct mtk_services *);
extern int init_script           (struct mtk_services *);
extern int init_appman           (struct mtk_services *);
extern int init_winlayout        (struct mtk_services *);
extern int init_messenger        (struct mtk_services *);
extern int init_sharedmem        (struct mtk_services *);
extern int init_clipboard        (struct mtk_services *);

/**
 * Prototypes from eventloop.c
 */
extern void eventloop(struct mtk_services *);

struct mtk_services mtk = {
	pool_get,
	pool_add,
};

int config_transparency  = 0;   /* use translucent effects                */
int config_clackcommit   = 0;   /* deliver commit events on mouse release */
int config_winborder     = 5;   /* size of window resize border           */
int config_menubar       = 0;   /* menubar visibility                     */
int config_dropshadows   = 0;   /* draw dropshadows behind windows        */
int config_adapt_redraw  = 0;   /* adapt redraw to duration time          */

int mtk_init(void *fb, int width, int height)
{
	INFO(char *dbg="Main(init): ");

	/**
	 * init modules
	 */

	INFO(printf("%sSharedMemory\n",dbg));
	init_sharedmem(&mtk);

	INFO(printf("%sTimer\n",dbg));
	init_timer(&mtk);

	INFO(printf("%sTick\n",dbg));
	init_tick(&mtk);

	INFO(printf("%sRelax\n",dbg));
	init_relax(&mtk);

	INFO(printf("%sKeymap\n",dbg));
	init_keymap(&mtk);

	INFO(printf("%sCache\n",dbg));
	init_cache(&mtk);

	INFO(printf("%sHashTable\n",dbg));
	init_hashtable(&mtk);

	INFO(printf("%sClipboard\n",dbg));
	init_clipboard(&mtk);

	INFO(printf("%sApplication Manager\n",dbg));
	init_appman(&mtk);

	INFO(printf("%sTokenizer\n",dbg));
	init_tokenizer(&mtk);

	INFO(printf("%sMessenger\n",dbg));
	init_messenger(&mtk);

	INFO(printf("%sScript\n",dbg));
	init_script(&mtk);

	INFO(printf("%sClipping\n",dbg));
	init_clipping(&mtk);

	INFO(printf("%sScreen Driver\n",dbg));
	init_scrdrv(&mtk);

	INFO(printf("%sViewManager\n",dbg));
	init_viewman(&mtk);

	INFO(printf("%sConvertFNT\n",dbg));
	init_conv_fnt(&mtk);

	INFO(printf("%sConvertTFF\n",dbg));
	init_conv_tff(&mtk);

	INFO(printf("%sFontManager\n",dbg));
	init_fontman(&mtk);

	INFO(printf("%sGfxScreen16\n",dbg));
	init_gfxscr16(&mtk);

	INFO(printf("%sGfxImage16\n",dbg));
	init_gfximg16(&mtk);

	INFO(printf("%sGfxImage32\n",dbg));
	init_gfximg32(&mtk);

	INFO(printf("%sGfx\n",dbg));
	init_gfx(&mtk);

	INFO(printf("%sRedrawManager\n",dbg));
	init_redraw(&mtk);

	INFO(printf("%sUserState\n",dbg));
	init_userstate(&mtk);

	INFO(printf("%sWidgetManager\n",dbg));
	init_widman(&mtk);

	INFO(printf("%sScope\n",dbg));
	init_scope(&mtk);

	INFO(printf("%sButton\n",dbg));
	init_button(&mtk);

	INFO(printf("%sEntry\n",dbg));
	init_entry(&mtk);

	INFO(printf("%sEdit\n",dbg));
	init_edit(&mtk);

	INFO(printf("%sVariable\n",dbg));
	init_variable(&mtk);

	INFO(printf("%sLabel\n",dbg));
	init_label(&mtk);

	INFO(printf("%sLabel\n",dbg));
	init_list(&mtk);

	INFO(printf("%sSeparator\n",dbg));
	init_separator(&mtk);

	INFO(printf("%sLoadDisplay\n",dbg));
	init_loaddisplay(&mtk);

	INFO(printf("%sBackground\n",dbg));
	init_background(&mtk);

	INFO(printf("%sScrollbar\n",dbg));
	init_scrollbar(&mtk);

	INFO(printf("%sScale\n",dbg));
	init_scale(&mtk);

	INFO(printf("%sFrame\n",dbg));
	init_frame(&mtk);

	INFO(printf("%sContainer\n",dbg));
	init_container(&mtk);

	INFO(printf("%sGrid\n",dbg));
	init_grid(&mtk);

	INFO(printf("%sWinLayout\n",dbg));
	init_winlayout(&mtk);

	INFO(printf("%sWindow\n",dbg));
	init_window(&mtk);

	INFO(printf("%sScreen\n",dbg));
	init_screen(&mtk);

	INFO(printf("%sScheduler\n",dbg));
	init_simple_scheduler(&mtk);

	INFO(printf("%screate screen\n",dbg));
	{
		static GFX_CONTAINER *scr_ds;
		gfx       = pool_get("Gfx 1.0");
		screen    = pool_get("Screen 1.0");
		userstate = pool_get("UserState 1.0");

		scr_ds = gfx->alloc_scr(fb, width, height, 16);
		curr_scr = screen->create();
		curr_scr->scr->set_gfx(curr_scr, scr_ds);
		userstate->set_max_mx(gfx->get_width(scr_ds));
		userstate->set_max_my(gfx->get_height(scr_ds));
	}

	return 1;
}
