/*
 * \brief   Interface of DOpE Label widget module
 * \date    2003-05-15
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2003-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_LABEL_H_
#define _DOPE_LABEL_H_

#include "widget.h"
#include "variable.h"

struct label_methods;
struct label_data;

#define LABEL struct label

struct label {
	struct widget_methods *gen;
	struct label_methods  *lab;
	struct widget_data    *wd;
	struct label_data     *ld;
};

struct label_methods {
	void      (*set_text)  (LABEL *, char *new_txt);
	char     *(*get_text)  (LABEL *);
	void      (*set_font)  (LABEL *, char *new_fontname);
	char     *(*get_font)  (LABEL *);
	void      (*set_var)   (LABEL *, VARIABLE *v);
	VARIABLE *(*get_var)   (LABEL *);
};

struct label_services {
	LABEL *(*create) (void);
};


#endif /* _DOPE_LABEL_H_ */
