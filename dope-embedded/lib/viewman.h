/*
 * \brief   Interface of view manager module
 * \date    2004-09-03
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_VIEWMAN_H_
#define _DOPE_VIEWMAN_H_

struct view;

struct viewman_services {
	struct view *(*create)    (void);
	void         (*destroy)   (struct view *);
	int          (*place)     (struct view *, int x, int y, int w, int h);
	int          (*top)       (struct view *);
	int          (*back)      (struct view *);
	int          (*set_title) (struct view *, const char *title);
	int          (*set_bg)    (struct view *);
};


#endif /* _DOPE_VIEWMAN_H_ */
