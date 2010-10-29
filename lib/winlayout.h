/*
 * \brief   Interface of the window layout module of MTK
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_WINLAYOUT_H_
#define _MTK_WINLAYOUT_H_

#include "widget.h"

struct winlayout_services {
	WIDGET *(*create_win_elements) (s32 elem_mask, int x, int y, int width, int height);
	void    (*resize_win_elements) (WIDGET *first_elem, s32 elem_mask,
	                                int x, int y, int width, int height);
	void    (*set_win_title)       (WIDGET *first_elem,char *new_title);
	char   *(*get_win_title)       (WIDGET *first_elem);
	void    (*set_win_state)       (WIDGET *first_elem,s32 state);
	s32     (*get_left_border)     (s32 elem_mask);
	s32     (*get_right_border)    (s32 elem_mask);
	s32     (*get_top_border)      (s32 elem_mask);
	s32     (*get_bottom_border)   (s32 elem_mask);
};

#endif /* _MTK_WINLAYOUT_H_ */
