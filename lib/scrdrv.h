/*
 * \brief   Interface of DOpE screen driver
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_SCRDRV_H_
#define _DOPE_SCRDRV_H_

struct scrdrv_services {
	long  (*set_screen)     (long width, long height, long depth);
	void  (*restore_screen) (void);
	long  (*get_scr_width)  (void);
	long  (*get_scr_height) (void);
	long  (*get_scr_depth)  (void);
	void *(*get_scr_adr)    (void);
	void *(*get_buf_adr)    (void);
	void  (*update_area)    (long x1, long y1, long x2, long y2);
	void  (*set_mouse_pos)  (long x,long y);
	void  (*set_mouse_shape)(void *);
//  void  (*mouse_off)      (void);
//  void  (*mouse_on)       (void);
//  void  (*set_draw_area)  (long x1, long y1, long x2, long y2);
};

#endif /* _DOPE_SCRDRV_H_ */
