/*
 * \brief  DOpE-embedded demo application
 * \author Matthias Alles
 * \author Norman Feske
 * \date   2008-08-17
 */

/*
 * Copyright (C) 2008 Matthias Alles
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* generic */
#include <stdio.h>
#include <stdlib.h>

/* dope */
#include "dopelib.h"
#include "vscreen.h"

/* logos */
#include "xilinx_logo.h"
#include "genode-labs-banner.h"

/* platform interface */
#include "platform.h"

/* local includes */
#include "disp_img.h"

/**
 * LED control mode
 */
static enum {
	LEDS_SOURCE_NONE,        /* LEDs are off */
	LEDS_SOURCE_BUTTONS,     /* LEDs controlled by GUI buttons */
	LEDS_SOURCE_SCALE,       /* LEDs controlled bz GUI scale widget */
} source_select;

static int led_app_id;       /* DOpE application ID for the LED control window */
static int led_state;        /* current LED state */
static int led_state_scale;  /* state of LED scale widget */


/**
 * Called when the source is selected
 */
static void button_select_callback(dope_event *e, void *arg)
{
	if ((int)arg == 0) {/* select single bits */
		if (source_select != LEDS_SOURCE_BUTTONS) {
			dope_cmdf(led_app_id,"b_select.set(-state 1)");
			dope_cmdf(led_app_id,"b_slide.set(-state 0)");
			source_select = LEDS_SOURCE_BUTTONS;
			set_leds(led_state);
		}
		else {
			/* switch off the LEDS */
			source_select = LEDS_SOURCE_NONE;
			dope_cmdf(led_app_id,"b_select.set(-state 0)");
			set_leds(0);
		}
	}
	else { /* use slider instead */
		if (source_select != LEDS_SOURCE_SCALE) {
			dope_cmdf(led_app_id,"b_select.set(-state 0)");
			dope_cmdf(led_app_id,"b_slide.set(-state 1)");
			source_select = LEDS_SOURCE_SCALE;
			set_leds(led_state_scale);
		}
		else {
			/* switch off the LEDS */
			source_select = LEDS_SOURCE_NONE;
			dope_cmdf(led_app_id,"b_slide.set(-state 0)");
			set_leds(0);
		}
	}
}


/**
 * Called when an LED button was pressed
 */
static void led_button_callback(dope_event *e, void *arg)
{
	int arg_int = (int) arg;
	int arg_pow2 = 1 << (arg_int-1);

	/* toggle the led state of the selected LED */
	led_state = led_state ^ arg_pow2;

	/* if the buttons are the current source write to LEDs */
	if (source_select == LEDS_SOURCE_BUTTONS)
		set_leds(led_state);

	/* tell the GUI to switch the state of the button */
	dope_cmdf(led_app_id,"b%d.set(-state %d)", arg_int,
	          ((led_state & arg_pow2) >> (arg_int-1)) & 1 );
}


/**
 * Called when the slider was moved
 */
static void led_scale_callback(dope_event *e, void *arg)
{
	char buf[10];

	/* request the value from the GUI and update the value drawn */
	dope_req(led_app_id, buf, sizeof(buf), "led_scale.value");
	led_state_scale = atoi(buf);
	dope_cmdf(led_app_id, "led_value.set(-text \"%d\")", led_state_scale);

	/* if the slider is the current source write to LEDs */
	if(source_select == LEDS_SOURCE_SCALE)
		set_leds(led_state_scale);
}


/**
 * Display 4096 Colors
 *
 * \return  DOpE application ID
 */
static int draw_colors()
{
	int i, app_id;
	int blue = 0, green = 0, red = 0;
	short *pixbuf;

	app_id = dope_init_app("4096 Colors");

	dope_cmd_seq(app_id,
		"vs = new VScreen()",
		"vs.setmode(64, 64, RGB16)",
		"gr = new Grid()",
		"gr.place(vs, -column 1 -row 1)",
		"fpga = new Window(-content vs -workx 570 -worky 450 -workw 128 -workh 128)",
		"fpga.open()",
		NULL);

	pixbuf = (short *)vscr_get_fb(app_id, "vs");

	for (i = 0; i < 64*64; i++) {
		pixbuf[i] = red << 11 | green << 5 | blue;
		if (blue == 30) {
			blue = 0;
			green += 4;
		} else
			blue += 2;

		if (green == 64) {
			green = 0;
			red += 2;
		}
	}

	return app_id;
}


/**
 * LED handling
 *
 * \return DOpE application ID
 */
static int draw_led_control()
{
	int i, app_id = dope_init_app("LED Control");

	dope_cmd_seq(app_id,
		"b_slide    = new Button(-text \"Slider\")",
		"b_select   = new Button(-text \"Buttons\")",
		"led_scale  = new Scale(-from 0 -to 256 -value 0.0)",
		"led_value  = new Label(-text \"0\")",
		"mode       = new Label(-text \"Select Source:\")",
		"text       = new Label(-text \"Current status: \")",
		"status_off = new Variable(-value \"Off...\")",
		"status_on  = new Variable(-value \"On...\")",
		NULL);

	dope_bind(app_id, "led_scale", "change", led_scale_callback, NULL);
	dope_bind(app_id, "b_select", "press", button_select_callback, (void *)0);
	dope_bind(app_id, "b_slide" , "press", button_select_callback, (void *)1);

	/* create g_led grid */
	dope_cmd(app_id, "g_led = new Grid()");
	for (i = 1; i <= 8; i++) {
		char label[8];
		snprintf(label, sizeof(label), "L%d", i);
		dope_cmdf(app_id, "b%d = new Button(-text \"%s\")", i, label);
		dope_cmdf(app_id, "g_led.place(b%d, -column %d -row 1)", i, i);
		dope_bindf(app_id, "b%d", "press", led_button_callback, (void *)i, i);
	}

	/* create main grid */
	dope_cmd_seq(app_id,
		"g = new Grid()",
		"g.place(mode,      -column 1 -row 2)",
		"g.place(b_slide,   -column 1 -row 3)",
		"g.place(led_scale, -column 2 -row 3)",
		"g.place(b_select,  -column 1 -row 4)",
		"g.place(g_led,     -column 2 -row 4)",
		"g.place(led_value, -column 3 -row 3)",
		"g.columnconfig(1, -size 64)",
		"g.columnconfig(3, -size 64)",
		NULL);

	/* create window displaying the main grid */
	dope_cmd(app_id, "a = new Window(-content g -workx 25 -worky 60)");
	dope_cmd(app_id, "a.open()");

	return app_id;
}

int main(int argc, char **argv)
{
	init_platform();

	/* initialize the GUI */
	printf("Initializing GUI...");
	dope_init();

	/* setup windows */
	printf("Setup windows...\n");
	draw_colors();
	led_app_id = draw_led_control();

	display_platform_images();
	display_image("Xilinx", pixel_data_xilinx, 570, 300, 145, 43);
	display_image("Genode Labs", pixel_data_genode, 420, 50, 232, 84);

	/* run the GUI */
	printf("Running GUI...\n");
	dope_eventloop(led_app_id);

	return 0;
}
