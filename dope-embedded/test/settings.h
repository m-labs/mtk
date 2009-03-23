/*
 * \brief  Interface of settings window
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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

/**
 * Initialize settings window
 */
extern void init_settings(void);

/**
 * Bring settings window to front
 */
extern void open_settings_window(void);

#endif /* _SETTINGS_H_ */
