##############################################################################
## Filename:          /media/hdc5/edk_user_repository/MyProcessorIPLib/drivers/plb_npi_vga_controller_v1_01_a/data/plb_npi_vga_controller_v2_1_0.tcl
## Description:       Microprocess Driver Command (tcl)
## Date:              Thu Dec 27 16:46:25 2007 (by Create and Import Peripheral Wizard)
##############################################################################

#uses "xillib.tcl"

proc generate {drv_handle} {
  xdefine_include_file $drv_handle "xparameters.h" "plb_npi_vga_controller" "NUM_INSTANCES" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" 
}
