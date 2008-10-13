/*
 * \brief  Platform for ML405
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

void set_leds(unsigned char leds_state)
{
	int *ptr = (int *)(XPAR_LEDS_4BIT_BASEADDR);

	/* set the LED state */
	*ptr++ = leds_state;

	/* set GPIO direction to out */
	*ptr   = 0;

	return;

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
