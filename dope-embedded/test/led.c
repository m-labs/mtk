/*
 * \brief  DOpE-embedded demo application (LED control window)
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

/* DOpE includes */
#include <dopelib.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * LED control mode
 */
static enum {
	LEDS_SOURCE_NONE,        /* LEDs are off */
	LEDS_SOURCE_BUTTONS,     /* LEDs controlled by GUI buttons */
	LEDS_SOURCE_SCALE,       /* LEDs controlled by GUI scale widget */
} source_select;

static int led_state;        /* current LED state */
static int led_state_scale;  /* state of LED scale widget */
static int num_leds;         /* number of LEDs to control */

static void (*set_led_state) (unsigned);  /* state-change callback */

static int app_id;           /* DOpE application ID for LED window */


/**
 * Callback: called when the source is selected
 */
static void button_select_callback(dope_event *e, void *arg)
{
	if ((int)arg == 0) {/* select single bits */
		if (source_select != LEDS_SOURCE_BUTTONS) {
			dope_cmdf(app_id,"b_select.set(-state 1)");
			dope_cmdf(app_id,"b_slide.set(-state 0)");
			source_select = LEDS_SOURCE_BUTTONS;
			if (set_led_state)
				set_led_state(led_state);
		}
		else {
			/* switch off the LEDS */
			source_select = LEDS_SOURCE_NONE;
			dope_cmdf(app_id,"b_select.set(-state 0)");
			if (set_led_state)
				set_led_state(0);
		}
	}
	else { /* use slider instead */
		if (source_select != LEDS_SOURCE_SCALE) {
			dope_cmdf(app_id, "b_select.set(-state 0)");
			dope_cmdf(app_id, "b_slide.set(-state 1)");
			source_select = LEDS_SOURCE_SCALE;
			if (set_led_state)
				set_led_state(led_state_scale);
		}
		else {
			/* switch off the LEDS */
			source_select = LEDS_SOURCE_NONE;
			dope_cmdf(app_id, "b_slide.set(-state 0)");
			if (set_led_state)
				set_led_state(0);
		}
	}
}


/**
 * Callback: called when an LED button was pressed
 */
static void led_button_callback(dope_event *e, void *arg)
{
	int arg_int = (int) arg;
	int arg_pow2 = 1 << (arg_int-1);

	/* toggle the led state of the selected LED */
	led_state = led_state ^ arg_pow2;

	/* if the buttons are the current source write to LEDs */
	if (source_select == LEDS_SOURCE_BUTTONS && set_led_state)
		set_led_state(led_state);

	/* tell the GUI to switch the state of the button */
	dope_cmdf(app_id, "b%d.set(-state %d)", arg_int,
	          ((led_state & arg_pow2) >> (arg_int-1)) & 1 );
}


/**
 * Callback: called when the slider was moved
 */
static void led_scale_callback(dope_event *e, void *arg)
{
	char buf[10];

	/* request the value from the GUI and update the value drawn */
	dope_req(app_id, buf, sizeof(buf), "led_scale.value");
	led_state_scale = atoi(buf);
	dope_cmdf(app_id, "led_value.set(-text \"%d\")", led_state_scale);

	/* if the slider is the current source write to LEDs */
	if(source_select == LEDS_SOURCE_SCALE && set_led_state)
		set_led_state(led_state_scale);
}


/**
 * Callback: called when the window-close button got pressed
 */
static void close_callback(dope_event *e, void *arg)
{
	dope_cmd(app_id, "w.close()");
}


/*************************
 ** Interface functions **
 *************************/

void init_led_window(int num,
                     void (*callback)(unsigned new_state))
{
	int i;

	num_leds      = num;
	set_led_state = callback;

	app_id = dope_init_app("LED Control");

	dope_cmd_seq(app_id,
		"b_slide    = new Button(-text \"Slider\")",
		"b_select   = new Button(-text \"Buttons\")",
		"led_scale  = new Scale()",
		"led_value  = new Label(-text \"0\")",
		"mode       = new Label(-text \"Select Source:\")",
		"text       = new Label(-text \"Current status: \")",
		"status_off = new Variable(-value \"Off...\")",
		"status_on  = new Variable(-value \"On...\")",
		NULL);

	dope_cmdf(app_id, "led_scale.set(-from 0 -to %d -value 0.0)", 1 << num_leds);
	dope_bind(app_id, "led_scale", "change", led_scale_callback,     NULL);
	dope_bind(app_id, "b_select",  "press",  button_select_callback, (void *)0);
	dope_bind(app_id, "b_slide" ,  "press",  button_select_callback, (void *)1);

	/* create g_led grid */
	dope_cmd(app_id, "g_led = new Grid()");
	for (i = 1; i <= num_leds; i++) {
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
		"w = new Window(-content g -workx 25 -worky 60)",
		NULL);

	dope_bind(app_id, "w", "close",  close_callback, NULL);
}


void open_led_window(void)
{
	dope_cmd(app_id, "w.open()");
	dope_cmd(app_id, "w.top()");
}
