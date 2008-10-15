/*
 * \brief  Platform interface
 * \author Norman Feske
 * \date   2008-08-18
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/**
 * Set the state of 8 leds
 */
extern void set_leds(unsigned char leds_state);

/**
 * Display the images that correspond to the current platform
 */
extern void display_platform_images(void);

/**
 * Platform-specific initialization
 *
 * \return 0 on succes
 */
extern int init_platform(void);

#endif /* _PLATFORM_H_ */