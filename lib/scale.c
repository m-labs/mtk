/*
 * \brief   MTK Scale widget module
 *
 * This widget type handles numeric scales.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

struct scale;
#define WIDGET struct scale

#include <stdlib.h>
#include <stdio.h>
#include "mtkstd.h"
#include "scale.h"
#include "button.h"
#include "variable.h"
#include "gfx.h"
#include "widget_data.h"
#include "widget_help.h"
#include "widman.h"
#include "script.h"
#include "userstate.h"
#include "messenger.h"

static struct gfx_services       *gfx;
static struct widman_services    *widman;
static struct button_services    *but;
static struct script_services    *script;
static struct userstate_services *userstate;
static struct messenger_services *msg;

#define SCALE_SIZE 14

struct scale_data {
	u32       type;           /* bitmask of scales properties */
	WIDGET   *slider_bg;      /* slider background            */
	WIDGET   *slider;         /* slider                       */
	VARIABLE *var;            /* associated variable          */
	float     value;          /* current scale value          */
	float     from,to;        /* boundary of the scale value  */
	s16       padx,pady;      /* space around the widget      */
};


int init_scale(struct mtk_services *d);


/********************************
 ** Internal utility functions **
 ********************************/

static s32 calc_slider_pos(float from, float to, float value, s32 size)
{
	if (from == to) return 0;
	return ((value-from)/(to-from))*size;
}

static float calc_slider_value(float from, float to, s32 size, s32 pos)
{
	if (size == 0) return 0;
	return from + (((float)pos)*(to-from))/(float)size;
}


static float check_value(float from, float to, float value)
{
	if (from == to) value = from;
	if (from < to)  {
		if (value < from) value = from;
		if (value > to)   value = to;
	}
	if (from > to) {
		if (value > from) value = from;
		if (value < to)   value = to;
	}
	return value;
}


static WIDGET *new_button(char *txt,void *clic,int context)
{
	BUTTON *nb = but->create();
	nb->but->set_click(nb, clic);
	nb->gen->set_context((WIDGET *)nb, (void *)context);
	nb->gen->set_evforward((WIDGET *)nb, 0);
	nb->gen->set_selectable((WIDGET *)nb, 0);
	nb->but->set_text(nb, txt);
	nb->but->set_pad_x(nb, 0);
	nb->but->set_pad_y(nb, 0);
	return (WIDGET *)nb;
}


/****************************
 ** General widget methods **
 ****************************/

static int scale_draw(SCALE *w, struct gfx_ds *ds, int x, int y, WIDGET *origin)
{
	int x1, y1, x2, y2;
	int ret = 0;

	if (origin == w) return 1;

	x1 = w->wd->x + x;
	y1 = w->wd->y + y;
	x2 = x1 + w->wd->w - 1;
	y2 = y1 + w->wd->h - 1;

	/* draw elements of the scale */
	ret |= w->sd->slider_bg->gen->draw(w->sd->slider_bg, ds, w->wd->x + x, w->wd->y + y, origin);
	ret |= w->sd->slider->gen->draw(w->sd->slider, ds, w->wd->x + x, w->wd->y + y, origin);

	/* do not draw groove if visibility is requested */
	if (origin) return ret;

	if (w->sd->type & SCALE_VER) {
		int x = x1 + w->sd->padx + 4;
		int y = y1 + w->sd->slider->wd->y + (w->sd->slider->wd->h>>1);
		int l = w->sd->slider->wd->w - 4;
		gfx->draw_hline(ds, x, y-1, l, GFX_RGBA(0,0,0,127));
		gfx->draw_hline(ds, x, y+1, l, GFX_RGBA(255,255,255,127));
		ret |= 1;
	} else {
		int x = x1 + w->sd->slider->wd->x + (w->sd->slider->wd->w>>1);
		int y = y1 + w->sd->pady + 4;
		int l = w->sd->slider->wd->h - 4;
		gfx->draw_vline(ds, x-1, y, l, GFX_RGBA(0,0,0,127));
		gfx->draw_vline(ds, x+1, y, l, GFX_RGBA(255,255,255,127));
		ret |= 1;
	}

	return ret;
}


