/*
 * \brief   Interface of clipping stack handling module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _MTK_CLIPPING_H_
#define _MTK_CLIPPING_H_

struct clipping_services {
	void     (*push)        (int x1, int y1, int x2, int y2);
	void     (*pop)         (void);
	void     (*reset)       (void);
	void     (*set_range)   (int x1, int y1, int x2, int y2);
	int     (*get_x1)      (void);
	int     (*get_y1)      (void);
	int     (*get_x2)      (void);
	int     (*get_y2)      (void);
};


#endif /* _MTK_CLIPPING_H_ */
