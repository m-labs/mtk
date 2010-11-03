/*
 * \brief   General widget data structures
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_WIDGET_DATA_H_
#define _MTK_WIDGET_DATA_H_

/**
 * Widget property and state flags
 */
#define WID_FLAGS_STATE      0x0001   /* widget is selected                  */
#define WID_FLAGS_MFOCUS     0x0002   /* widget has mouse focus              */
#define WID_FLAGS_KFOCUS     0x0004   /* widget has keyboard focus           */
#define WID_FLAGS_EVFORWARD  0x0008   /* pass unhandled events to the parent */
#define WID_FLAGS_HIGHLIGHT  0x0010   /* widget reacts on mouse focus        */
#define WID_FLAGS_EDITABLE   0x0020   /* widget interprets key strokes       */
#define WID_FLAGS_CONCEALING 0x0040   /* widget covers its area completely   */
#define WID_FLAGS_SELECTABLE 0x0080   /* widget is selectable via keyboard   */
#define WID_FLAGS_TAKEFOCUS  0x0100   /* widget can receive keyboard focus   */
#define WID_FLAGS_GRABFOCUS  0x0200   /* prevent keyboard focus to switch    */

/**
 * Widget update flags
 */
#define WID_UPDATE_SIZE     0x0001
#define WID_UPDATE_MINMAX   0x0002
#define WID_UPDATE_NEWCHILD 0x0004
#define WID_UPDATE_REFRESH  0x0008

struct binding;
struct binding {
	s16              ev_type;     /* event type          */
	char            *bind_ident;  /* action event string */
	char            *msg;         /* associated message  */
	struct binding  *next;        /* next binding        */
};


struct new_binding;
struct new_binding {
	int    app_id;       /* application to receive the event */
	char  *name;         /* name of binding                  */
	char **args;         /* arguments to pass with the event */
	char **cond_names;   /* conditions to be checked         */
	char **cond_values;  /* desired values of the conditions */
	struct new_binding *next;
};


struct widget_data {
	int    x, y, w, h;         /* current relative position and size  */
	int    min_w, min_h;       /* minimal size                        */
	int    max_w, max_h;       /* maximal size                        */
	int    flags;              /* state flags                         */
	int    update;             /* update flags                        */
	void    *context;           /* associated data                     */
	WIDGET  *parent;            /* parent in widget hierarchy          */
	WIDGET  *next;              /* next widget in a connected list     */
	WIDGET  *prev;              /* previous widget in a connected list */
	void    (*click) (void *);  /* event handle routine                */
	int    ref_cnt;            /* reference counter                   */
	s32     app_id;             /* application that owns the widget    */
	struct binding *bindings;   /* event bindings                      */
	struct new_binding *new_bindings;
};


#endif /* _MTK_WIDGET_DATA_H_ */