/**
 * Set sizes and positions of scale control elements
 */
static void refresh_elements(SCALE *w)
{
	s32 sbg_size;
	s32 sld_pos;
	s32 sld_size;
	WIDGET *slider;
	WIDGET *slider_bg;

	if (!w) return;

	slider = w->sd->slider;
	slider_bg = w->sd->slider_bg;

//
//	if (w->wd->w > w->wd->max_w) w->wd->w = w->wd->max_w;
//	if (w->wd->w < w->wd->min_w) w->wd->w = w->wd->min_w;
//	if (w->wd->h > w->wd->max_h) w->wd->h = w->wd->max_h;
//	if (w->wd->h < w->wd->min_h) w->wd->h = w->wd->min_h;

	if (w->sd->type & SCALE_VER) {
		sbg_size = w->wd->h - w->sd->pady*2;
		sld_size = (w->wd->w - 2*2 - w->sd->padx*2) * 3;
		sld_pos  = calc_slider_pos(w->sd->from, w->sd->to, w->sd->value, sbg_size-sld_size-4);

		slider->gen->set_x(slider, w->sd->padx + 2);
		slider->gen->set_y(slider, w->sd->pady + 2 + sld_pos);
		slider->gen->set_w(slider, w->wd->w - 2*2 - w->sd->padx*2);
		slider->gen->set_h(slider, sld_size);
		slider->gen->updatepos(slider);

		slider_bg->gen->set_x(slider_bg, w->sd->padx);
		slider_bg->gen->set_y(slider_bg, w->sd->pady);
		slider_bg->gen->set_w(slider_bg, w->wd->w - w->sd->padx*2);
		slider_bg->gen->set_h(slider_bg, sbg_size);
		slider_bg->gen->updatepos(slider_bg);
	} else {

		sbg_size = w->wd->w - w->sd->padx*2;
		sld_size = (w->wd->h - 2*2 - w->sd->pady*2) * 3;
		sld_pos  = calc_slider_pos(w->sd->from, w->sd->to, w->sd->value, sbg_size-sld_size-4);

		slider->gen->set_x(slider,w->sd->padx + 2 + sld_pos);
		slider->gen->set_y(slider,w->sd->pady + 2);
		slider->gen->set_w(slider,sld_size);
		slider->gen->set_h(slider,w->wd->h - 2*2 - w->sd->pady*2);
		slider->gen->updatepos(slider);

		slider_bg->gen->set_x(slider_bg, w->sd->padx);
		slider_bg->gen->set_y(slider_bg, w->sd->pady);
		slider_bg->gen->set_w(slider_bg, sbg_size);
		slider_bg->gen->set_h(slider_bg, w->wd->h - w->sd->pady*2);
		slider_bg->gen->updatepos(slider_bg);
	}
}


/**
 * Update scale after change of attributes
 *
 * If the change of attributes affects the size boundaries of
 * the scale, the new layout constraints must be forwarded
 * to the parent.
 */
static void (*orig_updatepos)(WIDGET *w);
static void scale_updatepos(SCALE *s)
{
	refresh_elements(s);
	orig_updatepos(s);

	/* we refresh the scale each time an attribute changed */
	s->wd->update |= WID_UPDATE_REFRESH;
}


/**
 * Determine min/max size of scale widget
 */
static void scale_calc_minmax(SCALE *s)
{
	if (s->sd->type & SCALE_VER) {
		s->wd->min_w = s->wd->max_w = SCALE_SIZE + s->sd->padx*2;
		s->wd->min_h = s->sd->pady*2 + SCALE_SIZE*4;
		s->wd->max_h = 99999;
	} else {
		s->wd->min_w = s->sd->padx*2 + SCALE_SIZE*4;
		s->wd->max_w = 99999;
		s->wd->min_h = s->wd->max_h = SCALE_SIZE + s->sd->pady*2;
	}
}


