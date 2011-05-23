/*
 * \brief   MTK widget base class module
 *
 * This module implements common functionality of all widgets.
 *
 * For more detailed information about the widget interface
 * functions please refer the header file widget.h
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mtkstd.h"
#include "widget.h"
#include "widget_data.h"
#include "widget_help.h"
#include "widman.h"
#include "messenger.h"
#include "redraw.h"
#include "script.h"
#include "appman.h"
#include "list_macros.h"
#include "window.h"
#include "mtkeycodes.h"
#include "userstate.h"

static struct redraw_services    *redraw;
static struct script_services    *script;
static struct appman_services    *appman;
static struct userstate_services *userstate;
static struct messenger_services *msg;

int init_widman(struct mtk_services *d);


/***********************************************************
 ** Default implementations for widget base class methods **
 ***********************************************************/

/**
 * Free data that is referenced by a widget
 */
static void wid_free_data(WIDGET *w)
{}


/**
 * Free single binding data struct
 */
static inline void free_binding(struct binding *b)
{
	if (b->bind_ident)
		free(b->bind_ident);
	if (b->msg)
		free(b->msg);
	free(b);
}


/**
 * Deallocate binding data structure
 */
static void free_new_binding(struct new_binding *b)
{
	int i;
	if (b->name)
		free(b->name);

	for (i=0; b->args[i]; i++) free(b->args[i]);
	free(b->args);

	for (i=0; b->cond_names[i]; i++) free(b->cond_names[i]);
	free(b->cond_names);

	for (i=0; b->cond_values[i]; i++) free(b->cond_values[i]);
	free(b->cond_values);

	free(b);
}


/**
 * Get widget type
 */
static char * wid_get_type(WIDGET *w)
{
	return "unknown";
}


/**
 * Get/set application id of the owner of the widget
 */
static s32 wid_get_app_id(WIDGET *w)
{
	return w->wd->app_id;
}
static void wid_set_app_id(WIDGET *w, s32 new_app_id)
{
	w->wd->app_id = new_app_id;
}


/**
 * Get/set widget position relative to its parent
 */
static int wid_get_x(WIDGET *w)
{
	return w->wd->x;
}
static void wid_set_x(WIDGET *w,int new)
{
	w->wd->x = new;
}
static int wid_get_y(WIDGET *w)
{
	return w->wd->y;
}
static void wid_set_y(WIDGET *w,int new)
{
	w->wd->y = new;
}


/**
 * Get/set widget size
 */
static int wid_get_w(WIDGET *w)
{
	return w->wd->w;
}
static void wid_set_w(WIDGET *w,int new)
{
	if (new < w->wd->min_w) new = w->wd->min_w;
	if (new > w->wd->max_w) new = w->wd->max_w;
	if (w->wd->w != new)
		w->wd->update |= WID_UPDATE_SIZE;
	w->wd->w = new;
}
static int wid_get_h(WIDGET *w)
{
	return w->wd->h;
}
static void wid_set_h(WIDGET *w,int new)
{
	if (new < w->wd->min_h) new = w->wd->min_h;
	if (new > w->wd->max_h) new = w->wd->max_h;
	if (w->wd->h != new)
		w->wd->update |= WID_UPDATE_SIZE;
	w->wd->h = new;
}


/**
 * Get minimal/maximal size of a widget
 */
static int wid_get_min_w(WIDGET *w)
{
	return w->wd->min_w;
}
static int wid_get_min_h(WIDGET *w)
{
	return w->wd->min_h;
}
static int wid_get_max_w(WIDGET *w)
{
	return w->wd->max_w;
}
static int wid_get_max_h(WIDGET *w)
{
	return w->wd->max_h;
}


/**
 * Calculate current min/max boundaries of the widget
 */
static void wid_calc_minmax(WIDGET *w)
{
}


/**
 * Determine absolute position of the widget on the screen
 */
static int wid_get_abs_x(WIDGET *w)
{
	int x=0;
	while ((w!=NULL) && (w!=(WIDGET *)'#')) {
		x+=w->wd->x;
		w=w->wd->parent;
	}
	return x;
}
static int wid_get_abs_y(WIDGET *w)
{
	int y=0;
	while ((w!=NULL) && (w!=(WIDGET *)'#')) {
		y+=w->wd->y;
		w=w->wd->parent;
	}
	return y;
}


