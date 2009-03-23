/*
 * \brief  DOpE-embedded demo application (settings window)
 * \author Norman Feske
 * \date   2009-03-09
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

/* local includes */
#include "settings.h"
#include "util.h"

static int app_id;  /* DOpE application ID used for the settings window */


/**
 * Global configuration variables of the DOpE library
 */
extern unsigned long config_bg_desk_color;
extern unsigned long config_bg_win_color;
extern unsigned long config_redraw_granularity;
extern int           config_transparency;
extern int           config_dropshadows;


/**
 * Variables used for the backup of the original configuration values
 */
static unsigned long default_bg_desk_color;
static unsigned long default_bg_win_color;
static int           default_transparency;
static int           default_dropshadows;
static unsigned      default_redraw_granularity;


/**
 * Utilities for accessing the components of a color value
 */
static unsigned long   red(unsigned long color) { return (color >> 24) & 0xff; }
static unsigned long green(unsigned long color) { return (color >> 16) & 0xff; }
static unsigned long  blue(unsigned long color) { return (color >>  8) & 0xff; }


/**
 * Utility for composing a color from its components
 */
static unsigned long rgb_color(unsigned long r, unsigned long g, unsigned long b)
{
	return ((b & 0xff) << 8) | ((g & 0xff) << 16) | ((r & 0xff) << 24) | 0xff;
}


/**
 * Update on/off buttons according to the current config variables
 */
static void update_on_off_buttons(void)
{
	dope_cmdf(app_id, "trans_on.set(-state %s)",    config_transparency ? "on"  : "off");
	dope_cmdf(app_id, "trans_off.set(-state %s)",   config_transparency ? "off" :  "on");
	dope_cmdf(app_id, "shadows_on.set(-state %s)",  config_dropshadows  ? "on"  : "off");
	dope_cmdf(app_id, "shadows_off.set(-state %s)", config_dropshadows  ? "off" :  "on");
}


/**
 * Preset useful redraw-granularity values for different effect settings
 */
static void preset_redraw_granularity(void)
{
	if (config_transparency) {
		if (config_dropshadows)
			dope_cmdf(app_id, "redraw_var.set(-value 16)");
		else
			dope_cmdf(app_id, "redraw_var.set(-value 40)");
	} else {
		if (config_dropshadows)
			dope_cmdf(app_id, "redraw_var.set(-value 150)");
		else
			dope_cmdf(app_id, "redraw_var.set(-value 300)");
	}
}


/**
 * Callback: called when enabling/disabling transparency
 *
 * \param arg  1 for enable transparency,
 *             0 for disable transparency
 */
static void set_trans_callback(dope_event *e, void *arg)
{
	config_transparency = (int)arg;
	update_on_off_buttons();
	dope_cmd(app_id, "screen.refresh()");
	preset_redraw_granularity();
}


/**
 * Callback: called when enabling/disabling drop shadows
 *
 * \param arg  1 for enable drop shadows,
 *             0 for disable drop shadows
 */
static void set_shadows_callback(dope_event *e, void *arg)
{
	config_dropshadows = (int)arg;
	update_on_off_buttons();
	dope_cmd(app_id, "screen.refresh()");
	preset_redraw_granularity();
}


/**
 * Callback: called when the reset-to-defaults button got pressed
 */
static void reset_callback(dope_event *e, void *arg)
{
	/* revert configuration to backed up values */
	config_transparency       = default_transparency;
	config_dropshadows        = default_dropshadows;
	config_redraw_granularity = default_redraw_granularity;

	update_on_off_buttons();

	/*
	 * By setting the value of a variable widget, the connected scale
	 * widget automatically takes over the new value and emits a 'change'
	 * event, which then gets handled by the 'set_colors_callback'.
	 * Hence, all we have to do here is to set the variables to their
	 * backed-up default values.
	 */
	dope_cmdf(app_id, "desk_r_var.set(-value %d)", red  (default_bg_desk_color));
	dope_cmdf(app_id, "desk_g_var.set(-value %d)", green(default_bg_desk_color));
	dope_cmdf(app_id, "desk_b_var.set(-value %d)", blue (default_bg_desk_color));
	dope_cmdf(app_id, "win_r_var.set(-value %d)",  red  (default_bg_win_color));
	dope_cmdf(app_id, "win_g_var.set(-value %d)",  green(default_bg_win_color));
	dope_cmdf(app_id, "win_b_var.set(-value %d)",  blue (default_bg_win_color));
	dope_cmdf(app_id, "redraw_var.set(-value %d)", config_redraw_granularity/1000);
}


