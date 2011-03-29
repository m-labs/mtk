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

#define MTK_VERSION "1.0"

#define EVENT_TYPE_UNDEFINED    0
#define EVENT_TYPE_COMMAND      1
#define EVENT_TYPE_MOTION       2
#define EVENT_TYPE_PRESS        3
#define EVENT_TYPE_RELEASE      4
#define EVENT_TYPE_USER_BASE    1000

typedef struct command_event {
	int  type;                     /* must be EVENT_TYPE_COMMAND */
	char *cmd;                      /* command string */
} command_event;

typedef struct motion_event {
	int type;                      /* must be EVENT_TYPE_MOTION */
	int rel_x,rel_y;               /* relative movement in x and y direction */
	int abs_x,abs_y;               /* current position inside the widget */
} motion_event;

typedef struct press_event {
	int type;                      /* must be EVENT_TYPE_PRESS */
	int code;                      /* code of key/button that is pressed */
} press_event;

typedef struct release_event {
	int type;                      /* must be EVENT_TYPE_RELEASE */
	int code;                      /* code of key/button that is released */
} release_event;

typedef struct keyrepeat_event {
	int type;                      /* must be EVENT_TYPE_KEYREPEAT */
	int code;                      /* code of key/button that is pressed */
} keyrepeat_event;

typedef union mtklib_event_union {
	int type;
	command_event   command;
	motion_event    motion;
	press_event     press;
	release_event   release;
	keyrepeat_event keyrepeat;
} mtk_event;

#define MAX_EVENTS 64

/* Configuration */
extern unsigned int config_wallpaper_w, config_wallpaper_h;
extern unsigned short *config_wallpaper_bitmap;

/**
 * Initialise mtk library
 */
extern int mtk_init(void *fb, int width, int height);


/**
 * Register mtk client application
 *
 * \param appname  name of the MTK application
 * \return         MTK application id
 */
extern int mtk_init_app(const char *appname);


/**
 * Unregister mtk client application
 *
 * \param app_id  MTK application to unregister
 * \return        0 on success
 */
extern int mtk_deinit_app(int app_id);


/**
 * Execute mtk command
 *
 * \param app_id  MTK application id
 * \param cmd     command to execute
 * \return        0 on success
 */
extern int mtk_cmd(int app_id, const char *cmd);


/**
 * Execute mtk format string command
 *
 * \param app_id  MTK application id
 * \param cmdf    command to execute specified as format string
 * \return        0 on success
 */
extern int mtk_cmdf(int app_id, const char *cmdf, ...);


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
extern int mtk_req(int app_id, char *dst, int dst_size, const char *cmd);


/**
 * Request result of a mtk command specified as format string
 *
 * \param app_id    MTK application id
 * \param dst       destination buffer for storing the result string
 * \param dst_size  size of destination buffer in bytes
 * \param cmd       command to execute - specified as format string
 * \return          0 on success
 */
extern int mtk_reqf(int app_id, char *dst, int dst_size, const char *cmdf, ...);


/**
 * Bind an event to a mtk widget
 *
 * \param app_id      MTK application id
 * \param var         widget to bind an event to
 * \param event_type  identifier for the event type
 * \param callback    callback function to be called for incoming events
 * \param arg         additional argument for the callback function
 */
extern void mtk_bind(int app_id,const char *var, const char *event_type,
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
extern void mtk_bindf(int id, const char *varfmt, const char *event_type,
                       void (*callback)(mtk_event *,void *), void *arg,...);


extern void mtk_input(mtk_event *e, int count);

/**
 * Request key or button state
 *
 * \param app_id   MTK application id
 * \param keycode  keycode of the requested key
 * \return         1 if key is currently pressed
 */
extern int mtk_get_keystate(int app_id, int keycode);


/**
 * Request current ascii keyboard state
 *
 * \param app_id   MTK application id
 * \param keycode  keycode of the requested key
 * \return         ASCII value of the currently pressed key combination
 */
extern char mtk_get_ascii(int app_id, int keycode);


#endif /* __MTK_INCLUDE_MTKLIB_H_ */