/**
 * Get/set widget state (selected or not)
 */
static int wid_get_state(WIDGET *w)
{
	return w->wd->flags & WID_FLAGS_STATE;
}
static void wid_set_state(WIDGET *w,int new)
{
	if (new) w->wd->flags = w->wd->flags | WID_FLAGS_STATE;
	else w->wd->flags = w->wd->flags & ~WID_FLAGS_STATE;
	w->wd->update |= WID_UPDATE_REFRESH;
}


/**
 * Get/set focus flag (mouse over widget)
 */
static int wid_get_mfocus(WIDGET *w)
{
	return w->wd->flags & WID_FLAGS_MFOCUS;
}
static void wid_set_mfocus(WIDGET *w, int new)
{
	if (new) w->wd->flags = w->wd->flags | WID_FLAGS_MFOCUS;
	else w->wd->flags = w->wd->flags & ~WID_FLAGS_MFOCUS;
	w->wd->update |= WID_UPDATE_REFRESH;
}


/**
 * Get/set keyboard focus flag
 */
static int wid_get_kfocus(WIDGET *w)
{
	return w->wd->flags & WID_FLAGS_MFOCUS;
}
static void wid_set_kfocus(WIDGET *w, int new)
{
	if (new) w->wd->flags = w->wd->flags | WID_FLAGS_KFOCUS;
	else w->wd->flags = w->wd->flags & ~WID_FLAGS_KFOCUS;
	w->wd->update |= WID_UPDATE_REFRESH;
}


/**
 * Request first widget of a subtree that can take the keyboard focus
 *
 * This implementation is only valid for leaf widgets.
 * Layout widget need a custom implementation of this function.
 */
static WIDGET *wid_first_kfocus(WIDGET *w)
{
	if (w->wd->flags & WID_FLAGS_TAKEFOCUS) return w;
	return NULL;
}


/**
 * Make widget the current keyboard focus of its window
 */
static void wid_focus(WIDGET *w)
{
	WINDOW *win = (WINDOW *)w->gen->get_window(w);
	if (!win) return;
	win->win->set_kfocus(win, w);
	userstate->set_active_window(win, 0);
}


/**
 * Get/set event forwarding flag (propagation to parents)
 */
static int wid_get_evforward(WIDGET *w)
{
	return w->wd->flags & WID_FLAGS_EVFORWARD;
}
static void wid_set_evforward(WIDGET *w,int new)
{
	if (new) w->wd->flags = w->wd->flags | WID_FLAGS_EVFORWARD;
	else w->wd->flags = w->wd->flags & ~WID_FLAGS_EVFORWARD;
}


/**
 * Get/set current parent of widget
 */
static WIDGET * wid_get_parent(WIDGET *w)
{
	return w->wd->parent;
}
static void wid_set_parent(WIDGET *w, void *new_parent)
{
	/* sanity check - avoid parent relationship with itself */
	if (w == new_parent) return;

	userstate->release_widget(w);

	w->wd->parent = new_parent;
}


/**
 * Get/set pointer to associated 'user' data
 */
static void *wid_get_context(WIDGET *w)
{
	return w->wd->context;
}
static void wid_set_context(WIDGET *w, void *c)
{
	w->wd->context = c;
}


/**
 * Get/set next/previous element when the widget is used in a connected list
 */
static WIDGET * wid_get_next(WIDGET *w)
{
	return w->wd->next;
}
static void wid_set_next(WIDGET *w, WIDGET *n)
{
	w->wd->next = n;
}
static WIDGET * wid_get_prev(WIDGET *w)
{
	return w->wd->prev;
}
static void wid_set_prev(WIDGET *w, WIDGET *p)
{
	w->wd->prev = p;
}


/**
 * Get/set grabfocus flag
 */
static int wid_get_grabfocus(WIDGET *w)
{
	return !!(w->wd->flags & WID_FLAGS_GRABFOCUS);
}
static void wid_set_grabfocus(WIDGET *w, int grabfocus_flag)
{
	if (grabfocus_flag) w->wd->flags |= WID_FLAGS_GRABFOCUS;
	else w->wd->flags &= ~WID_FLAGS_GRABFOCUS;
}


/**
 * Get/set event selectable flag
 */
