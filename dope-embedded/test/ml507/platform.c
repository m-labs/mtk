/*
 * \brief  Platform for ML507
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
#include "disp_img.h"
#include "gfx/virtex5.h"
#include "gfx/powerpc_logo.h"

/* DOpE includes */
#include <dopelib.h>
#include <stdio.h>
#include <stdlib.h>


static int v5_app_id, ppc_app_id; /* DOpE application IDs for logos */


/**
 * Callback: called when the led-control button of the main menu got pressed
 */
static void led_menu_button_callback(dope_event *e, void *arg)
{
	open_led_window();
}


/**
 * Callback: called when the Virtex5 button of the main menu got pressed
 */
static void v5_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(v5_app_id);
}


/**
 * Callback: called when the PowerPC button of the main menu got pressed
 */
static void ppc_menu_button_callback(dope_event *e, void *arg)
{
	open_image_window(ppc_app_id);
}


/**
 * Callback: called when LED state was changed
 */
static void set_led_state(unsigned led_state)
{
	int *ptr = (int *)(XPAR_LEDS_8BIT_BASEADDR);

	/* set the LED state */
	*ptr++ = led_state;

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
	init_led_window(8, set_led_state);

	v5_app_id  = create_image_window("Virtex 5",    pixel_data_virtex5, 280, 200, 250, 127);
	ppc_app_id = create_image_window("PowerPC 440", pixel_data_ppc,      25, 500, 126,  76);

	/* add items for LED-control window and logo windows into main menu */
	dope_cmd_seq(menu_app_id,
	 	"b_led_control = new Button(-text \"LED control\")",
	 	"b_v5_logo     = new Button(-text \"Virtex 5 logo\")",
	 	"b_ppc_logo    = new Button(-text \"PowerPC logo\")",
		"g.place(b_v5_logo,     -column 1 -row 20)",
		"g.place(b_ppc_logo,    -column 1 -row 21)",
		"g.place(b_led_control, -column 1 -row 22)",
		NULL);

	dope_bind(menu_app_id, "b_led_control", "commit", led_menu_button_callback,  NULL);
	dope_bind(menu_app_id, "b_v5_logo",     "commit", v5_menu_button_callback,   NULL);
	dope_bind(menu_app_id, "b_ppc_logo",    "commit", ppc_menu_button_callback,  NULL);
}