static WIDGET *scale_find(SCALE *w,int x,int y)
{
	WIDGET *result;

	if (!w) return NULL;

	/* check if position is inside the scale */
	if ((x >= w->wd->x) && (x < w->wd->x+w->wd->w)
	 && (y >= w->wd->y) && (y < w->wd->y+w->wd->h)) {

		/* we are hit - lets check our children */
		result = w->sd->slider->gen->find(w->sd->slider, x-w->wd->x, y-w->wd->y);
		if (result) return result;

		result = w->sd->slider_bg->gen->find(w->sd->slider_bg, x-w->wd->x, y-w->wd->y);
		if (result) return result;
	}
	return NULL;
}


/**
 * Add event binding
 *
 * We have to bind the event to the child widgets of the scale.
 */
static void (*orig_bind)(WIDGET *w, char *bind_ident, char *message);
static void scale_bind(SCALE *s, char *bind_ident, char *message)
{
	WIDGET *cw;
	
	if ((cw = s->sd->slider))    cw->gen->bind(cw, bind_ident, message);
	if ((cw = s->sd->slider_bg)) cw->gen->bind(cw, bind_ident, message);
	orig_bind(s, bind_ident, message);
}


/**
 * Propagate application id to child widgets
 */
static void (*orig_set_app_id)(WIDGET *w, s32 new_app_id);
static void scale_set_app_id(SCALE *s, s32 new_app_id)
{
	WIDGET *cw;

	if ((cw = s->sd->slider))    cw->gen->set_app_id(cw, new_app_id);
	if ((cw = s->sd->slider_bg)) cw->gen->set_app_id(cw, new_app_id);
	orig_set_app_id(s, new_app_id);
}


/**
 * Free scale widget data
 */
static void scale_free_data(SCALE *s)
{
	WIDGET *cw;
	if ((cw = s->sd->slider_bg))     cw->gen->dec_ref(cw);
	if ((cw = s->sd->slider))        cw->gen->dec_ref(cw);
	if ((cw = (WIDGET *)s->sd->var)) cw->gen->dec_ref(cw);
}


/**
 * Return widget type identifier
 */
static char *scale_get_type(SCALE *s)
{
	return "Scale";
}


/****************************
 ** Scale specific methods **
 ****************************/


/**
 * Get/set orientation of scale (vertical or horizontal)
 */
static void scale_set_orient_bit (SCALE *s,u32 new_type)
{
	if (new_type == s->sd->type) return;
	s->sd->type = new_type;
	s->wd->update |= WID_UPDATE_MINMAX;
}
static u32 scale_get_orient_bit (SCALE *s)
{
	return s->sd->type;
}


/**
 * Get/set scale value
 */
static void scale_set_value(SCALE *s,float new_value)
{
	static char strbuf[24];
	int app_id;
	char *m;

	s->sd->value = check_value(s->sd->from, s->sd->to, new_value);
	mtk_ftoa(s->sd->value, 2, strbuf, 24);
	if (s->sd->var) s->sd->var->var->set_string(s->sd->var, &strbuf[0]);
	
	/* notify client that bound an "change"-event */
	m = s->gen->get_bind_msg(s, "change");
	app_id = s->gen->get_app_id(s);
	if (m) msg->send_action_event(app_id, "change", m);

	s->wd->update |= WID_UPDATE_REFRESH;
}
static float scale_get_value (SCALE *s)
{
	return s->sd->value;
}


/**
 * Get/set min scale value
 */
static void scale_set_from(SCALE *s,float new_from)
{
	s->sd->from = new_from;
	s->sd->value = check_value(s->sd->from, s->sd->to, s->sd->value);
	s->wd->update |= WID_UPDATE_REFRESH;
}
static float scale_get_from(SCALE *s)
{
	return s->sd->from;
}