static int wid_get_selectable(WIDGET *w)
{
	return w->wd->flags & WID_FLAGS_SELECTABLE;
}
static void wid_set_selectable(WIDGET *w,int new_sel)
{
	if (new_sel) w->wd->flags = w->wd->flags | WID_FLAGS_SELECTABLE;
	else w->wd->flags = w->wd->flags & ~WID_FLAGS_SELECTABLE;
	w->wd->update |= WID_UPDATE_REFRESH;
}


/**
 * Draw a widget (dummy - must be overwritten by something more useful
 */
static int wid_draw(WIDGET *w, struct gfx_ds *ds, int x, int y, WIDGET *origin)
{
	w = w; x = x; y = y;    /* just to avoid warnings */
	return 0;
}


/**
 * Cause the redraw of a specified widget area
 */
static int wid_drawarea(WIDGET *cw, WIDGET *origin, int x, int y, int w, int h)
{
	WIDGET *parent = cw->gen->get_parent(cw);
	if (!parent) return 0;

	/* shrink the effective drawing area to the visible part */
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if (x + w > cw->wd->w) w = cw->wd->w - x;
	if (y + h > cw->wd->h) h = cw->wd->h - y;

	/* determine position of the area relative to the parent */
	x += cw->wd->x;
	y += cw->wd->y;

	return parent->gen->drawarea(parent, origin, x, y, w, h);
}


/**
 * Cause the redraw of the window behind a specified widget area
 */
static int wid_drawbehind(WIDGET *cw, WIDGET *child,
                          int x, int y, int w, int h, WIDGET *origin) {
	WIDGET *parent = cw->gen->get_parent(cw);
	if (!parent) return 0;

	/* shrink the effective drawing area to the visible part */
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if (x + w > cw->wd->w) w = cw->wd->w - x;
	if (y + h > cw->wd->h) h = cw->wd->h - y;

	/* determine position of the area relative to the parent */
	x += cw->wd->x;
	y += cw->wd->y;

	return parent->gen->drawbehind(parent, cw, x, y, w, h, origin);
}


/**
 * Draw background of widget
 */
static int wid_draw_bg(WIDGET *cw, struct gfx_ds *ds, int x, int y,
                       int w, int h, WIDGET *origin, int opaque ) {

	/* sanity check */
	if (w <= 0 || h <= 0 || !cw->wd->parent) return 0;

	/* propagate background drawing request to the parent */
	return cw->wd->parent->gen->draw_bg(cw->wd->parent, ds, x, y, w, h, origin, opaque);
}


/**
 * Find widget inside a widget at the given position (relative to parent)
 */
static WIDGET *wid_find(WIDGET *w,int x,int y)
{
	if (w) {
		if ((x >= w->wd->x) && (y >= w->wd->y) &&
			(x < w->wd->x+w->wd->w) && (y < w->wd->y+w->wd->h)) {
			return w;
		}
	}
	return NULL;
}


/**
 * Determine the window in where the widget is
 */
static WIDGET *wid_get_window(WIDGET *cw)
{
	WIDGET *win = NULL;

	/* find root parent, which is a screen */
	while (cw->wd->parent) {
		if (cw->wd->parent->gen->is_root(cw->wd->parent)) win = cw;
		cw = cw->wd->parent;
	}
	return win;
}


/**
 * Update widgets when attributes changed
 */
static void wid_update(WIDGET *w)
{
	int old_min_w = w->wd->min_w;
	int old_max_w = w->wd->max_w;
	int old_min_h = w->wd->min_h;
	int old_max_h = w->wd->max_h;

	/*
	 * If wid_update is called from within the destroy or free_data
	 * function (for example, if free_data calls the remove_child
	 * function) we must ignore it.
	 */
	if (w->wd->ref_cnt == 0) return;

	w->gen->calc_minmax(w);

	if (w->wd->min_w != old_min_w || w->wd->max_w != old_max_w
	 || w->wd->min_h != old_min_h || w->wd->max_h != old_max_h)
		w->wd->update |= WID_UPDATE_MINMAX;

	if (w->wd->update & WID_UPDATE_MINMAX) {
		WIDGET *parent = w->gen->get_parent((WIDGET *)w);
		if (parent) parent->gen->do_layout(parent, w);
	}
	if (w->wd->update) {
		w->gen->updatepos(w);
		w->gen->force_redraw(w);
	}
	w->wd->update = 0;
}


