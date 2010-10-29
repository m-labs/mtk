/*
 * \brief   Error values that are exported by MTK
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the MTK package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef __MTK_INCLUDE_MTKDEF_H_
#define __MTK_INCLUDE_MTKDEF_H_


#define MTK_ERR_PERM               -1  /* permission denied          */
#define MTK_ERR_NOT_PRESENT        -2  /* no MTK server to speak to */

#define MTKCMD_ERR_UNKNOWN_VAR    -11  /* variable does not exist               */
#define MTKCMD_ERR_INVALID_VAR    -12  /* variable became invalid               */
#define MTKCMD_ERR_INVALID_TYPE   -13  /* variable type does not exist          */
#define MTKCMD_ERR_ATTR_W_PERM    -14  /* attribute cannot be written           */
#define MTKCMD_ERR_ATTR_R_PERM    -15  /* attribute cannot be requested         */
#define MTKCMD_ERR_NO_SUCH_MEMBER -16  /* method or attribute does not exist    */
#define MTKCMD_ERR_NO_TAG         -17  /* expected tag but found something else */
#define MTKCMD_ERR_UNKNOWN_TAG    -18  /* tag does not exist                    */
#define MTKCMD_ERR_INVALID_ARG    -19  /* illegal argument value                */
#define MTKCMD_ERR_LEFT_PAR       -20  /* missing left parenthesis              */
#define MTKCMD_ERR_RIGHT_PAR      -21  /* missing right parenthesis             */
#define MTKCMD_ERR_TOO_MANY_ARGS  -22  /* too many method arguments             */
#define MTKCMD_ERR_MISSING_ARG    -23  /* mandatory argument is missing         */
#define MTKCMD_ERR_UNCOMPLETE     -24  /* unexpected end of command             */
#define MTKCMD_ERR_NO_SUCH_SCOPE  -25  /* variable scope could not be resolved  */
#define MTKCMD_ERR_ILLEGAL_CMD    -26  /* command could not be examined         */

#define MTKCMD_WARN_TRUNC_RET_STR  11  /* return string was truncated */

#endif /* __MTK_INCLUDE_MTKDEF_H_ */
