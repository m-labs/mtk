//////////////////////////////////////////////////////////////////////////////
// Filename:          /media/hdc5/edk_user_repository/MyProcessorIPLib/drivers/plb_npi_vga_controller_v1_01_a/src/plb_npi_vga_controller.h
// Version:           1.01.a
// Description:       plb_npi_vga_controller Driver Header File
// Date:              Thu Dec 27 16:46:25 2007 (by Create and Import Peripheral Wizard)
//////////////////////////////////////////////////////////////////////////////

#ifndef PLB_NPI_VGA_CONTROLLER_H
#define PLB_NPI_VGA_CONTROLLER_H

/***************************** Include Files *******************************/

#include "xbasic_types.h"
#include "xstatus.h"
#include "xio.h"

/************************** Constant Definitions ***************************/
/* HELP! Shouldn't this be prototyped in some Xilinx standalone BSP header? */
//void xil_printf(const char* ctrl1, ...);

/**
 * User Logic Slave Space Offsets
 * -- SLV_REG0 : user logic slave module register 0
 * -- SLV_REG1 : user logic slave module register 1
 * -- SLV_REG2 : user logic slave module register 2
 * -- SLV_REG3 : user logic slave module register 3
 */
#define PLB_NPI_VGA_CONTROLLER_USER_SLV_SPACE_OFFSET (0x00000000)
#define PLB_NPI_VGA_CONTROLLER_SLV_REG0_OFFSET (PLB_NPI_VGA_CONTROLLER_USER_SLV_SPACE_OFFSET + 0x00000000)
#define PLB_NPI_VGA_CONTROLLER_SLV_REG1_OFFSET (PLB_NPI_VGA_CONTROLLER_USER_SLV_SPACE_OFFSET + 0x00000004)
#define PLB_NPI_VGA_CONTROLLER_SLV_REG2_OFFSET (PLB_NPI_VGA_CONTROLLER_USER_SLV_SPACE_OFFSET + 0x00000008)
#define PLB_NPI_VGA_CONTROLLER_SLV_REG3_OFFSET (PLB_NPI_VGA_CONTROLLER_USER_SLV_SPACE_OFFSET + 0x0000000C)

/**
 * Interrupt Controller Space Offsets
 * -- INTR_DISR  : device (peripheral) interrupt status register
 * -- INTR_DIPR  : device (peripheral) interrupt pending register
 * -- INTR_DIER  : device (peripheral) interrupt enable register
 * -- INTR_DIIR  : device (peripheral) interrupt id (priority encoder) register
 * -- INTR_DGIER : device (peripheral) global interrupt enable register
 * -- INTR_ISR   : ip (user logic) interrupt status register
 * -- INTR_IER   : ip (user logic) interrupt enable register
 */
#define PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET (0x00000100)
#define PLB_NPI_VGA_CONTROLLER_INTR_DISR_OFFSET (PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000000)
#define PLB_NPI_VGA_CONTROLLER_INTR_DIPR_OFFSET (PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000004)
#define PLB_NPI_VGA_CONTROLLER_INTR_DIER_OFFSET (PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000008)
#define PLB_NPI_VGA_CONTROLLER_INTR_DIIR_OFFSET (PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000018)
#define PLB_NPI_VGA_CONTROLLER_INTR_DGIER_OFFSET (PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x0000001C)
#define PLB_NPI_VGA_CONTROLLER_INTR_IPISR_OFFSET (PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000020)
#define PLB_NPI_VGA_CONTROLLER_INTR_IPIER_OFFSET (PLB_NPI_VGA_CONTROLLER_INTR_CNTRL_SPACE_OFFSET + 0x00000028)

/**
 * Interrupt Controller Masks
 * -- INTR_TERR_MASK : transaction error
 * -- INTR_DPTO_MASK : data phase time-out
 * -- INTR_IPIR_MASK : ip interrupt requeset
 * -- INTR_RFDL_MASK : read packet fifo deadlock interrupt request
 * -- INTR_WFDL_MASK : write packet fifo deadlock interrupt request
 * -- INTR_IID_MASK  : interrupt id
 * -- INTR_GIE_MASK  : global interrupt enable
 * -- INTR_NOPEND    : the DIPR has no pending interrupts
 */
#define INTR_TERR_MASK (0x00000001UL)
#define INTR_DPTO_MASK (0x00000002UL)
#define INTR_IPIR_MASK (0x00000004UL)
#define INTR_RFDL_MASK (0x00000020UL)
#define INTR_WFDL_MASK (0x00000040UL)
#define INTR_IID_MASK (0x000000FFUL)
#define INTR_GIE_MASK (0x80000000UL)
#define INTR_NOPEND (0x80)

/**************************** Type Definitions *****************************/


/***************** Macros (Inline Functions) Definitions *******************/

