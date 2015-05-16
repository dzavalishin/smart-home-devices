This bootloader use first 2k of FLASH. Use XMODEM-CRC, XMODEM-1k or YMODEM 
to upload your code. (autodetected protocol)

When compiling Nut/OS, in tools->GCC setting->linker script select at91sam7s64_bootrom
or at91sam7s256_bootrom to use with this bootloader.

Use SAM-BA to burn bootloader into flash memory. VERY IMPORTANT: you must erase all 
flash to unlock all sector before write bootloader. Bootload don't unlock sector. 

You can use ready-to-use binary into /bin directory or recompile.

To compile:
In makefile, select MCU to use (AT91SAM7S64,128,256)
In board.h select uart: UARTO, UART1 or DBGU
In board.h select baudrate
make (using ARM-NONE-EABI gnu compiler)

All source file is inside XLoader7.inter, file format using Interspector text editor, 
some weird thing can appear in source code if you open it directly.

Rémi Bilodeau (dec 2010)