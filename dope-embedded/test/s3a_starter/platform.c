/*
 * \brief  Platform for Starter Kit 3A
 * \author Norman Feske
 * \date   2008-08-17
 */

/* FPGA includes */
#include "xparameters.h"
#ifdef __MICROBLAZE__
#include "mb_interface.h"
#endif /* __MICROBLAZE__ */

#ifdef PPC440CACHE
#include "xcache_l.h"
#endif

/* local includes */
#include "platform.h"
#include "spartan3a.h"
#include "../microblaze_logo.h"

void set_leds(unsigned char leds_state)
{
	int *ptr = (int *)(XPAR_LEDS_8BIT_BASEADDR);
	*ptr = leds_state;
}

void display_platform_images(void){

	display_image("Spartan-3A", pixel_data_spartan3a, 280, 200, 300, 223);
	display_image("Microblaze", pixel_data_microblaze, 25, 500, 269, 51);
}

int init_platform(void)
{
	/* enable microblaze cache if available */
#if XPAR_MICROBLAZE_0_USE_ICACHE 
	microblaze_enable_icache();
#endif
#if XPAR_MICROBLAZE_0_USE_DCACHE 
	microblaze_enable_dcache();
#endif

	return 0;
}
