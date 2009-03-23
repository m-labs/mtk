/*
 * \brief  Platform for Starter Kit 3E
 * \author Norman Feske
 * \date   2008-08-17
 */

/* FPGA includes */
#include "xparameters.h"
#include "mb_interface.h"

/* local includes */
#include "platform.h"
#include "disp_img.h"
#include "led.h"
#include "gfx/spartan3e.h"
#include "gfx/microblaze_logo.h"
#include "gfx/kolt_logo.h"

/* DOpE includes */
#include <dopelib.h>
#include <stdio.h>
#include <stdlib.h>


static int s3e_app_id, mb_app_id, kolt_app_id; /* DOpE application IDs for logos */


/**
 * Callback: called when the led-control button of the main menu got pressed
 */
static void led_menu_button_callback(dope_event *e, void *arg)
{
	open_led_window();
}


/**
 * Callback: called when the Spartan3E button of the main menu got pressed
 */
static void s3e_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(s3e_app_id);
}


/**
 * Callback: called when the Microblaze button of the main menu got pressed
 */
static void mb_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(mb_app_id);
}


/**
 * Callback: called when the Kolt logo button of the main menu got pressed
 */
static void kolt_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(kolt_app_id);
}


/**
 * Callback: called when LED state was changed
 */
static void set_led_state(unsigned led_state)
{
	int *ptr = (int *)(XPAR_LEDS_8BIT_BASEADDR);
	*ptr = led_state;
}


void init_platform(void)
{
	/* enable microblaze cache if available */
#if XPAR_MICROBLAZE_0_USE_ICACHE
	microblaze_disable_icache();
	microblaze_init_icache_range(0, XPAR_MICROBLAZE_0_CACHE_BYTE_SIZE); 
	microblaze_enable_icache();
#endif
#if XPAR_MICROBLAZE_0_USE_DCACHE
	microblaze_disable_dcache();
	microblaze_init_dcache_range(0, XPAR_MICROBLAZE_0_DCACHE_BYTE_SIZE); 
	microblaze_enable_dcache();
#endif
}


void init_platform_gui(int menu_app_id)
{
	init_led_window(8, set_led_state);

	s3e_app_id = create_image_window("Spartan-3E", pixel_data_spartan3e,
	                                280, 200, 300, 223);

	mb_app_id = create_image_window("Microblaze", pixel_data_microblaze,
	                                25, 500, 269, 51);

	kolt_app_id = create_image_window("KOLT", pixel_data_kolt_logo,
	                                  55, 220, 250, 179);

	/* add items for LED-control window and logo windows into main menu */
	dope_cmd_seq(menu_app_id,
	 	"b_led_control = new Button(-text \"LED control\")",
	 	"b_s3e_logo    = new Button(-text \"Spartan3E logo\")",
	 	"b_mb_logo     = new Button(-text \"Microblaze logo\")",
	 	"b_kolt_logo   = new Button(-text \"Microblaze logo\")",
		"g.place(b_s3e_logo,    -column 1 -row 20)",
		"g.place(b_mb_logo,     -column 1 -row 21)",
		"g.place(b_kolt_logo,   -column 1 -row 22)",
		"g.place(b_led_control, -column 1 -row 23)",
		NULL);

	dope_bind(menu_app_id, "b_led_control", "commit", led_menu_button_callback,  NULL);
	dope_bind(menu_app_id, "b_s3e_logo",    "commit", s3e_menu_button_callback,  NULL);
	dope_bind(menu_app_id, "b_mb_logo",     "commit", mb_menu_button_callback,   NULL);
	dope_bind(menu_app_id, "b_kolt_logo",   "commit", kolt_menu_button_callback, NULL);
}
