/*
 * \brief   Interface of DOpE server module
 * \date    2002-11-13
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DOPE_SERVER_H_
#define _DOPE_SERVER_H_

struct server_services {
	void (*start) (void);
};

#endif /* _DOPE_SERVER_H_ */
