/*
 * \brief   Interface of MTK screen driver
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_SCRDRV_H_
#define _MTK_SCRDRV_H_

struct scrdrv_services {
	int  (*set_screen)     (void *fb, int width, int height, int depth);
	void  (*restore_screen) (void);
	int  (*get_scr_width)  (void);
	int  (*get_scr_height) (void);
	int  (*get_scr_depth)  (void);
	void *(*get_scr_adr)    (void);
	void *(*get_buf_adr)    (void);
	void  (*update_area)    (int x1, int y1, int x2, int y2);
	void  (*set_mouse_pos)  (int x, int y);
	void  (*set_mouse_shape)(void *);
};

#endif /* _MTK_SCRDRV_H_ */