/**
 * Update widget size and position
 */
static void wid_updatepos(WIDGET *w)
{
	w->wd->update &= ~WID_UPDATE_SIZE;
}


/**
 * Inc/decrement reference counter of widget
 */
static void wid_inc_ref(WIDGET *w)
{
	w->wd->ref_cnt++;
}
static void wid_dec_ref(WIDGET *w)
{
	w->wd->ref_cnt--;
	if (w->wd->ref_cnt > 0) return;

	/* the widget is not referenced anymore - destroy is */
	INFO(printf("widman(dec_ref): %s %p ref_cnt reached zero -> commit suicide\n",
	     w->gen->get_type(w), w));

	userstate->release_widget(w);

	/* free widget type specific data */
	w->gen->free_data(w);

	/* free bindings */
	FREE_CONNECTED_LIST(struct binding, w->wd->bindings, free_binding);
	FREE_CONNECTED_LIST(struct new_binding, w->wd->new_bindings, free_new_binding);

	/* free widget struct */
	free(w);
}


/**
 * Cause a redraw of the widget
 */
static void wid_force_redraw(WIDGET *cw)
{
	redraw->draw_widget(cw);
}


/**
 * Handle events
 */
static void wid_handle_event(WIDGET *cw,EVENT *e, WIDGET *from)
{
	struct binding *cb;
	s16 propagate = 1;

	/* tell the parent to switch the keyboard focus */
	if ((e->type == EVENT_PRESS)
	 && (e->code == MTK_KEY_TAB)
	 && !(cw->wd->flags & WID_FLAGS_GRABFOCUS)) {
		if (cw->wd->parent)
			cw->wd->parent->gen->handle_event(cw->wd->parent, e, cw);
		return;
	}
	
	/* propagate mouse wheel events */
	if((e->type == EVENT_PRESS) && ((e->code == MTK_BTN_GEAR_UP) || (e->code == MTK_BTN_GEAR_DOWN))) {
		if (cw->wd->parent)
			cw->wd->parent->gen->handle_event(cw->wd->parent, e, cw);
	}

	/* check bindings */
	for (cb = cw->wd->bindings; cb; cb = cb->next) {
		if (cb->ev_type == e->type) {
			msg->send_input_event(cw->wd->app_id,e,cb->msg);
			propagate = 0;
		}
	}

	/* propagate event to parent widget by default */
	if ((cw->wd->flags & WID_FLAGS_EVFORWARD) && (propagate) &&
	    (cw->wd->parent) && (cw->wd->parent!=(WIDGET *)'#')) {
		cw->wd->parent->gen->handle_event(cw->wd->parent, e, cw);
	}
}


/**
 * Do the layout inside a widget
 */
static int wid_do_layout(WIDGET *cw,WIDGET *child)
{
	int w,h;

	/* check if size of child is in its valid min/max range */
	w = child->wd->w;
	h = child->wd->h;

	if (w <= child->wd->min_w) w = child->wd->min_w;
	if (w >= child->wd->max_w) w = child->wd->max_w;
	if (h <= child->wd->min_h) h = child->wd->min_h;
	if (h >= child->wd->max_h) h = child->wd->max_h;

	child->gen->set_w(child, w);
	child->gen->set_h(child, h);

	if (child->wd->update & WID_UPDATE_SIZE) child->gen->updatepos(child);

	cw->wd->update |= WID_UPDATE_MINMAX;
	cw->gen->update(cw);

	/*
	 * return 1 -> we didnt made a redraw - the caller has to do this
	 * return 0 -> we made a redraw - the caller dont has to do anything more
	 */

	return 0;
}


///**
// * Attach event binding to widget
// */
//static void wid_new_bind(WIDGET *cw, struct new_binding *new) {
//	struct new_binding *b;
//
//	/* check if there exists a binding with the same name */
//	b = cw->wd->new_bindings;
//	while (b) {
//		if (mtk_streq(b->name, new->name, 255)) {
//			REMOVE_LIST_ELEMENT(struct new_binding, cw->wd->new_bindings, b, free_new_binding);
//			break;
//		}
//		b = b->next;
//	}
//
//	/* attach new binding */
//
//}


/**
 * Add event binding for a widget
 */
