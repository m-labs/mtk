#include "dopestd.h"

int config_oldresize    = 0;  /* use traditional way to resize windows */
int config_adapt_redraw = 0;  /* do not adapt redraw to duration time  */

void native_startup(int argc, char **argv);
void native_startup(int argc, char **argv) { }

void init_don_scheduler(struct dope_services *d);
void init_don_scheduler(struct dope_services *d) { }

void init_pslim_server(struct dope_services *d);
void init_pslim_server(struct dope_services *d) { }

void init_pslim(struct dope_services *d);
void init_pslim(struct dope_services *d)
{
	d->register_module("PSLIMServer 1.0", NULL);
}

void init_vscr_server(struct dope_services *d);
void init_vscr_server(struct dope_services *d)
{
	d->register_module("VScreenServer 1.0", NULL);
}

void init_server(struct dope_services *d);
void init_server(struct dope_services *d)
{
	d->register_module("Server 1.0", NULL);
}
