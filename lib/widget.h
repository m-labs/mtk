/*
 * \brief   General MTK widget structures
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_WIDGET_H_
#define _MTK_WIDGET_H_

#include "event.h"

#if !defined(WIDGET)
#define WIDGET struct widget
#endif

#if !defined(WIDGETARG)
#define WIDGETARG WIDGET
#endif


/**
 * Widget drawing operation flags
 */

#define WID_DRAW_TRANS 0x1  /* draw transparent widget elements */
#define WID_DRAW_SOLID 0x2  /* draw solid widget elements       */


struct widget_methods;
struct gfx_ds;

struct widget {
	struct widget_methods *gen;   /* generic widget functions       */
	void                  *spec;  /* widget type specific functions */
	struct widget_data    *wd;    /* generic widget data            */
	void                  *sd;    /* widget type specific data      */
};

/**
 * Functions that all widgets have in common
 */
struct widget_methods {

	/**
	 * Free widget specific data
	 *
	 * This function must be implemented by all widgets that manage dynamically
	 * allocated widget type specific data structures. Such memory must be
	 * freed within this function. Layout widgets should call for every child
	 * the widget release function to detach from the layout widget.
	 *
	 * It is called implicitely when the widget gets destroyed.
	 */
	void (*free_data) (WIDGETARG *);


	char   *(*get_type)     (WIDGETARG *);
	s32     (*get_app_id)   (WIDGETARG *);
	void    (*set_app_id)   (WIDGETARG *, s32 app_id);
	long    (*get_x)        (WIDGETARG *);
	void    (*set_x)        (WIDGETARG *, long new_x);
	long    (*get_y)        (WIDGETARG *);
	void    (*set_y)        (WIDGETARG *, long new_y);
	long    (*get_w)        (WIDGETARG *);
	void    (*set_w)        (WIDGETARG *, long new_w);
	long    (*get_h)        (WIDGETARG *);
	void    (*set_h)        (WIDGETARG *, long new_h);
	long    (*get_min_w)    (WIDGETARG *);
	long    (*get_min_h)    (WIDGETARG *);
	long    (*get_max_w)    (WIDGETARG *);
	long    (*get_max_h)    (WIDGETARG *);
	long    (*get_abs_x)    (WIDGETARG *);
	long    (*get_abs_y)    (WIDGETARG *);
	int     (*get_state)    (WIDGETARG *);
	void    (*set_state)    (WIDGETARG *, int new_state);
	int     (*get_evforward)(WIDGETARG *);
	void    (*set_evforward)(WIDGETARG *, int new_state);
	WIDGET *(*get_parent)   (WIDGETARG *);
	void    (*set_parent)   (WIDGETARG *, void *parent);
	void   *(*get_context)  (WIDGETARG *);
	void    (*set_context)  (WIDGETARG *, void *context);
	WIDGET *(*get_next)     (WIDGETARG *);
	void    (*set_next)     (WIDGETARG *, WIDGETARG *next);
	WIDGET *(*get_prev)     (WIDGETARG *);
	void    (*set_prev)     (WIDGETARG *, WIDGETARG *prev);
	int     (*get_selectable) (WIDGETARG *);
	void    (*set_selectable) (WIDGETARG *, int new_flag);


	/**
	 * Draw widget - to implement for every widget
	 *
	 * If a widget is specified as origin argument, the draw function does not
	 * perform any actual drawing operations but only returns if the specified
	 * origin widget is visible through the widget. This way, we can determine
	 * if a transparent window in the foreground must be updated when a window
	 * in the background changes.
	 *
	 * \param dst     destination gfx container into which to draw
	 * \param x       x position of left top corner
	 * \param y       y position of left top corner
	 * \param origin  widget for which we want to know if it is visible
	 *                or not, or NULL if drawing should be performed.
	 * \returns       0 if no drawing operation was performed,
	 *                1 if a drawing operation was performed
	 */
	int (*draw) (WIDGETARG *, struct gfx_ds *dst, long x, long y, WIDGETARG *origin);


