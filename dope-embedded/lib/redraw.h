/*
 * \brief   Interface of the redraw manager of DOpE
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_REDRAW_H_
#define _DOPE_REDRAW_H_

#include "widget.h"

struct redraw_services {
	void  (*draw_area)       (WIDGET *win, int x1, int y1, int x2, int y2);
	void  (*draw_widget)     (WIDGET *wid);
	void  (*draw_widgetarea) (WIDGET *wid, int x1, int y1, int x2, int y2);
	s32   (*exec_redraw)     (s32 avail_time);
	void  (*exec_redraw_all) (void);
	s32   (*process_pixels)  (s32 max_pixels);
	u32   (*get_noque)       (void);
	s32   (*is_queued)       (WIDGET *wid);
};


#endif /* _DOPE_REDRAW_H_ */
