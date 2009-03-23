/*
 * \brief  Interface of LED-control window
 * \author Norman Feske
 * \date   2009-03-23
 */

/*
 * Copyright (C) 2009 Norman Feske
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _LED_H_
#define _LED_H_

/**
 * Initialize LED-control window
 *
 * \param num_leds       number of LEDs to control
 * \param set_led_state  function to be called on LED-state changes
 */
extern void init_led_window(int num_leds,
                            void (*set_led_state)(unsigned new_state));

/**
 * Bring LED-control window to front
 */
extern void open_led_window(void);

#endif /* _SETTINGS_H_ */