	/**
	 * Cause the redraw of a specified widget area
	 *
	 * This function can be used to update a certain region of a widget.
	 * It propagates the redraw request to the parent that shrinks the
	 * effective redraw area to the visible area. The root-parent (for
	 * example the Screen widget) catches the request by a custom
	 * implementation of this function and performes the actual
	 * drawing operation.
	 *
	 * This function is called exclusively by the redraw manager.
	 * It is never called by widgets.
	 *
	 * \param cw       current widget that propagates the request
	 * \param ow       widget to redraw
	 * \param x,y,w,h  area to be redrawn - relative to widget cw
	 * \returns        1 if drawing operation was performed or if specified
	 *                 os widget is visible, otherwise 0.
	 */
	int (*drawarea) (WIDGETARG *cw, WIDGETARG *ow, long x, long y, long w, long h);


	/**
	 * Cause the redraw of the window behind a specified widget area
	 *
	 * This function is a drawing primitive that can be used from within
	 * widget's draw function to implement transparent user interface elements.
	 * For more information about the origin parameter please revisit the
	 * description of the draw function.
	 *
	 * \param cw        current widget that propagates the request
	 * \param child     reference to the caller of the function
	 *                  This information is used by the screen to find the
	 *                  refering window.
	 * \param x,y,w,h   area to be redrawn - relative to widget cw.
	 * \param origin    widget for which we want to know if it is visible
	 *                  or not, or NULL if drawing should be performed.
	 * \returns         1 if any graphics operations were performed,
	 *                  0 if no graphics operations were performed.
	 */
	int (*drawbehind) (WIDGETARG *cw, WIDGETARG *child,
	                   long x, long y, long w, long h, WIDGETARG *origin);


	/**
	 * Draw background of widget
	 *
	 * This function is only used by widget draw functions to draw their
	 * background. A widget can decide whether a background is drawn or not.
	 * Since the background drawing is propagated to the parents, stacked
	 * backgrounds can be easily handled without overdrawing an area multiple
	 * times. For more information about the origin parameter, please revisit
	 * the description of the draw function.
	 *
	 * The position is specified as absolute coordinates.
	 *
	 * \param dst     destination gfx container into which to draw
	 * \param x,y     absolute screen position of left top corner of area
	 * \param w,h     dimensions of area to draw
	 * \param opaque  do not use transparency / do not call drawbehind
	 * \param origin    widget for which we want to know if it is visible
	 *                  or not, or NULL if drawing should be performed.
	 * \returns       0 if no drawing operation was performed,
	 *                1 if a drawing operation was performed
	 */
	int (*draw_bg) (WIDGETARG *, struct gfx_ds *dst, long x, long y, long w, long h,
	                WIDGETARG *origin, int opaque);


	/**
	 * Update widgets when attributes changed
	 *
	 * This function is called after a set of widget attributes changed to
	 * update the internal state of the widget dependent on the new attribute
	 * settings and to force a redraw of the widget on screen. Usually this is
	 * the case when finishing a 'set' MTK command.
	 *
	 * This function should only be implemented by widgets that need to
	 * react on attribute changes specifically to update their internal
	 * state. Widget specific implementations should always call the default
	 * implementation of the widget base class to perform.
	 */
	void (*update) (WIDGETARG *);


	/**
	 * Update widget size and position
	 *
	 * This function is exclusively called by the parent widget after setting
	 * the relative size and position of the widget.  Widgets should implement
	 * the placement routine for child widgets in this function.
	 */
	void    (*updatepos)    (WIDGETARG *);
	WIDGET *(*find)         (WIDGETARG *, long x, long y);
	void    (*inc_ref)      (WIDGETARG *);


	/**
	 * Decrement reference counter and destroy widget when needed
	 *
	 * This function must be called when releasing a reference to a widget
	 * to decrement its reference counter. If there are no references left,
	 * the widget gets destroyed.
	 *
	 * This function frees all generic widget data and calls the widget type
	 * specific free_data function.
	 */
	void (*dec_ref) (WIDGETARG *);


	/**
	 * Trigger a redraw of a widget
	 *
	 * Calling this function causes the redraw of the widget. The widget
	 * is not redrawn immediately but the redraw request is handed over
	 * to the Redraw Manager, which can decide when to perform the actual
	 * redrawing operation.
	 */
	void (*force_redraw) (WIDGETARG *);