/**
 *
 * Write a value to a PLB_NPI_VGA_CONTROLLER register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the PLB_NPI_VGA_CONTROLLER device.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.
 *
 * @note
 * C-style signature:
 * 	void PLB_NPI_VGA_CONTROLLER_mWriteReg(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Data)
 *
 */
#define PLB_NPI_VGA_CONTROLLER_mWriteReg(BaseAddress, RegOffset, Data) \
 	XIo_Out32((BaseAddress) + (RegOffset), (Xuint32)(Data))

/**
 *
 * Read a value from a PLB_NPI_VGA_CONTROLLER register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the PLB_NPI_VGA_CONTROLLER device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.
 *
 * @note
 * C-style signature:
 * 	Xuint32 PLB_NPI_VGA_CONTROLLER_mReadReg(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define PLB_NPI_VGA_CONTROLLER_mReadReg(BaseAddress, RegOffset) \
 	XIo_In32((BaseAddress) + (RegOffset))


/**
 *
 * Write/Read 32 bit value to/from PLB_NPI_VGA_CONTROLLER user logic slave registers.
 *
 * @param   BaseAddress is the base address of the PLB_NPI_VGA_CONTROLLER device.
 * @param   RegOffset is the offset from the slave register to write to or read from.
 * @param   Value is the data written to the register.
 *
 * @return  Data is the data from the user logic slave register.
 *
 * @note
 * C-style signature:
 * 	void PLB_NPI_VGA_CONTROLLER_mWriteSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset, Xuint32 Value)
 * 	Xuint32 PLB_NPI_VGA_CONTROLLER_mReadSlaveRegn(Xuint32 BaseAddress, unsigned RegOffset)
 *
 */
#define PLB_NPI_VGA_CONTROLLER_mWriteSlaveReg0(BaseAddress, RegOffset, Value) \
 	XIo_Out32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG0_OFFSET) + (RegOffset), (Xuint32)(Value))
#define PLB_NPI_VGA_CONTROLLER_mWriteSlaveReg1(BaseAddress, RegOffset, Value) \
 	XIo_Out32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG1_OFFSET) + (RegOffset), (Xuint32)(Value))
#define PLB_NPI_VGA_CONTROLLER_mWriteSlaveReg2(BaseAddress, RegOffset, Value) \
 	XIo_Out32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG2_OFFSET) + (RegOffset), (Xuint32)(Value))
#define PLB_NPI_VGA_CONTROLLER_mWriteSlaveReg3(BaseAddress, RegOffset, Value) \
 	XIo_Out32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG3_OFFSET) + (RegOffset), (Xuint32)(Value))

#define PLB_NPI_VGA_CONTROLLER_mReadSlaveReg0(BaseAddress, RegOffset) \
 	XIo_In32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG0_OFFSET) + (RegOffset))
#define PLB_NPI_VGA_CONTROLLER_mReadSlaveReg1(BaseAddress, RegOffset) \
 	XIo_In32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG1_OFFSET) + (RegOffset))
#define PLB_NPI_VGA_CONTROLLER_mReadSlaveReg2(BaseAddress, RegOffset) \
 	XIo_In32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG2_OFFSET) + (RegOffset))
#define PLB_NPI_VGA_CONTROLLER_mReadSlaveReg3(BaseAddress, RegOffset) \
 	XIo_In32((BaseAddress) + (PLB_NPI_VGA_CONTROLLER_SLV_REG3_OFFSET) + (RegOffset))

/************************** Function Prototypes ****************************/


/**
 *
 * Enable all possible interrupts from PLB_NPI_VGA_CONTROLLER device.
 *
 * @param   baseaddr_p is the base address of the PLB_NPI_VGA_CONTROLLER device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void PLB_NPI_VGA_CONTROLLER_EnableInterrupt(void * baseaddr_p);

/**
 *
 * Example interrupt controller handler.
 *
 * @param   baseaddr_p is the base address of the PLB_NPI_VGA_CONTROLLER device.
 *
 * @return  None.
 *
 * @note    None.
 *
 */
void PLB_NPI_VGA_CONTROLLER_Intr_DefaultHandler(void * baseaddr_p);

/**
 *
 * Run a self-test on the driver/device. Note this may be a destructive test if
 * resets of the device are performed.
 *
 * If the hardware system is not built correctly, this function may never
 * return to the caller.
 *
 * @param   baseaddr_p is the base address of the PLB_NPI_VGA_CONTROLLER instance to be worked on.
 *
 * @return
 *
 *    - XST_SUCCESS   if all self-test code passed
 *    - XST_FAILURE   if any self-test code failed
 *
 * @note    Caching must be turned off for this function to work.
 * @note    Self test may fail if data memory and device are not on the same bus.
 *
 */
XStatus PLB_NPI_VGA_CONTROLLER_SelfTest(void * baseaddr_p);

#endif // PLB_NPI_VGA_CONTROLLER_H
