/*
 * \brief   DOpE font manager module
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_FONTMAN_H_
#define _DOPE_FONTMAN_H_

struct font {
	s32  font_id;
	s32 *width_table;
	s32 *offset_table;
	s32  img_w,img_h;
	s16  top,bottom;
	u8  *image;
	u8  *name;
};


struct fontman_services {
	struct font *(*get_by_id)       (s32 font_id);
	s32          (*calc_str_width)  (s32 font_id, char *str);
	s32          (*calc_str_height) (s32 font_id, char *str);
	s32          (*calc_char_idx)   (s32 font_id, char *str, s32 xpos, s32 ypos);
};


#endif /* _DOPE_FONTMAN_H_ */
