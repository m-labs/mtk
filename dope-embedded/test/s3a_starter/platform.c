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

void set_leds(unsigned char leds_state)
{
	int *ptr = (int *)(XPAR_LEDS_8BIT_BASEADDR);
	*ptr = leds_state;
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
