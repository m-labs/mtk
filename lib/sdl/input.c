/*
 * \brief   DOpE pseudo input driver module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 *
 * It uses SDL for requesting the mouse state under
 * Linux.  All other modules should use this one to
 * get information about the mouse state. The hand-
 * ling of mouse cursor and its appeariance is done
 * by the 'Screen' component.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* general includes */
#include <stdlib.h>

/* SDL includes */
#include <SDL/SDL.h>

/* local includes */
#include "dopestd.h"
#include "event.h"
#include "input.h"

/* includes exported by DOpE */
#include "keycodes.h"

int init_input(struct dope_services *d);

/********************************
 ** Functions for internal use **
 ********************************/

/**
 * map sdl keysyms to dope event keycodes
 */
static long map_keycode(SDLKey sk)
{
	switch (sk) {
	case SDLK_BACKSPACE:    return DOPE_KEY_BACKSPACE;
	case SDLK_TAB:          return DOPE_KEY_TAB;
//  case SDLK_CLEAR:        return DOPE_KEY_CLEAR;
	case SDLK_RETURN:       return DOPE_KEY_ENTER;
	case SDLK_PAUSE:        return DOPE_KEY_PAUSE;
	case SDLK_ESCAPE:       return DOPE_KEY_ESC;
	case SDLK_SPACE:        return DOPE_KEY_SPACE;
	case SDLK_COMMA:        return DOPE_KEY_COMMA;
	case SDLK_MINUS:        return DOPE_KEY_MINUS;
	case SDLK_PERIOD:       return DOPE_KEY_DOT;
	case SDLK_SLASH:        return DOPE_KEY_SLASH;
	case SDLK_0:            return DOPE_KEY_0;
	case SDLK_1:            return DOPE_KEY_1;
	case SDLK_2:            return DOPE_KEY_2;
	case SDLK_3:            return DOPE_KEY_3;
	case SDLK_4:            return DOPE_KEY_4;
	case SDLK_5:            return DOPE_KEY_5;
	case SDLK_6:            return DOPE_KEY_6;
	case SDLK_7:            return DOPE_KEY_7;
	case SDLK_8:            return DOPE_KEY_8;
	case SDLK_9:            return DOPE_KEY_9;
	case SDLK_EQUALS:       return DOPE_KEY_EQUAL;
	case SDLK_QUESTION:     return DOPE_KEY_QUESTION;
	case SDLK_BACKSLASH:    return DOPE_KEY_BACKSLASH;
	case SDLK_a:            return DOPE_KEY_A;
	case SDLK_b:            return DOPE_KEY_B;
	case SDLK_c:            return DOPE_KEY_C;
	case SDLK_d:            return DOPE_KEY_D;
	case SDLK_e:            return DOPE_KEY_E;
	case SDLK_f:            return DOPE_KEY_F;
	case SDLK_g:            return DOPE_KEY_G;
	case SDLK_h:            return DOPE_KEY_H;
	case SDLK_i:            return DOPE_KEY_I;
	case SDLK_j:            return DOPE_KEY_J;
	case SDLK_k:            return DOPE_KEY_K;
	case SDLK_l:            return DOPE_KEY_L;
	case SDLK_m:            return DOPE_KEY_M;
	case SDLK_n:            return DOPE_KEY_N;
	case SDLK_o:            return DOPE_KEY_O;
	case SDLK_p:            return DOPE_KEY_P;
	case SDLK_q:            return DOPE_KEY_Q;
	case SDLK_r:            return DOPE_KEY_R;
	case SDLK_s:            return DOPE_KEY_S;
	case SDLK_t:            return DOPE_KEY_T;
	case SDLK_u:            return DOPE_KEY_U;
	case SDLK_v:            return DOPE_KEY_V;
	case SDLK_w:            return DOPE_KEY_W;
	case SDLK_x:            return DOPE_KEY_X;
	case SDLK_y:            return DOPE_KEY_Y;
	case SDLK_z:            return DOPE_KEY_Z;
	case SDLK_DELETE:       return DOPE_KEY_DELETE;

	/* numeric keypad */
	case SDLK_KP0:          return DOPE_KEY_KP0;
	case SDLK_KP1:          return DOPE_KEY_KP1;
	case SDLK_KP2:          return DOPE_KEY_KP2;
	case SDLK_KP3:          return DOPE_KEY_KP3;
	case SDLK_KP4:          return DOPE_KEY_KP4;
	case SDLK_KP5:          return DOPE_KEY_KP5;
	case SDLK_KP6:          return DOPE_KEY_KP6;
	case SDLK_KP7:          return DOPE_KEY_KP7;
	case SDLK_KP8:          return DOPE_KEY_KP8;
	case SDLK_KP9:          return DOPE_KEY_KP9;
	case SDLK_KP_PERIOD:    return DOPE_KEY_KPDOT;
	case SDLK_KP_DIVIDE:    return DOPE_KEY_KPSLASH;
	case SDLK_KP_MULTIPLY:  return DOPE_KEY_KPASTERISK;
	case SDLK_KP_MINUS:     return DOPE_KEY_KPMINUS;
	case SDLK_KP_PLUS:      return DOPE_KEY_KPPLUS;
	case SDLK_KP_ENTER:     return DOPE_KEY_KPENTER;
	case SDLK_KP_EQUALS:    return DOPE_KEY_KPEQUAL;

	/* arrows + home/end pad */
	case SDLK_UP:           return DOPE_KEY_UP;
	case SDLK_DOWN:         return DOPE_KEY_DOWN;
	case SDLK_RIGHT:        return DOPE_KEY_RIGHT;
	case SDLK_LEFT:         return DOPE_KEY_LEFT;
	case SDLK_INSERT:       return DOPE_KEY_INSERT;
	case SDLK_HOME:         return DOPE_KEY_HOME;
	case SDLK_END:          return DOPE_KEY_END;
	case SDLK_PAGEUP:       return DOPE_KEY_PAGEUP;
	case SDLK_PAGEDOWN:     return DOPE_KEY_PAGEDOWN;

	/* function keys */
	case SDLK_F1:           return DOPE_KEY_F1;
	case SDLK_F2:           return DOPE_KEY_F2;
	case SDLK_F3:           return DOPE_KEY_F3;
	case SDLK_F4:           return DOPE_KEY_F4;
	case SDLK_F5:           return DOPE_KEY_F5;
	case SDLK_F6:           return DOPE_KEY_F6;
	case SDLK_F7:           return DOPE_KEY_F7;
	case SDLK_F8:           return DOPE_KEY_F8;
	case SDLK_F9:           return DOPE_KEY_F9;
	case SDLK_F10:          return DOPE_KEY_F10;
	case SDLK_F11:          return DOPE_KEY_F11;
	case SDLK_F12:          return DOPE_KEY_F12;
	case SDLK_F13:          return DOPE_KEY_F13;
	case SDLK_F14:          return DOPE_KEY_F14;
	case SDLK_F15:          return DOPE_KEY_F15;

	/* key state modifier keys */
	case SDLK_NUMLOCK:      return DOPE_KEY_NUMLOCK;
	case SDLK_CAPSLOCK:     return DOPE_KEY_CAPSLOCK;
//  case SDLK_SCROLLOCK:    return DOPE_KEY_SCROLLOCK;
	case SDLK_RSHIFT:       return DOPE_KEY_RIGHTSHIFT;
	case SDLK_LSHIFT:       return DOPE_KEY_LEFTSHIFT;
	case SDLK_RCTRL:        return DOPE_KEY_RIGHTCTRL;
	case SDLK_LCTRL:        return DOPE_KEY_LEFTCTRL;
	case SDLK_RALT:         return DOPE_KEY_RIGHTALT;
	case SDLK_LALT:         return DOPE_KEY_LEFTALT;
	case SDLK_RMETA:        return DOPE_KEY_RIGHTALT;
	case SDLK_LMETA:        return DOPE_KEY_LEFTALT;

	default: return 0;
	}
	return 0;
}