/**
 * Callback: called when a color scale widget was changed
 */
static void set_colors_callback(dope_event *e, void *arg)
{
	unsigned long r, g, b;

	r = dope_req_l(app_id, "desk_r_var.value");
	g = dope_req_l(app_id, "desk_g_var.value");
	b = dope_req_l(app_id, "desk_b_var.value");

	config_bg_desk_color = rgb_color(r, g, b);

	r = dope_req_l(app_id, "win_r_var.value");
	g = dope_req_l(app_id, "win_g_var.value");
	b = dope_req_l(app_id, "win_b_var.value");

	config_bg_win_color = rgb_color(r, g, b);

	dope_cmd(app_id, "screen.refresh()");
}


/**
 * Callback: called when the redraw-granularity scale widget was changed
 */
static void set_redraw_granularity_callback(dope_event *e, void *arg)
{
	unsigned long v;

	v = dope_req_l(app_id, "redraw_var.value");

	config_redraw_granularity = v*1000;
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

void init_settings(void)
{
	app_id = dope_init_app("Settings");

	default_bg_desk_color      = config_bg_desk_color;
	default_bg_win_color       = config_bg_win_color;
	default_transparency       = config_transparency;
	default_dropshadows        = config_dropshadows;
	default_redraw_granularity = config_redraw_granularity;

	dope_cmd_seq(app_id,
		"g = new Grid()",

		/* rgb scale for desktop background color */
		"l = new Label(-text \"Desktop background\")",
		"g.place(l, -row 1 -column 1 -align ns)",
		"desk_r_label = new Label(-text red)",
		"desk_g_label = new Label(-text green)",
		"desk_b_label = new Label(-text blue)",
		"desk_r_var   = new Variable()",
		"desk_g_var   = new Variable()",
		"desk_b_var   = new Variable()",
		"desk_r_scale = new Scale(-from 0 -to 255 -variable desk_r_var)",
		"desk_g_scale = new Scale(-from 0 -to 255 -variable desk_g_var)",
		"desk_b_scale = new Scale(-from 0 -to 255 -variable desk_b_var)",
		"desk_r_val   = new Label(-variable desk_r_var)",
		"desk_g_val   = new Label(-variable desk_g_var)",
		"desk_b_val   = new Label(-variable desk_b_var)",
		"desk_rgb_grid = new Grid()",
		"desk_rgb_grid.place(desk_r_label, -row 1 -column 1 -align e)",
		"desk_rgb_grid.place(desk_r_scale, -row 1 -column 2)",
		"desk_rgb_grid.place(desk_r_val,   -row 1 -column 3)",
		"desk_rgb_grid.place(desk_g_label, -row 2 -column 1 -align e)",
		"desk_rgb_grid.place(desk_g_scale, -row 2 -column 2)",
		"desk_rgb_grid.place(desk_g_val,   -row 2 -column 3)",
		"desk_rgb_grid.place(desk_b_label, -row 3 -column 1 -align e)",
		"desk_rgb_grid.place(desk_b_scale, -row 3 -column 2)",
		"desk_rgb_grid.place(desk_b_val,   -row 3 -column 3)",
		"desk_rgb_grid.columnconfig(1, -size 80)",
		"desk_rgb_grid.columnconfig(3, -size 80)",
		"g.place(desk_rgb_grid, -row 2 -column 1)",

		/* rgb scale for window background color */
		"l = new Label(-text \"Window background\")",
		"g.place(l, -row 4 -column 1 -align ns)",
		"win_r_label = new Label(-text red)",
		"win_g_label = new Label(-text green)",
		"win_b_label = new Label(-text blue)",
		"win_r_var   = new Variable()",
		"win_g_var   = new Variable()",
		"win_b_var   = new Variable()",
		"win_r_scale = new Scale(-from 0 -to 255 -variable win_r_var)",
		"win_g_scale = new Scale(-from 0 -to 255 -variable win_g_var)",
		"win_b_scale = new Scale(-from 0 -to 255 -variable win_b_var)",
		"win_r_val   = new Label(-variable win_r_var)",
		"win_g_val   = new Label(-variable win_g_var)",
		"win_b_val   = new Label(-variable win_b_var)",
		"win_rgb_grid = new Grid()",
		"win_rgb_grid.place(win_r_label, -row 1 -column 1 -align e)",
		"win_rgb_grid.place(win_r_scale, -row 1 -column 2)",
		"win_rgb_grid.place(win_r_val,   -row 1 -column 3)",
		"win_rgb_grid.place(win_g_label, -row 2 -column 1 -align e)",
		"win_rgb_grid.place(win_g_scale, -row 2 -column 2)",
		"win_rgb_grid.place(win_g_val,   -row 2 -column 3)",
		"win_rgb_grid.place(win_b_label, -row 3 -column 1 -align e)",
		"win_rgb_grid.place(win_b_scale, -row 3 -column 2)",
		"win_rgb_grid.place(win_b_val,   -row 3 -column 3)",
		"win_rgb_grid.columnconfig(1, -size 80)",
		"win_rgb_grid.columnconfig(3, -size 80)",
		"g.place(win_rgb_grid, -row 5 -column 1)",

		"l = new Label(-text \"Redraw granularity\")",
		"g.place(l, -row 8 -column 1 -align ns)",
		"redraw_label = new Label(-text \"kilopixels\")",
		"redraw_var   = new Variable()",
		"redraw_scale = new Scale(-from 2 -to 300 -variable redraw_var)",
		"redraw_val   = new Label(-variable redraw_var)",
		"redraw_grid  = new Grid()",
		"redraw_grid.place(redraw_label, -row 1 -column 1 -align e)",
		"redraw_grid.place(redraw_scale, -row 1 -column 2)",
		"redraw_grid.place(redraw_val,   -row 1 -column 3)",
		"redraw_grid.columnconfig(3, -size 80)",
		"g.place(redraw_grid, -row 9 -column 1)",

		/* effect settings */
		"l = new Label(-text \"Effects\")",
		"g.place(l, -row 10 -column 1 -align ns)",
		"efx_grid = new Grid()",

		"l = new Label(-text \"Translucent windows\")",
		"efx_grid.place(l, -row 1 -column 1)",
		"trans_on = new Button(-text On)",
		"efx_grid.place(trans_on, -row 1 -column 2)",
		"trans_off = new Button(-text Off)",
		"efx_grid.place(trans_off, -row 1 -column 3)",

		"l = new Label(-text \"Drop shadows\")",
		"efx_grid.place(l, -row 2 -column 1)",
		"shadows_on = new Button(-text On)",
		"efx_grid.place(shadows_on, -row 2 -column 2)",
		"shadows_off = new Button(-text Off)",
		"efx_grid.place(shadows_off, -row 2 -column 3)",
		"g.place(efx_grid, -row 11 -column 1)",

		"reset = new Button(-text \"Reset to defaults\")",
		"g.place(reset, -row 100 -column 1)",
		"w = new Window(-content g)",
		0
	);

	/*
	 * Set initial values of scales and on/off buttons
	 */

	dope_cmdf(app_id, "desk_r_var.set(-value %d)", red  (config_bg_desk_color));
	dope_cmdf(app_id, "desk_g_var.set(-value %d)", green(config_bg_desk_color));
	dope_cmdf(app_id, "desk_b_var.set(-value %d)", blue (config_bg_desk_color));

	dope_cmdf(app_id, "win_r_var.set(-value %d)", red  (config_bg_win_color));
	dope_cmdf(app_id, "win_g_var.set(-value %d)", green(config_bg_win_color));
	dope_cmdf(app_id, "win_b_var.set(-value %d)", blue (config_bg_win_color));

	dope_cmdf(app_id, "redraw_var.set(-value %d)", config_redraw_granularity/1000);

	update_on_off_buttons();

	/*
	 * Bind callback routines to widget events
	 */

	dope_bind(app_id, "desk_r_scale", "change", set_colors_callback, NULL);
	dope_bind(app_id, "desk_g_scale", "change", set_colors_callback, NULL);
	dope_bind(app_id, "desk_b_scale", "change", set_colors_callback, NULL);
	dope_bind(app_id, "win_r_scale",  "change", set_colors_callback, NULL);
	dope_bind(app_id, "win_g_scale",  "change", set_colors_callback, NULL);
	dope_bind(app_id, "win_b_scale",  "change", set_colors_callback, NULL);
	dope_bind(app_id, "redraw_scale", "change", set_redraw_granularity_callback, NULL);

	dope_bind(app_id, "trans_on",    "press", set_trans_callback,   (void *)1);
	dope_bind(app_id, "trans_off",   "press", set_trans_callback,   (void *)0);
	dope_bind(app_id, "shadows_on",  "press", set_shadows_callback, (void *)1);
	dope_bind(app_id, "shadows_off", "press", set_shadows_callback, (void *)0);

	dope_bind(app_id, "reset", "commit", reset_callback, NULL);
	dope_bind(app_id, "w",     "close",  close_callback, NULL);
}


void open_settings_window(void)
{
	dope_cmd(app_id, "w.open()");
	dope_cmd(app_id, "w.top()");
}

