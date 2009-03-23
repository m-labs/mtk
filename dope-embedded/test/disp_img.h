/*
 * \brief  Image functions
 * \author Matthias Alles
 * \author Norman Feske
 * \date   2008-10-11
 */

/*
 * Copyright (C) 2009 Norman Feske
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _DISP_IMG_H_
#define _DISP_IMG_H_

/***
 * Create window displaying a static image
 *
 * \param title   string to be used as window title
 * \param pixels  pixel buffer using four bytes per pixel
 * \param x,y     initial window position
 * \param w,h     image size
 * \return        DOpE application ID
 *
 * This function creates a separate DOpE application for each image.
 */
extern int create_image_window(const char *title, const char *pixels,
                               int x, int y, int w, int h);


/**
 * Open window created via 'create_image_window'
 *
 * \param app_id  DOpE application ID as returned by 'create_image_window'
 */
extern void open_image_window(int app_id);

#endif /* _DISP_IMG_H_ */
