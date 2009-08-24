/*
 * \brief  Platform for TE0304
 * \author Norman Feske
 * \date   2009-02-24
 */

/* FPGA includes */
#include "xparameters.h"
#include "mb_interface.h"

/* local includes */
#include "platform.h"
#include "disp_img.h"
#include "led.h"
#include "util.h"
#include "gfx/kolt_logo.h"

/* DOpE includes */
#include <dopelib.h>
#include <stdio.h>
#include <stdlib.h>


static int ovl_visible;  /* visibility state of the overlay */
static int ovl_app_id;   /* DOpE application ID used for overlay window */
static int kolt_app_id;  /* DOpE application ID used for the Kolt logo */


/**
 * Define overlay region
 *
 * This function is implemented by the te0304-specific 'scrdrv.c'.
 */
void scrdrv_overlay(long x1, long y1, long x2, long y2);


/**
 * Callback: called when the led-control button of the main menu got pressed
 */
static void led_menu_button_callback(dope_event *e, void *arg)
{
	open_led_window();
}


/**
 * Callback: called when the Kolt logo button got pressed
 */
static void kolt_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(kolt_app_id);
}


/**
 * Callback: gets called when the overlay window changes its position
 */
static void ovl_winpos_callback(dope_event *e, void *arg)
{
	int new_x = dope_req_l(ovl_app_id, "ovl_win.workx"),
	    new_y = dope_req_l(ovl_app_id, "ovl_win.worky"),
	    new_w = dope_req_l(ovl_app_id, "ovl_win.workw"),
	    new_h = dope_req_l(ovl_app_id, "ovl_win.workh");

	if (ovl_visible)
		scrdrv_overlay(new_x, new_y, new_x + new_w - 1, new_y + new_h - 1);
	else
		scrdrv_overlay(-1, -1, -1, -1);
}


/**
 * Callback: called when the overlay window button of the main menu got pressed
 */
static void ovl_menu_button_callback(dope_event *e, void *arg)
{
	/* enable overlay */
	ovl_visible = 1;
	ovl_winpos_callback(NULL, NULL);

	dope_cmd(ovl_app_id, "ovl_win.open()");
	dope_cmd(ovl_app_id, "ovl_win.top()");
}


/**
 * Callback: called when the window-close button of the overlay window got pressed
 */
static void ovl_close_callback(dope_event *e, void *arg)
{
	/* disable overlay */
	ovl_visible = 0;
	ovl_winpos_callback(NULL, NULL);

	dope_cmd(ovl_app_id, "ovl_win.close()");
}


/**
 * Callback: called when LED state changes
 */
static void set_led_state(unsigned led_state)
{
	int *ptr = (int *)(XPAR_LED_BASEADDR);
	*ptr = led_state;
}


/*************************
 ** Interface functions **
 *************************/

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
	init_led_window(1, set_led_state);

	kolt_app_id = create_image_window("Kolt", pixel_data_kolt_logo, 420,  50, 250, 179);
	ovl_app_id  = dope_init_app("Overlay");

	/* create window that "contains" the overlay */
	dope_cmd(ovl_app_id, "ovl_win = new Window(-x 200 -y 150 -w 250 -h 200)");

	/* install callbacks to get notified about window-position changes */
	dope_bind(ovl_app_id, "ovl_win", "move",    ovl_winpos_callback, NULL);
	dope_bind(ovl_app_id, "ovl_win", "resized", ovl_winpos_callback, NULL);
	dope_bind(ovl_app_id, "ovl_win", "close",   ovl_close_callback,  NULL);

	/* add items for LED-control window and overlay window into main menu */
	dope_cmd_seq(menu_app_id,
	 	"b_kolt_logo   = new Button(-text \"Kolt logo\")",
	 	"b_led_control = new Button(-text \"LED control\")",
	 	"b_overlay     = new Button(-text \"Overlay test\")",
		"g.place(b_kolt_logo,   -column 1 -row 20)",
		"g.place(b_led_control, -column 1 -row 21)",
		"g.place(b_overlay,     -column 1 -row 22)",
		0);

	dope_bind(menu_app_id, "b_kolt_logo",   "commit", kolt_menu_button_callback, NULL);
	dope_bind(menu_app_id, "b_led_control", "commit", led_menu_button_callback,  NULL);
	dope_bind(menu_app_id, "b_overlay",     "commit", ovl_menu_button_callback,  NULL);
}
