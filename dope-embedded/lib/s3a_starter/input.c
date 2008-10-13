/*
 * \brief   DOpE input driver module for Charon's FPGA board
 * \date    2005-02-04
 * \author  Norman Feske <norman.feske@genode-labs.com>
 */

/*
 * Copyright (C) 2005-2008 Norman Feske <norman.feske@genode-labs.com>
 * Genode Labs, Feske & Helmuth Systementwicklung GbR
 *
 * This file is part of the DOpE-embedded package, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* FPGA includes */
#include <stdio.h>     /* for printf */
#include <unistd.h>    /* for usleep */
#include "xparameters.h"
#include "xutil.h"
#include "xintc.h"
#include "plb_ps2_controller.h"
#ifdef __MICROBLAZE__
#include "mb_interface.h"
#endif /* __MICROBLAZE__ */

#ifdef __PPC__
#include "xexception_l.h"
#include "xtime_l.h"
#endif /* __PPC__ */


/* local includes */
#include "dopestd.h"
#include "event.h"
#include "input.h"
#include "keycodes.h"

#define EV_FIFO_SIZE 200

#define INTC_DEVICE_ID         XPAR_INTC_0_DEVICE_ID

/* translation table for scancode set 2 to scancode set 1 conversion */
static int keyb_translation_table[16][16] = {
{0xff,0x43,0x41,0x3f,0x3d,0x3b,0x3c,0x58,0x64,0x44,0x42,0x40,0x3e,0x0f,0x29,0x59},
{0x65,0x38,0x2a,0x70,0x1d,0x10,0x02,0x5a,0x66,0x71,0x2c,0x1f,0x1e,0x11,0x03,0x5b},
{0x67,0x2e,0x2d,0x20,0x12,0x05,0x04,0x5c,0x68,0x39,0x2f,0x21,0x14,0x13,0x06,0x5d},
{0x69,0x31,0x30,0x23,0x22,0x15,0x07,0x5e,0x6a,0x72,0x32,0x24,0x16,0x08,0x09,0x5f},
{0x6b,0x33,0x25,0x17,0x18,0x0b,0x0a,0x60,0x6c,0x34,0x35,0x26,0x27,0x19,0x0c,0x61},
{0x6d,0x73,0x28,0x74,0x1a,0x0d,0x62,0x6e,0x3a,0x36,0x1c,0x1b,0x75,0x2b,0x63,0x76},
{0x55,0x56,0x77,0x78,0x79,0x7a,0x0e,0x7b,0x7c,0x4f,0x7d,0x4b,0x47,0x7e,0x7f,0x6f},
{0x52,0x53,0x50,0x4c,0x4d,0x48,0x01,0x45,0x57,0x4e,0x51,0x4a,0x37,0x49,0x46,0x54},
{0x80,0x81,0x82,0x41,0x54,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f},
{0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f},
{0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf},
{0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf},
{0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf},
{0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf},
{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef},
{0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff}};


int init_input(struct dope_services *d);
XStatus SetUpInterruptSystem(XIntc *XIntcInstancePtr);
static XIntc InterruptController; /* Instance of the Interrupt Controller */

struct input_event {
	u32 type;
	u32 code;
	u32 rel_x;
	u32 rel_y;
};

static struct input_event evqueue[EV_FIFO_SIZE];
static int head = 0, tail = 0;


/***********************
 ** Utility functions **
 ***********************/

/**
 * Insert event into event queue
 */
void enqueue_event(int type, int code, int rel_x, int rel_y)
{
	evqueue[head].type  = type;
	evqueue[head].code  = code;
	evqueue[head].rel_x = rel_x;
	evqueue[head].rel_y = rel_y;

	head = (head + 1) % EV_FIFO_SIZE;

	/* if queue is full, discard old elements */
	if (head == tail)
		tail = (tail + 1) % EV_FIFO_SIZE;

}


/***********************
 ** Interrupt handler **
 ***********************/

#define EVENT_FROM_KEYBD      0x1
#define EVENT_FROM_MOUSE      0x2

#define MOUSE_LEFT            0x1
#define MOUSE_RIGHT           0x2
#define MOUSE_HOR_MASK   0x00ff00
#define MOUSE_VER_MASK   0xff0000
#define MOUSE_HOR_SHIFT         8
#define MOUSE_VER_SHIFT        16
#define MOUSE_HOR_NEG    0x008000
#define MOUSE_VER_NEG    0x800000


#define KEYBD_TYPE_MASK     0x100
#define KEYBD_CODE_MASK      0xff


/**
 * Convert mouse event to dope event
 */
