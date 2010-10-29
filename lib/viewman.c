/*
 * \brief   MTK view manager
 *
 * This is just a dummy implementation that is used for MTK
 * as standalone window server.
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "viewman.h"

int init_viewman(struct mtk_services *d);


/***********************
 ** Service functions **
 ***********************/

/**
 * Create a new view
 *
 * \return  view id of the new view or a negative error code
 */
static struct view *view_create(void)
{
	return NULL;
}


/**
 * Destroy view
 */
static void view_destroy(struct view *v)
{ }


/**
 * Position view
 *
 * \return  0 on success or a negative error code
 */
static int view_place(struct view *v, int x, int y, int w, int h)
{
	return 0;
}


/**
 * Bring view on top
 *
 * \return  0 on success or a negative error code
 */
static int view_top(struct view *v)
{
	return 0;
}


/**
 * Bring view on back
 *
 * \return  0 on success or a negative error code
 */
static int view_back(struct view *v)
{
	return 0;
}


/**
 * Set title of a view
 *
 * \return  0 on success or a negative error code
 */
static int view_set_title(struct view *v, const char *title)
{
	return 0;
}


/**
 * Define background view
 *
 * \return  0 on success or a negative error code
 */
static int view_set_bg(struct view *v)
{
	return 0;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct viewman_services services = {
	view_create,
	view_destroy,
	view_place,
	view_top,
	view_back,
	view_set_title,
	view_set_bg,
};


/************************
 ** Module entry point **
 ************************/

int init_viewman(struct mtk_services *d)
{
	d->register_module("ViewManager 1.0", &services);
	return 1;
}
