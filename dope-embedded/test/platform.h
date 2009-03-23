/*
 * \brief  Platform interface
 * \author Norman Feske
 * \date   2008-08-18
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/**
 * Platform-specific initialization
 *
 * This function is called before any other code is executed.
 * It is the right place to initialize caches and peripherals.
 */
extern void init_platform(void);

/**
 * Platform-specific customization of the main menu
 *
 * \param menu_app_id  DOpE application ID of main menu
 *
 * By using 'menu_app_id' the platform is able to supplement
 * and modify the main menu by custom items.
 */
extern void init_platform_gui(int menu_app_id);

#endif /* _PLATFORM_H_ */
