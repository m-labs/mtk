/*
 * \brief  Interface of grid demo window
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

#ifndef _GRID_H_
#define _GRID_H_

/**
 * Initialize grid window
 */
extern void init_grid_window(void);

/**
 * Reset window geometry and bring window to front
 */
extern void open_grid_window(void);

#endif /* _GRID_H_ */
