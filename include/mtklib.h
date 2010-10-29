/*
 * \brief   Interface of MTK client library
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef __MTK_INCLUDE_MTKLIB_H_
#define __MTK_INCLUDE_MTKLIB_H_

#define EVENT_TYPE_UNDEFINED    0
#define EVENT_TYPE_COMMAND      1
#define EVENT_TYPE_MOTION       2
#define EVENT_TYPE_PRESS        3
#define EVENT_TYPE_RELEASE      4
#define EVENT_TYPE_KEYREPEAT    5

typedef struct command_event {
	long  type;                     /* must be EVENT_TYPE_COMMAND */
	char *cmd;                      /* command string */
} command_event;

typedef struct motion_event {
	long type;                      /* must be EVENT_TYPE_MOTION */
	long rel_x,rel_y;               /* relative movement in x and y direction */
	long abs_x,abs_y;               /* current position inside the widget */
} motion_event;

typedef struct press_event {
	long type;                      /* must be EVENT_TYPE_PRESS */
	long code;                      /* code of key/button that is pressed */
} press_event;

typedef struct release_event {
	long type;                      /* must be EVENT_TYPE_RELEASE */
	long code;                      /* code of key/button that is released */
} release_event;

typedef struct keyrepeat_event {
	long type;                      /* must be EVENT_TYPE_KEYREPEAT */
	long code;                      /* code of key/button that is pressed */
} keyrepeat_event;

typedef union mtklib_event_union {
	long type;
	command_event   command;
	motion_event    motion;
	press_event     press;
	release_event   release;
	keyrepeat_event keyrepeat;
} mtk_event;


/**
 * Initialise mtk library
 */
extern long  mtk_init(void);


/**
 * Deinitialise mtk library
 */
extern void  mtk_deinit(void);


/**
 * Register mtk client application
 *
 * \param appname  name of the MTK application
 * \return         MTK application id
 */
extern long  mtk_init_app(const char *appname);


/**
 * Unregister mtk client application
 *
 * \param app_id  MTK application to unregister
 * \return        0 on success
 */
extern long  mtk_deinit_app(long app_id);


/**
 * Execute mtk command
 *
 * \param app_id  MTK application id
 * \param cmd     command to execute
 * \return        0 on success
 */
extern int mtk_cmd(long app_id, const char *cmd);


/**
 * Execute mtk format string command
 *
 * \param app_id  MTK application id
 * \param cmdf    command to execute specified as format string
 * \return        0 on success
 */
extern int mtk_cmdf(long app_id, const char *cmdf, ...);


/**
 * Execute sequence of MTK commands
 *
 * \param app_id  MTK application id
 * \param ...     command sequence
 * \return        0 on success
 *
 * The sequence is a series of NULL-terminated strings. Its end must be
 * marked by a NULL pointer.
 */
extern int mtk_cmd_seq(int app_id, ...);


/**
 * Execute mtk command and request result
 *
 * \param app_id    MTK application id
 * \param dst       destination buffer for storing the result string
 * \param dst_size  size of destination buffer in bytes
 * \param cmd       command to execute
 * \return          0 on success
 */
extern int mtk_req(long app_id, char *dst, int dst_size, const char *cmd);


/**
 * Request result of a mtk command specified as format string
 *
 * \param app_id    MTK application id
 * \param dst       destination buffer for storing the result string
 * \param dst_size  size of destination buffer in bytes
 * \param cmd       command to execute - specified as format string
 * \return          0 on success
 */
extern int mtk_reqf(long app_id, char *dst, int dst_size, const char *cmdf, ...);


/**
 * Bind an event to a mtk widget
 *
 * \param app_id      MTK application id
 * \param var         widget to bind an event to
 * \param event_type  identifier for the event type
 * \param callback    callback function to be called for incoming events
 * \param arg         additional argument for the callback function
 */
extern void mtk_bind(long app_id,const char *var, const char *event_type,
                      void (*callback)(mtk_event *,void *),void *arg);


/**
 * Bind an event to a mtk widget specified as format string
 *
 * \param app_id      MTK application id
 * \param varfmt      widget to bind an event to (format string)
 * \param event_type  identifier for the event type
 * \param callback    callback function to be called for incoming events
 * \param arg         additional argument for the callback function
 * \param ...         format string arguments
 */
extern void mtk_bindf(long id, const char *varfmt, const char *event_type,
                       void (*callback)(mtk_event *,void *), void *arg,...);


/**
 * Enter mtk eventloop
 *
 * \param app_id  MTK application id
 */
extern void mtk_eventloop(long app_id);


/**
 * Return number of pending events
 *
 * \param app_id  MTK application id
 * \return        number of pending events
 */
int mtk_events_pending(int app_id);


/**
 * Process one single mtk event
 *
 * This function processes exactly one MTK event. If no event is pending, it
 * blocks until an event is available. Thus, for non-blocking operation, this
 * function should be called only if mtk_events_pending was consulted before.
 *
 * \param app_id  MTK application id
 */
extern void mtk_process_event(long app_id);


/**
 * Request key or button state
 *
 * \param app_id   MTK application id
 * \param keycode  keycode of the requested key
 * \return         1 if key is currently pressed
 */
extern long mtk_get_keystate(long app_id, long keycode);


/**
 * Request current ascii keyboard state
 *
 * \param app_id   MTK application id
 * \param keycode  keycode of the requested key
 * \return         ASCII value of the currently pressed key combination
 */
extern char mtk_get_ascii(long app_id, long keycode);


#endif /* __MTK_INCLUDE_MTKLIB_H_ */
