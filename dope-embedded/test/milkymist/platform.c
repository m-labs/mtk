/*
 * \brief  Platform for Milkymist
 * \author Sebastien Bourdeauducq / Norman Feske
 * \date   2009-08-06
 */


/* local includes */
#include "platform.h"
#include "disp_img.h"
#include "led.h"
#include "gfx/spartan3e.h"
#include "gfx/microblaze_logo.h"
#include "gfx/kolt_logo.h"

/* DOpE includes */
#include <dopelib.h>

#include <bsp.h>
#include <pthread.h>
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
	/* TODO */
}


void init_platform(void)
{
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

int main(int argc, char **argv);

void *POSIX_Init(void *argument)
{
	main(0, NULL);
	return NULL;
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
#define CONFIGURE_EXTRA_TASK_STACKS 1900
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4
#define CONFIGURE_MAXIMUM_POSIX_THREADS         1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 1
#define CONFIGURE_INIT
#include <rtems/confdefs.h>