/**
 * Get/set max scale value
 */
static void scale_set_to(SCALE *s,float new_to)
{
	s->sd->to= new_to;
	s->sd->value = check_value(s->sd->from, s->sd->to, s->sd->value);
	s->wd->update |= WID_UPDATE_REFRESH;
}
static float scale_get_to(SCALE *s)
{
	return s->sd->to;
}


static void scale_var_notify(SCALE *s, VARIABLE *v)
{
	static int depth;
	if (depth == 0) {
		depth++;
//		s->scale->set_value(s, strtod(v->var->get_string(v), (char **)NULL));
		scale_set_value(s, atof(v->var->get_string(v)));
		depth--;
	}
	s->gen->update((WIDGET *)s);
}


/**
 * Connect/disconnect to variable
 */
static void scale_set_var(SCALE *s, VARIABLE *v)
{
	if (s->sd->var) {
		s->sd->var->gen->dec_ref((WIDGET *)s->sd->var);
		s->sd->var->var->disconnect(s->sd->var, s);
	}

	if (!v) return;

	s->sd->var = v;
	v->gen->inc_ref((WIDGET *)v);
	v->var->connect(v, s, scale_var_notify);
	scale_set_value(s, atof(v->var->get_string(v)));
}
static VARIABLE *scale_get_var(SCALE *s)
{
	return s->sd->var;
}


static struct widget_methods gen_methods;


/***********************
 ** Service functions **
 ***********************/

/**
 * Variables for userstate handling
 */
static int    osx, osy;         /* slider position when begin of dragging */
static SCALE *curr_scale;       /* currently modified scale widget */

static void slider_motion_callback(WIDGET *w, int dx, int dy)
{
	float from, to, value;
	s32 pos, size, app_id;
	char *m;

	if (!(scale_get_orient_bit(curr_scale) & SCALE_VER)) {
		pos  = osx + dx;
		size = curr_scale->gen->get_w(curr_scale) - w->gen->get_w(w);
	} else {
		pos  = osy + dy;
		size = curr_scale->gen->get_h(curr_scale) - w->gen->get_h(w);
	}

	/* determine the new scale value */
	from  = curr_scale->sd->from;
	to    = curr_scale->sd->to;
	value = calc_slider_value(from, to, size, pos);

	scale_set_value(curr_scale, value);

	m = curr_scale->gen->get_bind_msg(curr_scale, "slide");
	app_id = curr_scale->gen->get_app_id(curr_scale);
	if (m) msg->send_action_event(app_id, "slide", m);

	curr_scale->wd->update |= WID_UPDATE_REFRESH;
	curr_scale->gen->update((WIDGET *)curr_scale);
}


static void slider_release_callback(WIDGET *s, int dx, int dy)
{
	char *m = curr_scale->gen->get_bind_msg(curr_scale, "slid");
	int app_id = curr_scale->gen->get_app_id(curr_scale);
	if (m) msg->send_action_event(app_id, "slid", m);
}



static void slider_callback(BUTTON *b)
{
	if (!b) return;
	curr_scale = b->gen->get_parent((WIDGET *)b);
	osx = b->gen->get_x((WIDGET *)b);
	osy = b->gen->get_y((WIDGET *)b);
	userstate->drag((WIDGET *)b, slider_motion_callback, NULL, slider_release_callback);
}