	/**
	 * Set/request mouse focus of a widget
	 *
	 * Widgets with an enabled WID_FLAGS_HIGHLIGHT flag change their look
	 * when the mouse moves over them. This functions set and request
	 * the current mouse focus state. It is normally called from the
	 * Userstate Manager.
	 */
	void (*set_mfocus) (WIDGETARG *, int new_mfocus);
	int  (*get_mfocus) (WIDGETARG *);


	/**
	 * Set/request keyboard focus of a widget
	 *
	 * Widgets with an enabled WID_FLAGS_EDIT flag can take the keyboard
	 * focus. This functions set and request the keyboard focus state of
	 * such widgets. They are normally called from the Userstate Manager
	 * only.
	 */
	void (*set_kfocus) (WIDGETARG *, int new_kfocus);
	int  (*get_kfocus) (WIDGETARG *);


	/**
	 * Set/request grabfocus flag
	 *
	 * If this flag is set, the keyboard focus remains at the widget once the
	 * widget got it until the user clicks on another widget. Per default,
	 * widgets do not grab the focus.
	 */
	void (*set_grabfocus) (WIDGETARG *, int new_grabfocus);
	int  (*get_grabfocus) (WIDGETARG *);


	/**
	 * Make widget the current keyboard focus of its window
	 */
	void (*focus) (WIDGETARG *);


	/**
	 * Request first widget of a subtree that can take the keyboard focus
	 *
	 * This function finds a suitable widget to take the keyboard focus within
	 * a widget subtree. It is usually called by layout widgets that want to
	 * change the keyboard focus among its children to find out the child that
	 * is able to bear the keyboard focus job.
	 *
	 * \return  NULL if no widget of the subtree can take the keyboard focus,
	 *          or the kfocus-capable widget.
	 */
	WIDGET *(*first_kfocus) (WIDGETARG *);


	/**
	 * Handle event
	 *
	 * This function is called when an event occurs that needs to be handled
	 * by the widget. If the widget does not handle the event by itself it
	 * can propagate the event to the parent.
	 *
	 * \param ev    event data
	 * \param from  NULL if the widget received the event immediately
	 *              or the child that propagated the event
	 */
	void (*handle_event) (WIDGETARG *, EVENT *ev, WIDGET *from);


	/**
	 * Determine the window in where the widget resides
	 *
	 * Return value is either the window that contains the specified widget
	 * or NULL if the widget is homeless.
	 */
	WIDGET *(*get_window)   (WIDGETARG *);
	int     (*do_layout)    (WIDGETARG *, WIDGETARG *);
	void    (*bind)         (WIDGETARG *, char *bind_ident, char *message);
	void    (*unbind)       (WIDGETARG *, char *bind_ident);
	char   *(*get_bind_msg) (WIDGETARG *, char *bind_ident);


	/**
	 * Determine if widget is a root widget
	 *
	 * The only existing root widget is Screen at the moment.
	 */
	int (*is_root) (WIDGETARG *);


	/**
	 * Calculate current min/max boundaries of the widget
	 *
	 * The actual min/max boundaries depend on the widget type and the
	 * boundaries of child widgets. This function updates the min_w, min_h,
	 * max_w and max_h attributes of the widget. It is called from the
	 * widget update function.
	 *
	 * All widgets with min/max constains should implement this function.
	 */
	void (*calc_minmax) (WIDGETARG *);

	/**
	 * Remove child widget from widget
	 *
	 * This function is called when a child widget gets adopted by another
	 * widget. It detaches itself from its parent by calling this function.
	 *
	 * This function should be implemented by all layout widgets.
	 */
	void (*remove_child) (WIDGETARG *, WIDGETARG *child);


	/**
	 * Break with parent - let's be free
	 *
	 * This function quits the child relationship of a widget. It should
	 * be called by layout widgets before a new child is adopted or on
	 * destroying the Grid. This way, we avoid multiple parents quarrel
	 * about one end the same child.
	 */
	void (*release) (WIDGETARG *);


	/**
	 * Check if widget has a parent relationship to another widget
	 *
	 * This function must be called by layout widget before adopting
	 * a new child to avoid cyclic parent relationships.
	 */
	int (*related_to) (WIDGETARG *, WIDGETARG *);
};

#endif /* _MTK_WIDGET_H_ */