static void wid_bind(WIDGET *cw,char *bind_ident,char *message)
{
	struct binding *new;

	INFO(printf("Widman(bind): create new binding for %s\n",bind_ident);)
	new = (struct binding *)zalloc(sizeof(struct binding));
	if (!new) {
		ERROR(printf("WidgetManager(bind): out of memory!\n");)
		return;
	}

	new->msg  = strdup(message);
	new->next = cw->wd->bindings;
	new->bind_ident  = strdup(bind_ident);
	cw->wd->bindings = new;

	if (mtk_streq(bind_ident, "press",     6)) {new->ev_type = EVENT_PRESS;       }
	if (mtk_streq(bind_ident, "release",   8)) {new->ev_type = EVENT_RELEASE;     }
	if (mtk_streq(bind_ident, "motion",    7)) {new->ev_type = EVENT_MOTION;      }
	if (mtk_streq(bind_ident, "leave",     6)) {new->ev_type = EVENT_MOUSE_LEAVE; }
	if (mtk_streq(bind_ident, "enter",     6)) {new->ev_type = EVENT_MOUSE_ENTER; }
	if (mtk_streq(bind_ident, "keyrepeat", 9)) {new->ev_type = EVENT_KEY_REPEAT;  }

	if (new->ev_type == 0)
		new->ev_type = EVENT_ACTION;

	/* enable widget to receive the keyboard focus */
	if (new->ev_type == EVENT_PRESS
	 || new->ev_type == EVENT_RELEASE
	 || new->ev_type == EVENT_KEY_REPEAT) {

		/* only consider selectable or editable widgets to take the focus */
		if ((cw->wd->flags & (WID_FLAGS_SELECTABLE | WID_FLAGS_EDITABLE))
		 && !(cw->wd->flags & WID_FLAGS_TAKEFOCUS)) {
			cw->wd->flags |= WID_FLAGS_TAKEFOCUS;

			/*
			 * If we enable the widget to take the focus its properties
			 * may change. For example, a button needs a padding to draw
			 * the focus frame.
			 */
			cw->gen->update(cw);
		}
	}
}


/**
 * Remove event binding from a widget
 */
static void wid_unbind(WIDGET *cw, char *bind_ident)
{
	struct binding *b = cw->wd->bindings;

	/* search for binding to remove */
	for (; b && b->next && mtk_streq(b->next->bind_ident, bind_ident, 16); b = b->next);

	/* remove binding from binding list of the widget */
	if (!b) return;
	REMOVE_LIST_ELEMENT(struct binding, &cw->wd->bindings, b, free_binding);
}


/**
 * Check if widget is bound to a certain event
 */
static char *wid_get_bind_msg(WIDGET *cw, char *bind_ident)
{
	struct binding *cb;

	for (cb = cw->wd->bindings; cb; cb = cb->next)
		if (mtk_streq(cb->bind_ident,bind_ident, 256))
			return cb->msg;

	return NULL;
}


/**
 * Determine if widget is a root widget
 */
static int wid_is_root(WIDGET *cw)
{
	return 0;
}

/**
 * Remove child widget from widget
 */
static void wid_remove_child(WIDGET *w, WIDGET *child)
{ }


/**
 * Break with parent - let's be free
 */
static void wid_release(WIDGET *w)
{
	if (!w->wd->parent) return;
	w->wd->parent->gen->remove_child(w->wd->parent, w);
}


/**
 * Check if w1 has a parent relationship to w2
 */
static int wid_related_to(WIDGET *w1, WIDGET *w2)
{
	WIDGET *cw;

	/* if both widgets are the same they are obviously related */
	if (w1 == w2) return 1;

	/* check if w2 is a parent of w1 */
	for (cw = w2; cw; cw = cw->gen->get_parent(cw))
		if (cw == w1) return 1;

	return 0;
}


/***********************
 ** Service functions **
 ***********************/

/**
 * Set general widget attributes to default values
 */
static void default_widget_data(struct widget_data *d)
{
	d->x = d->y = d->w = d->h = 0;
	d->min_w   = 0;
	d->min_h   = 0;
	d->max_w   = 10000;
	d->max_h   = 10000;
	d->update  = 0;
	d->next    = NULL;
	d->prev    = NULL;
	d->flags   = WID_FLAGS_EVFORWARD;
	d->context = (void *)0;
	d->parent  = (void *)0;
	d->click   = (void *)0;
	d->ref_cnt = 1;
	d->app_id  = -1;
	d->bindings= 0;
}


