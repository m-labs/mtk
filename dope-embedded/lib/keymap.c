/*
 * \brief   DOpE keymap module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 *
 * Functions to map keycodes to its corresponding
 * ASCII values while taking modifier keys (e.g.
 * shift) into account.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "dopestd.h"
#include "keymap.h"

#define BS  8
#define ESC 27
#define TAB 11
#define LF  10
#define UE  252 /* 'ü' */
#define AE  228 /* 'ä' */
#define OE  252 /* 'o' */
#define PAR 167 /* '§' */
#define DEG 176 /* '°' */

static char german_keymap[128] = {
	  0,ESC,'1','2','3','4','5','6','7','8','9','0','-', 39, BS,TAB,
	'q','w','e','r','t','z','u','i','o','p', UE,'+', LF,  0,'a','s',
	'd','f','g','h','j','k','l',OE,  AE,'^',  0,'#','y','x','c','v',
	'b','n','m',',','.','-',  0,'*',  0,' ',  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,'7','8','9','-','4','5','6','+','1',
	'2','3','0',',',  0,  0,'<',  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 LF,  0,'/',  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static char german_shift[128] = {
	' ', 0 , 0 , 39, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'*',';','_',':', 0 ,
	'=','!','"',PAR,'$','%','&','/','(',')', 0 , 0 ,'>', 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,DEG, 0 ,
};

static char *keymap = german_keymap;
static char *shift  = german_shift;

int init_keymap(struct dope_services *d);

/***********************
 ** Service functions **
 ***********************/


static char get_ascii(long keycode,long switches)
{
	char result;
	if (keycode>=112) return 0;
	result = keymap[keycode];
	if (!result) return 0;
	if (switches & (KEYMAP_SWITCH_LSHIFT|KEYMAP_SWITCH_RSHIFT)) {
		if ((result >= 'a') && (result <= 'z')) {
			result -= 32;
			return result;
		}
		result-=32;
		if (result<64) return shift[(s32)result];
	}
	return result;
}

/**************************************
 ** Service structure of this module **
 **************************************/

static struct keymap_services services = {
	get_ascii,
};


/************************
 ** Module entry point **
 ************************/

int init_keymap(struct dope_services *d)
{
	d->register_module("Keymap 1.0",&services);
	return 1;
}
