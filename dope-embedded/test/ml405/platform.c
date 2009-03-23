/*
 * \brief  Platform for ML405
 * \author Norman Feske
 * \author Matthias Alles
 * \date   2008-08-17
 */

/* FPGA includes */
#include "xparameters.h"
#ifdef __MICROBLAZE__
#include "mb_interface.h"
#endif /* __MICROBLAZE__ */

#ifdef __PPC__
#include "xcache_l.h"
#endif

/* local includes */
#include "platform.h"
#include "led.h"

/* DOpE includes */
#include <dopelib.h>
#include <stdio.h>
#include <stdlib.h>


/**
 * Callback: called when the led-control button of the main menu got pressed
 */
static void led_menu_button_callback(dope_event *e, void *arg)
{
	open_led_window();
}


/**
 * Callback: called when LED state was changed
 */
static void set_led_state(unsigned leds_state)
{
	int *ptr = (int *)(XPAR_LEDS_4BIT_BASEADDR);

	/* set the LED state */
	*ptr++ = leds_state;

	/* set GPIO direction to out */
	*ptr   = 0;

	return;
}


/*************************
 ** Interface functions **
 *************************/

void init_platform(void)
{

	/* enable PPC cache if available */
#ifdef __PPC__
	XCache_EnableICache(0x80000000);
	XCache_EnableDCache(0x80000000);
#endif
}


void init_platform_gui(int menu_app_id)
{
	init_led_window(4, set_led_state);

	/* add item into main menu */
	dope_cmd_seq(menu_app_id,
	 	"b_led_control = new Button(-text \"LED control\")",
		"g.place(b_led_control, -column 1 -row 20)",
		0);

	dope_bind(menu_app_id, "b_led_control", "commit", led_menu_button_callback, NULL);
}