static void slider_bg_callback(BUTTON *b)
{
	SCALE  *s;
	u16 dir = 1;        /* default right/bottom direction */

	if (!b) return;
	s = (SCALE *)b->gen->get_parent((WIDGET *)b);
	if (!s) return;

	/* check if mouse is on the left or right side of the slider */
	if (s->sd->type & SCALE_VER) {
		if (userstate->get_my() < s->sd->slider->gen->get_abs_y(s->sd->slider)) dir = 0;
	} else {
		if (userstate->get_mx() < s->sd->slider->gen->get_abs_x(s->sd->slider)) dir = 0;
	}

	/* change view offset dependent on the scroll direction */
//  if (dir) {
//      s->view_offset += s->view_size;
//      if (s->view_offset > s->real_size - s->view_size)
//          s->view_offset = s->real_size - s->view_size;
//  } else {
//      s->view_offset -= s->view_size;
//      if (s->view_offset < 0) s->view_offset = 0;
//  }

	s->gen->update(s);
}


static SCALE *create(void)
{
	SCALE *new = ALLOC_WIDGET(struct scale);
	SET_WIDGET_DEFAULTS(new, struct scale, NULL);

	/* default scale attributes */
	new->sd->value = 50;
	new->sd->to    = 100;
	new->sd->padx  = 2;
	new->sd->pady  = 2;

	/* create scale element widgets */
	new->sd->slider    = new_button(NULL,slider_callback,0);
	new->sd->slider_bg = new_button(NULL,slider_bg_callback,0);

	new->sd->slider_bg->gen->set_state(new->sd->slider_bg,1);
	new->sd->slider->gen->set_parent(new->sd->slider,new);
	new->sd->slider_bg->gen->set_parent(new->sd->slider_bg,new);

	refresh_elements(new);
	scale_calc_minmax(new);
	return new;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct scale_services services = {
	create
};


/**********************
 ** Command language **
 **********************/

static void scale_set_orient(SCALE *s, char *orient)
{
	if (!strcmp("vertical",orient)) {
		scale_set_orient_bit(s, scale_get_orient_bit(s) | SCALE_VER);
		return;
	}
	if (!strcmp("horizontal",orient)) {
		scale_set_orient_bit(s, scale_get_orient_bit(s) & (-1^SCALE_VER));
		return;
	}
}

static char *scale_get_orient(SCALE *s)
{
	if (scale_get_orient_bit(s) & SCALE_VER) return "vertical";
	else return "horizontal";
}

static void build_script_lang(void)
{
	void *widtype;

	widtype = script->reg_widget_type("Scale",(void *(*)(void))create);

	script->reg_widget_attrib(widtype,"Variable variable",scale_get_var,scale_set_var,gen_methods.update);
	script->reg_widget_attrib(widtype,"float value",scale_get_value,scale_set_value,gen_methods.update);
	script->reg_widget_attrib(widtype,"float from",scale_get_from,scale_set_from,gen_methods.update);
	script->reg_widget_attrib(widtype,"float to",scale_get_to,scale_set_to,gen_methods.update);
	script->reg_widget_attrib(widtype,"string orient",scale_get_orient,scale_set_orient,gen_methods.update);

	widman->build_script_lang(widtype,&gen_methods);
}


/************************
 ** Module entry point **
 ************************/

int init_scale(struct mtk_services *d)
{
	gfx       = d->get_module("Gfx 1.0");
	but       = d->get_module("Button 1.0");
	script    = d->get_module("Script 1.0");
	widman    = d->get_module("WidgetManager 1.0");
	userstate = d->get_module("UserState 1.0");
	msg       = d->get_module("Messenger 1.0");

	/* define general widget functions */
	widman->default_widget_methods(&gen_methods);

	orig_bind                = gen_methods.bind;
	orig_set_app_id          = gen_methods.set_app_id;
	orig_updatepos           = gen_methods.updatepos;

	gen_methods.get_type     = scale_get_type;
	gen_methods.draw         = scale_draw;
	gen_methods.updatepos    = scale_updatepos;
	gen_methods.find         = scale_find;
	gen_methods.bind         = scale_bind;
	gen_methods.set_app_id   = scale_set_app_id;
	gen_methods.calc_minmax  = scale_calc_minmax;
	gen_methods.free_data    = scale_free_data;

	build_script_lang();

	d->register_module("Scale 1.0",&services);
	return 1;
}