/***********************
 ** Service functions **
 ***********************/

/**
 * Get next event of event queue
 *
 * \return  0 if there is no pending event
 *          1 if there an event was returned in out parameter e
 */
static int get_event(EVENT *e)
{
	static SDL_Event sdl_event;
	static int new_mx, new_my;
	static int old_mx, old_my;

	SDL_GetMouseState(&new_mx, &new_my);

	if (!SDL_PollEvent(&sdl_event)) return 0;

	switch (sdl_event.type) {
		case SDL_KEYUP:
			e->type = EVENT_RELEASE;
			e->code = map_keycode(sdl_event.key.keysym.sym);
			break;

		case SDL_KEYDOWN:
			e->type = EVENT_PRESS;
			e->code = map_keycode(sdl_event.key.keysym.sym);
			break;

		case SDL_QUIT:
			exit(0);

		case SDL_MOUSEBUTTONDOWN:
			switch (sdl_event.button.button) {
				case SDL_BUTTON_LEFT:
					e->type = EVENT_PRESS;
					e->code = DOPE_BTN_LEFT;
					break;
				case SDL_BUTTON_RIGHT:
					e->type = EVENT_PRESS;
					e->code = DOPE_BTN_RIGHT;
					break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			switch (sdl_event.button.button) {
				case SDL_BUTTON_LEFT:
					e->type = EVENT_RELEASE;
					e->code = DOPE_BTN_LEFT;
					break;
				case SDL_BUTTON_RIGHT:
					e->type = EVENT_RELEASE;
					e->code = DOPE_BTN_RIGHT;
					break;
			}
			break;

		case SDL_MOUSEMOTION:
			e->type = EVENT_ABSMOTION;
			e->abs_x = new_mx;
			e->abs_y = new_my;
			old_mx = new_mx;
			old_my = new_my;
			break;
	}
	return 1;
}


/**************************************
 ** Service structure of this module **
 **************************************/

static struct input_services input = {
	get_event,
};


/************************
 ** Module entry point **
 ************************/

int init_input(struct dope_services *d)
{
	d->register_module("Input 1.0", &input);
	return 1;
}
