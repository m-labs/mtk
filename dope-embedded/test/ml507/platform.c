/*
 * \brief  Platform for ML507
 * \author Norman Feske
 * \author Matthias Alles
 * \date   2008-08-17
 */

/* FPGA includes */
#include "xparameters.h"
#ifdef __MICROBLAZE__
#include "mb_interface.h"
#endif /* __MICROBLAZE__ */

#ifdef __PPC__
#include "xcache_l.h"
#endif

/* local includes */
#include "platform.h"
#include "virtex5.h"
#include "powerpc_logo.h"

void set_leds(unsigned char leds_state)
{
	int *ptr = (int *)(XPAR_LEDS_8BIT_BASEADDR);

	/* set the LED state */
	*ptr++ = leds_state;

	/* set GPIO direction to out */
	*ptr   = 0;

	return;

}

void display_platform_images(void){

	display_image("Virtex 5", pixel_data_virtex5, 280, 200, 250, 127);
	display_image("PowerPC 440", pixel_data_ppc, 25, 500, 126, 76);
}

int init_platform(void)
{

	/* enable PPC cache if available */
#ifdef __PPC__
	XCache_EnableICache(0x80000000);
	XCache_EnableDCache(0x80000000);
#endif

	return 0;
}