void handle_mouse_event(u32 new_mstate)
{
	static u32 old_mstate;

	/* left mouse button pressed */
	if (!(old_mstate & MOUSE_LEFT) && (new_mstate & MOUSE_LEFT))
		enqueue_event(EVENT_PRESS, DOPE_BTN_LEFT, 0, 0);

	/* left mouse button released */
	if ((old_mstate & MOUSE_LEFT) && !(new_mstate & MOUSE_LEFT))
		enqueue_event(EVENT_RELEASE, DOPE_BTN_LEFT, 0, 0);

	/* right mouse button pressed */
	if (!(old_mstate & MOUSE_RIGHT) && (new_mstate & MOUSE_RIGHT))
		enqueue_event(EVENT_PRESS, DOPE_BTN_RIGHT, 0, 0);

	/* right mouse button released */
	if ((old_mstate & MOUSE_RIGHT) && !(new_mstate & MOUSE_RIGHT))
		enqueue_event(EVENT_RELEASE, DOPE_BTN_RIGHT, 0, 0);

	/* check for mouse motion */
	if ((new_mstate & MOUSE_HOR_MASK) || (new_mstate & MOUSE_VER_MASK)) {
		enqueue_event(EVENT_MOTION, 0,
		      (new_mstate & MOUSE_HOR_NEG) ? (((new_mstate & MOUSE_HOR_MASK) >> MOUSE_HOR_SHIFT) | 0xFFFFFF00) : (new_mstate & MOUSE_HOR_MASK) >> MOUSE_HOR_SHIFT,
		      (new_mstate & MOUSE_VER_NEG) ?  (int)((new_mstate & MOUSE_VER_MASK) >> MOUSE_VER_SHIFT | 0xFFFFFF00) * -1 : (int)((new_mstate & MOUSE_VER_MASK) >> MOUSE_VER_SHIFT) * -1 );
	}

	/* remember current mouse state for the next time */
	old_mstate = new_mstate;
}


/**
 * Convert keyboard event to dope event
 */
static void handle_keybd_event(u32 keystate)
{
	int scancode;

	/* translate hardware scancode set 2 to software scancode set 1 */
	scancode = keyb_translation_table[(keystate>>4)&0x0F][keystate&0x0F];

	/* key released */
	if (keystate & KEYBD_TYPE_MASK)
		enqueue_event(EVENT_RELEASE, scancode & KEYBD_CODE_MASK, 0, 0);

	/* key pressed */
	else enqueue_event(EVENT_PRESS, scancode & KEYBD_CODE_MASK, 0, 0);
}


/**
 * Input interrupt handler
 */
void PS2_InterruptHandler(void *CallbackRef)
{
	u32 mouse_state, keybd_state, flags;
	Xuint32 baseaddr = (Xuint32) XPAR_PLB_PS2_CONTROLLER_0_BASEADDR;

	/* read information from device registers */
	keybd_state = ((u32 *)baseaddr)[0];
	mouse_state = ((u32 *)baseaddr)[1];
	flags       = PLB_PS2_CONTROLLER_mReadReg(baseaddr, PLB_PS2_CONTROLLER_INTR_IPISR_OFFSET);

	if (flags & EVENT_FROM_MOUSE)
		handle_mouse_event(mouse_state);

	if (flags & EVENT_FROM_KEYBD)
		handle_keybd_event(keybd_state);


	/* clear interrupt flags in PS/2 controller */
	PLB_PS2_CONTROLLER_mWriteReg(baseaddr, PLB_PS2_CONTROLLER_INTR_IPISR_OFFSET, flags);
}

/* VGA INTERRUPT HANDLER */
//void NPI_VGA_InterruptHandler(void *CallbackRef) {
//	u32 flags;
//	Xuint32 baseaddr = (Xuint32) XPAR_PLB_PS2_CONTROLLER_0_BASEADDR;
//
//	/* read interrupt state and clear interrupt */
//	flags       = PLB_NPI_VGA_CONTROLLER_mReadReg(baseaddr, PLB_PS2_CONTROLLER_INTR_IPISR_OFFSET);
//	PLB_NPI_VGA_CONTROLLER_mWriteReg(baseaddr, PLB_PS2_CONTROLLER_INTR_IPISR_OFFSET, flags);
//}

/***********************
 ** Service functions **
 ***********************/

/**
 * Get next event of event queue
 *
 * \return  0 if there is no pending event
 *          1 if there an event was returned in out parameter e
 */
static int get_event(EVENT *e)
{
	/* no event in queue */
	if (head == tail) return 0;

	e->type  = evqueue[tail].type;
	e->code  = evqueue[tail].code;
	e->rel_x = evqueue[tail].rel_x;
	e->rel_y = evqueue[tail].rel_y;

	tail = (tail + 1) % EV_FIFO_SIZE;
	return 1;
}


