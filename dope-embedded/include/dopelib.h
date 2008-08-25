/*
 * \brief   Interface of DOpE client library
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

#ifndef __DOPE_INCLUDE_DOPELIB_H_
#define __DOPE_INCLUDE_DOPELIB_H_

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

typedef union dopelib_event_union {
	long type;
	command_event   command;
	motion_event    motion;
	press_event     press;
	release_event   release;
	keyrepeat_event keyrepeat;
} dope_event;


/**
 * Initialise dope library
 */
extern long  dope_init(void);


/**
 * Deinitialise dope library
 */
extern void  dope_deinit(void);


/**
 * Register dope client application
 *
 * \param appname  name of the DOpE application
 * \return         DOpE application id
 */
extern long  dope_init_app(const char *appname);


/**
 * Unregister dope client application
 *
 * \param app_id  DOpE application to unregister
 * \return        0 on success
 */
extern long  dope_deinit_app(long app_id);


/**
 * Execute dope command
 *
 * \param app_id  DOpE application id
 * \param cmd     command to execute
 * \return        0 on success
 */
extern int dope_cmd(long app_id, const char *cmd);


/**
 * Execute dope format string command
 *
 * \param app_id  DOpE application id
 * \param cmdf    command to execute specified as format string
 * \return        0 on success
 */
extern int dope_cmdf(long app_id, const char *cmdf, ...);


/**
 * Execute sequence of DOpE commands
 *
 * \param app_id  DOpE application id
 * \param ...     command sequence
 * \return        0 on success
 *
 * The sequence is a series of NULL-terminated strings. Its end must be
 * marked by a NULL pointer.
 */
extern int dope_cmd_seq(int app_id, ...);


/**
 * Execute dope command and request result
 *
 * \param app_id    DOpE application id
 * \param dst       destination buffer for storing the result string
 * \param dst_size  size of destination buffer in bytes
 * \param cmd       command to execute
 * \return          0 on success
 */
extern int dope_req(long app_id, char *dst, int dst_size, const char *cmd);


/**
 * Request result of a dope command specified as format string
 *
 * \param app_id    DOpE application id
 * \param dst       destination buffer for storing the result string
 * \param dst_size  size of destination buffer in bytes
 * \param cmd       command to execute - specified as format string
 * \return          0 on success
 */
extern int dope_reqf(long app_id, char *dst, int dst_size, const char *cmdf, ...);


/**
 * Bind an event to a dope widget
 *
 * \param app_id      DOpE application id
 * \param var         widget to bind an event to
 * \param event_type  identifier for the event type
 * \param callback    callback function to be called for incoming events
 * \param arg         additional argument for the callback function
 */
extern void dope_bind(long app_id,const char *var, const char *event_type,
                      void (*callback)(dope_event *,void *),void *arg);


/**
 * Bind an event to a dope widget specified as format string
 *
 * \param app_id      DOpE application id
 * \param varfmt      widget to bind an event to (format string)
 * \param event_type  identifier for the event type
 * \param callback    callback function to be called for incoming events
 * \param arg         additional argument for the callback function
 * \param ...         format string arguments
 */
extern void dope_bindf(long id, const char *varfmt, const char *event_type,
                       void (*callback)(dope_event *,void *), void *arg,...);


/**
 * Enter dope eventloop
 *
 * \param app_id  DOpE application id
 */
extern void dope_eventloop(long app_id);


/**
 * Return number of pending events
 *
 * \param app_id  DOpE application id
 * \return        number of pending events
 */
int dope_events_pending(int app_id);


/**
 * Process one single dope event
 *
 * This function processes exactly one DOpE event. If no event is pending, it
 * blocks until an event is available. Thus, for non-blocking operation, this
 * function should be called only if dope_events_pending was consulted before.
 *
 * \param app_id  DOpE application id
 */
extern void dope_process_event(long app_id);


/**
 * Request key or button state
 *
 * \param app_id   DOpE application id
 * \param keycode  keycode of the requested key
 * \return         1 if key is currently pressed
 */
extern long dope_get_keystate(long app_id, long keycode);


/**
 * Request current ascii keyboard state
 *
 * \param app_id   DOpE application id
 * \param keycode  keycode of the requested key
 * \return         ASCII value of the currently pressed key combination
 */
extern char dope_get_ascii(long app_id, long keycode);


#endif /* __DOPE_INCLUDE_DOPELIB_H_ */
