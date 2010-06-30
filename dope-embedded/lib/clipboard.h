/*
 * \brief  Interface of the DOpE clipboard module
 */

/*
 * Copyright (C) 2010 Romain P<rom1@netcourrier.com>
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */
#ifndef _DOPE_CLIPBOARD_H
#define _DOPE_CLIPBOARD_H

struct clipboard_services {
	void     (*set)    (char *dataIn, s32 length);
	void     (*get)    (char **dataOut, s32 *length);
};

#endif /* _DOPE_CLIPBOARD_H */ 

