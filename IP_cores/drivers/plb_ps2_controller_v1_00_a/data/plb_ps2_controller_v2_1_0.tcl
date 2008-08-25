##############################################################################
## Filename:          /media/hdc5/edk_user_repository/MyProcessorIPLib/drivers/plb_ps2_controller_v1_00_a/data/plb_ps2_controller_v2_1_0.tcl
## Description:       Microprocess Driver Command (tcl)
## Date:              Sat Jan 12 14:37:13 2008 (by Create and Import Peripheral Wizard)
##############################################################################

#uses "xillib.tcl"

proc generate {drv_handle} {
  xdefine_include_file $drv_handle "xparameters.h" "plb_ps2_controller" "NUM_INSTANCES" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR" 
}
