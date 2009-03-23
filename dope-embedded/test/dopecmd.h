/*
 * \brief  Interface of DOpE command window
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

#ifndef _DOPECMD_H_
#define _DOPECMD_H_

/**
 * Bring DOpE command window to front
 */
extern void init_dopecmd(void);

/**
 * Bring DOpE command window to front
 */
extern void open_dopecmd_window(void);

#endif /* _DOPECMD_H_ */
