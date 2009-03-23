/*
 * \brief  Interface of slideshow window
 * \author Norman Feske
 * \date   2009-03-09
 */

/*
 * Copyright (C) 2009 Norman Feske
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _SLIDESHOW_H_
#define _SLIDESHOW_H_

/**
 * Initialize slideshow window
 */
extern void init_slideshow_window(void);

/**
 * Reset slideshow and bring slideshow window to front
 */
extern void open_slideshow_window(void);

#endif /* _SLIDESHOW_H_ */
