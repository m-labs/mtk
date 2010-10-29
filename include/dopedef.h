/*
 * \brief   Error values that are exported by DOpE
 */

/*
 * Copyright (C) 2004-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef __DOPE_INCLUDE_DOPEDEF_H_
#define __DOPE_INCLUDE_DOPEDEF_H_


#define DOPE_ERR_PERM               -1  /* permission denied          */
#define DOPE_ERR_NOT_PRESENT        -2  /* no DOpE server to speak to */

#define DOPECMD_ERR_UNKNOWN_VAR    -11  /* variable does not exist               */
#define DOPECMD_ERR_INVALID_VAR    -12  /* variable became invalid               */
#define DOPECMD_ERR_INVALID_TYPE   -13  /* variable type does not exist          */
#define DOPECMD_ERR_ATTR_W_PERM    -14  /* attribute cannot be written           */
#define DOPECMD_ERR_ATTR_R_PERM    -15  /* attribute cannot be requested         */
#define DOPECMD_ERR_NO_SUCH_MEMBER -16  /* method or attribute does not exist    */
#define DOPECMD_ERR_NO_TAG         -17  /* expected tag but found something else */
#define DOPECMD_ERR_UNKNOWN_TAG    -18  /* tag does not exist                    */
#define DOPECMD_ERR_INVALID_ARG    -19  /* illegal argument value                */
#define DOPECMD_ERR_LEFT_PAR       -20  /* missing left parenthesis              */
#define DOPECMD_ERR_RIGHT_PAR      -21  /* missing right parenthesis             */
#define DOPECMD_ERR_TOO_MANY_ARGS  -22  /* too many method arguments             */
#define DOPECMD_ERR_MISSING_ARG    -23  /* mandatory argument is missing         */
#define DOPECMD_ERR_UNCOMPLETE     -24  /* unexpected end of command             */
#define DOPECMD_ERR_NO_SUCH_SCOPE  -25  /* variable scope could not be resolved  */
#define DOPECMD_ERR_ILLEGAL_CMD    -26  /* command could not be examined         */

#define DOPECMD_WARN_TRUNC_RET_STR  11  /* return string was truncated */

#endif /* __DOPE_INCLUDE_DOPEDEF_H_ */