/**
 * Set general widget methods to default methods
 */
static void default_widget_methods(struct widget_methods *m)
{
	m->free_data      = wid_free_data;
	m->get_type       = wid_get_type;
	m->get_app_id     = wid_get_app_id;
	m->set_app_id     = wid_set_app_id;
	m->get_x          = wid_get_x;
	m->set_x          = wid_set_x;
	m->get_y          = wid_get_y;
	m->set_y          = wid_set_y;
	m->get_w          = wid_get_w;
	m->set_w          = wid_set_w;
	m->get_h          = wid_get_h;
	m->set_h          = wid_set_h;
	m->get_min_w      = wid_get_min_w;
	m->get_min_h      = wid_get_min_h;
	m->get_max_w      = wid_get_max_w;
	m->get_max_h      = wid_get_max_h;
	m->get_abs_x      = wid_get_abs_x;
	m->get_abs_y      = wid_get_abs_y;
	m->get_state      = wid_get_state;
	m->set_state      = wid_set_state;
	m->get_evforward  = wid_get_evforward;
	m->set_evforward  = wid_set_evforward;
	m->get_parent     = wid_get_parent;
	m->set_parent     = wid_set_parent;
	m->get_context    = wid_get_context;
	m->set_context    = wid_set_context;
	m->get_next       = wid_get_next;
	m->set_next       = wid_set_next;
	m->get_prev       = wid_get_prev;
	m->set_prev       = wid_set_prev;
	m->get_selectable = wid_get_selectable;
	m->set_selectable = wid_set_selectable;
	m->draw           = wid_draw;
	m->update         = wid_update;
	m->updatepos      = wid_updatepos;
	m->find           = wid_find;
	m->inc_ref        = wid_inc_ref;
	m->dec_ref        = wid_dec_ref;
	m->force_redraw   = wid_force_redraw;
	m->get_mfocus     = wid_get_mfocus;
	m->set_mfocus     = wid_set_mfocus;
	m->get_kfocus     = wid_get_kfocus;
	m->set_kfocus     = wid_set_kfocus;
	m->get_grabfocus  = wid_get_grabfocus;
	m->set_grabfocus  = wid_set_grabfocus;
	m->focus          = wid_focus;
	m->first_kfocus   = wid_first_kfocus;
	m->handle_event   = wid_handle_event;
	m->get_window     = wid_get_window;
	m->do_layout      = wid_do_layout;
	m->bind           = wid_bind;
	m->unbind         = wid_unbind;
	m->get_bind_msg   = wid_get_bind_msg;
	m->drawarea       = wid_drawarea;
	m->drawbehind     = wid_drawbehind;
	m->draw_bg        = wid_draw_bg;
	m->is_root        = wid_is_root;
	m->calc_minmax    = wid_calc_minmax;
	m->remove_child   = wid_remove_child;
	m->release        = wid_release;
	m->related_to     = wid_related_to;
}


static void build_script_lang(void *widtype,struct widget_methods *m)
{
	script->reg_widget_attrib(widtype, "string type",       m->get_type,      NULL,             NULL);
	script->reg_widget_attrib(widtype, "boolean state",     m->get_state,     m->set_state,     m->update);
	script->reg_widget_attrib(widtype, "boolean grabfocus", m->get_grabfocus, m->set_grabfocus, m->update);
	script->reg_widget_method(widtype, "void bind(string type,string msg)", m->bind);
	script->reg_widget_method(widtype, "void unbind(string type)", m->unbind);
	script->reg_widget_method(widtype, "void focus()", m->focus);
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct widman_services services = {
	default_widget_data,
	default_widget_methods,
	build_script_lang,
};


/************************
 ** Module entry point **
 ************************/

int init_widman(struct mtk_services *d)
{
	redraw    = d->get_module("RedrawManager 1.0");
	msg       = d->get_module("Messenger 1.0");
	script    = d->get_module("Script 1.0");
	appman    = d->get_module("ApplicationManager 1.0");
	userstate = d->get_module("UserState 1.0");

	d->register_module("WidgetManager 1.0",&services);
	return 1;
}
