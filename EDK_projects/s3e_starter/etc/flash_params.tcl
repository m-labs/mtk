set FLASH_FILE             "demo.elf.srec"         ;# Image file to program the flash with
set FLASH_BASEADDR         0x89000000              ;# Base address of flash device
set FLASH_PROG_OFFSET      0x00000000              ;# Offset at which the image should be programmed within flash
set FLASH_BUSWIDTH         8                       ;# Device bus width of all flash parts combined
set SCRATCH_BASEADDR       0x8C000000              ;# Base address of scratch memory
set SCRATCH_LEN            0x04000000              ;# Length of scratch memory
set EXTRA_COMPILER_FLAGS   "-mxl-barrel-shift -mno-xl-soft-mul -mno-xl-soft-div -mxl-pattern-compare" ;# Extra Compiler Flags for MicroBlaze
set XMD_CONNECT            "connect mb mdm -debugdevice cpunr 1" ;# Target Command to connect to XMD
set TARGET_TYPE            "MICROBLAZE"            ;# Target processor type
set PROC_INSTANCE          "microblaze_0"          ;# Processor Instance name

