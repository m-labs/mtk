/*
 * \brief  Platform for Starter Kit 3A
 * \author Norman Feske
 * \date   2008-08-17
 */

/* FPGA includes */
#include "xparameters.h"
#include "mb_interface.h"

/* local includes */
#include "platform.h"
#include "gfx/spartan3a.h"
#include "gfx/microblaze_logo.h"
#include "led.h"
#include "disp_img.h"

/* DOpE includes */
#include <dopelib.h>
#include <stdio.h>
#include <stdlib.h>


static int s3a_app_id, mb_app_id; /* DOpE application IDs for logos */


/**
 * Callback: called when the led-control button of the main menu got pressed
 */
static void led_menu_button_callback(dope_event *e, void *arg)
{
	open_led_window();
}


/**
 * Callback: called when the Spartan3A button of the main menu got pressed
 */
static void s3a_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(s3a_app_id);
}


/**
 * Callback: called when the Microblaze button of the main menu got pressed
 */
static void mb_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(mb_app_id);
}


/**
 * Callback: called when LED state was changed
 */
static void set_led_state(unsigned led_state)
{
	int *ptr = (int *)(XPAR_LEDS_8BIT_BASEADDR);
	*ptr = led_state;
}


/*************************
 ** Interface functions **
 *************************/

void init_platform(void)
{
	/* enable microblaze cache if available */
#if XPAR_MICROBLAZE_0_USE_ICACHE 
	microblaze_enable_icache();
#endif
#if XPAR_MICROBLAZE_0_USE_DCACHE 
	microblaze_enable_dcache();
#endif
}


void init_platform_gui(int menu_app_id)
{
	init_led_window(8, set_led_state);

	s3a_app_id = create_image_window("Spartan-3A", pixel_data_spartan3a,
		                    280, 200, 300, 223);

	mb_app_id = create_image_window("Microblaze", pixel_data_microblaze,
		                    25, 500, 269, 51);

	/* add items for LED-control window and logo windows into main menu */
	dope_cmd_seq(menu_app_id,
	 	"b_led_control = new Button(-text \"LED control\")",
	 	"b_s3a_logo    = new Button(-text \"Spartan3A logo\")",
	 	"b_mb_logo     = new Button(-text \"Microblaze logo\")",
		"g.place(b_s3a_logo,    -column 1 -row 20)",
		"g.place(b_mb_logo,     -column 1 -row 21)",
		"g.place(b_led_control, -column 1 -row 22)",
		NULL);

	dope_bind(menu_app_id, "b_led_control", "commit", led_menu_button_callback,  NULL);
	dope_bind(menu_app_id, "b_s3a_logo",    "commit", s3a_menu_button_callback,  NULL);
	dope_bind(menu_app_id, "b_mb_logo",     "commit", mb_menu_button_callback,   NULL);
}
