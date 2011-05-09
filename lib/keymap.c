/*
 * \brief   MTK keymap module
 *
 * Functions to map keycodes to its corresponding
 * ASCII values while taking modifier keys (e.g.
 * shift) into account.
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include "mtkstd.h"
#include "keymap.h"

#define BS  8
#define ESC 27
#define TAB 11
#define LF  10

#define DEG 167 /* '°' */
#define PAR  21 /* '§' */
#define ET  225 /* 'ß' */
#define UE  129 /* 'ü' */
#define OE  148 /* 'ö' */
#define AE  132 /* 'ä' */
#define MU  230 /* 'µ' */
#define DI  246 /* '÷' */
#define EA  130 /* 'é' */
#define DR  175 /* '»' */

static int curr_layout = 2;

/*
 * TODO: AltGr not working, it's same as SHIFT,
 *       Non-ascii not working.
 *       SHIFLT of 'ö' 'ä' 'µ' 'é' '»' not working.
 * http://ascii-table.com/keyboards.php
 */
static char keymap[3][128] = {
	{ 0,ESC,'1','2','3','4','5','6','7','8','9','0','-','=', BS,TAB, /* 16 */
	'q','w','e','r','t','y','u','i','o','p','[',']', LF,  0,'a','s', /* 32 */
	'd','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v', /* 48 */
	'b','n','m',',','.','/',  0,'*',  0,' ',  0,  0,  0,  0,  0,  0, /* 64 */
	  0,  0,  0,  0,  0,  0,  0,'7','8','9','-','4','5','6','+','1', /* 80 */
	'2','3','0','.',  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 96 */
	 LF,  0,'/',  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 112 */
	},			/* US layout 103P */
	{ 0,ESC,'1','2','3','4','5','6','7','8','9','0','-','=', BS,TAB,
	'q','w','e','r','t','y','u','i','o','p','[',']', LF,  0,'a','s',
	'd','f','g','h','j','k','l',';','{','#',0,'<','z','x','c','v',
	'b','n','m',',','.', EA,  0,'*',  0,' ',  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,'7','8','9','-','4','5','6','+','1',
	'2','3','0','.',  0,  0, DR,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 LF,  0,'/',  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	},			/* French layout 058 */
	{ 0,ESC,'1','2','3','4','5','6','7','8','9','0','\\',  0,BS,TAB,
	'q','w','e','r','t','z','u','i','o','p', UE,'+', LF,  0,'a','s',
	'd','f','g','h','j','k','l',OE,  AE,'^',  0,'#','y','x','c','v',
	'b','n','m',',','.','-',  0,'*',  0,' ',  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,'7','8','9','-','4','5','6','+','1',
	'2','3','0',',',  0,  0,'<',  0,  0,  0,  0,  0,  0,  0,  0,  0,
	 LF,  0, DI,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	}			/* German layout 129 */
};

static char shift[3][128] = {
	{' ', 0 , 0 , 0 , 0 , 0 , 0,'\"', 0, 0 , 0 , 0 ,'<','_','>','?', /* 16 */
	')','!','@','#','$','%','^','&','*','(', 0 ,':', 0 ,'+', 0 , 0 , /* 32 */
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , /* 48 */
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'{','|','}', 0 , 0 , /* 64 */
	'~',
	},			/* US layout 103P */
	{' ', 0 , 0 ,'|', 0 , 0 , 0, 0 , 0, 0 , 0 , 0,'\'','_','\"', 0 ,
	')','!','\"','/','$','%','?','&','*','(',0 ,':','>','+', 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'{', 0 ,'}', 0 , 0 ,
	},			/* French layout 058 */
	{' ', 0 , 0 ,39, 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'*',';','_',':', 0 ,
	'=','!','"',PAR,'$','%','&','/','(',')', 0 , 0 ,'>', 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'?', 0 ,DEG, 0 ,
	}			/* German layout 129 */
};

int init_keymap(struct mtk_services *d);

/***********************
 ** Service functions **
 ***********************/


static char get_ascii(int keycode,int switches)
{
	char result;
	if (keycode>=112) return 0;
	result = keymap[curr_layout][keycode];
	if (!result) return 0;
	if (switches & (KEYMAP_SWITCH_LSHIFT|KEYMAP_SWITCH_RSHIFT)) {
		if ((result >= 'a') && (result <= 'z')) {
			result -= 32;
			return result;
		}
		result-=32;
		if (result<=64) return shift[curr_layout][(s32)result];
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

int init_keymap(struct mtk_services *d)
{
	d->register_module("Keymap 1.0",&services);
	return 1;
}

void mtk_config_set_keyboard_layout(int layout)
{
	if (curr_layout < 0 || curr_layout > 2)
		return;

	curr_layout = layout;
}
