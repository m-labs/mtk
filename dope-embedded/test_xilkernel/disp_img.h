/*
 * \brief  Image functions
 * \author Matthias Alles
 * \author Norman Feske
 * \date   2008-10-11
 */

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
int display_image(const char *title, const char *pixels,
                         int x, int y, int w, int h);
