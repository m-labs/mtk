/*
 * \brief  Platform for Milkymist
 * \author Sebastien Bourdeauducq
 * \date   2009-08-06
 */
 
#include <hw/interrupts.h>
#include <irq.h>
#include <uart.h>

void isr()
{
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

	if(irqs & IRQ_UARTRX)
		uart_async_isr_rx();
	if(irqs & IRQ_UARTTX)
		uart_async_isr_tx();

	irq_ack(irqs);
}
