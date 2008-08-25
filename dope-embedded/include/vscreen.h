/*
 * \brief   Interface of VScreen library
 * \date    2002-11-25
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2002-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */


/******************************************
 * HANDLE SHARED MEMORY BUFFER OF VSCREEN *
 ******************************************/

/**
 * Return local address of specified shared memory block
 *
 * \param smb_ident  identifier string of the shared memory block
 * \return           address of the smb block in local address space
 *
 * The format of the smb_ident string is platform dependent. The
 * result of the VScreen widget's map function can directly be passed
 * into this function. (as done by vscr_get_fb)
 */
extern void *vscr_map_smb(char *smb_ident);


/**
 * Return local address of the framebuffer of the specified vscreen
 *
 * \param app_id     DOpE application id to which the VScreen widget belongs
 * \param vscr_name  name of the VScreen widget
 * \return           address of the VScreen buffer in the local address space
 */
extern void *vscr_get_fb(int app_id, const char *vscr_name);


/**
 * Release vscreen buffer from local address space
 *
 * \param fb_adr   start address of the vscreen buffer
 * \return         0 on success
 */
extern int vscr_free_fb(void *fb_adr);


/*************************
 * HANDLE VSCREEN SERVER *
 *************************/

/**
 * Connect to specified vscreen widget server
 *
 * \param vscr_ident  identifier of the widget's server
 * \return            id to be used for the vscr_server-functions
 */
extern void *vscr_connect_server(char *vscr_ident);


/**
 * Get server id of the specified vscreen widget
 *
 * \param app_id     DOpE application id to which the VScreen widget belongs
 * \param vscr_name  name of the VScreen widget
 * \return           id to be used for the vscr_server-functions
 */
extern void *vscr_get_server_id(int app_id, const char *vscr_name);


/**
 * Close connection to vscreen server
 *
 * \param vscr_server_id  id of the vscreen widget server to disconnect from
 */
extern void  vscr_release_server_id(void *vscr_server_id);


/**
 * Wait for synchronisation
 *
 * This function waits until the current drawing period of this
 * widget is finished.
 */
extern void  vscr_server_waitsync(void *vscr_server_id);


/**
 * Refresh rectangular widget area asynchronously
 *
 * This function refreshes the specified area of the VScreen widget
 * in a best-efford way. The advantage of this function over a
 * dope_cmd call of vscr.refresh() is its deterministic execution
 * time and its nonblocking way of operation.
 */
extern void  vscr_server_refresh(void *vscr_server_id, int x, int y, int w, int h);