/*************************************
 * Service structure of this module **
 *************************************/

static struct input_services input = {
	get_event,
};


/************************
 ** Module entry point **
 ************************/

int init_input(struct dope_services *d)
{
	XStatus Status;
	Xuint32 baseaddr = (Xuint32) XPAR_PLB_PS2_CONTROLLER_0_BASEADDR;

	/* initialize PS/2 interrupts */
	PLB_PS2_CONTROLLER_mWriteReg(baseaddr, PLB_PS2_CONTROLLER_INTR_DIER_OFFSET, 0);
	PLB_PS2_CONTROLLER_mWriteReg(baseaddr, PLB_PS2_CONTROLLER_INTR_DGIER_OFFSET, 0xFFFFFFFF);
	PLB_PS2_CONTROLLER_mWriteReg(baseaddr, PLB_PS2_CONTROLLER_INTR_IPIER_OFFSET, 0xFFFFFFFF);
	PLB_PS2_CONTROLLER_mWriteReg(baseaddr, PLB_PS2_CONTROLLER_INTR_IPISR_OFFSET, 0);

	/* initialize Interrupt controller */
	Status = XIntc_Initialize(&InterruptController, (Xuint16)INTC_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
//		print("INTC Interrupt init failed!\n");
		return XST_FAILURE;
	}
//	print("INTC Interrupt init successful!\n");
	Status = XIntc_SelfTest(&InterruptController);
	if (Status != XST_SUCCESS)
	{
//		print("INTC Interrupt selftest failed!\n");
		return XST_FAILURE;
	}
//	print("INTC Interrupt selftest successful!\n");
    Status = SetUpInterruptSystem(&InterruptController);
	if (Status != XST_SUCCESS)
	{
//		print("INTC Interrupt setup failed!\n");
		return XST_FAILURE;
	}
//	print("INTC Interrupt setup successful!\n");

	d->register_module("Input 1.0",&input);
	return 1;
}


XStatus SetUpInterruptSystem(XIntc *XIntcInstancePtr)
{
    XStatus Status;

    /*
     * Connect a device driver handler that will be called when an interrupt
     * for the device occurs, the device driver handler performs the specific
     * interrupt processing for the device
     */
//    Status = XIntc_Connect(XIntcInstancePtr, XPAR_XPS_INTC_0_PLB_NPI_VGA_CONTROLLER_0_IP2INTC_IRPT_INTR,
//                           (XInterruptHandler)NPI_VGA_InterruptHandler,
//                           (void *)0);
//    if (Status != XST_SUCCESS)
//    {
//        return XST_FAILURE;
//    }
//	print("Interrupt handler VGA connected!\n");


    Status = XIntc_Connect(XIntcInstancePtr, XPAR_XPS_INTC_0_PLB_PS2_CONTROLLER_0_IP2INTC_IRPT_INTR,
                           (XInterruptHandler)PS2_InterruptHandler,
                           (void *)0);
    if (Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }
//	print("Interrupt handler PS/2 connected!\n");


    /*
     * Start the interrupt controller such that interrupts are enabled for
     * all devices that cause interrupts, specify simulation mode so that
     * an interrupt can be caused by software rather than a real hardware
     * interrupt
     */
    Status = XIntc_Start(XIntcInstancePtr, XIN_REAL_MODE);
    if (Status != XST_SUCCESS)
    {
        return XST_FAILURE;
    }


    /*
     * Enable the interrupt for the device and then cause (simulate) an
     * interrupt so the handlers will be called
     */

    XIntc_Enable(XIntcInstancePtr, XPAR_XPS_INTC_0_PLB_PS2_CONTROLLER_0_IP2INTC_IRPT_INTR);

#ifdef __PPC__

//	print("Enabling PPC interrupts!\n");
    /*
     * Initialize the PPC440 exception table
     */
    XExc_Init();

    /*
     * Register the interrupt controller handler with the exception table
     */
    XExc_RegisterHandler(XEXC_ID_NON_CRITICAL_INT,
                        (XExceptionHandler)XIntc_InterruptHandler,
                         XIntcInstancePtr);

    /*
     * Enable non-critical exceptions
     */
    XExc_mEnableExceptions(XEXC_NON_CRITICAL);

	/* clear the timer status register to prevent timer interrupts */
	XTime_TSRClearStatusBits(XREG_TSR_CLEAR_ALL);

#endif /* __PPC__ */


#ifdef __MICROBLAZE__

    /*
     * Enable the Interrupts for the MicroBlaze Processor
     */
//	print("Enable Microblaze Interrupt!\n");
    microblaze_enable_interrupts();

#endif /* __MICROBLAZE__ */

    return XST_SUCCESS;

}
