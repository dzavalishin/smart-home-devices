Copyright (C) 2011 by Ole Reinhardt <ole.reinhardt@embedded-it.de>, 
                      Thermotemp GmbH. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holders nor the names of
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THERMOTEMP GMBH AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THERMOTEMP
GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

For additional information see http://www.ethernut.de/

Platform:
=========

This code is implemented for and tested with the following boards:

CPUs:

- AT91SAM7X256 
- AT91SAM7X512

Boards:

- at91sam7x_ek
- olimex sam7-ex256
- eNet-sam7X


at91sam7x_bootloader:
=====================

This bootloader should be considered as a demo. It is designed as 
a standard Nut/OS application and therefor compilation results in 
quite large binaries (caused by the overhead that Nut/OS brings in)

But at least this program is a fully functional TFTP bootloader which
can be controled using the CONFBOOT struct data placed in NVmem 
(e.g. a flash sector). 

In contrast to e.g. bootmon, this bootloader extends the CONFBOOT struct
by a field used to save an MD5 digest of the current application image.
This ensures that the application code itself was not modified (e.g. by 
a misbehaving application that overwrites parts of the flash).

It could also be enhanced by checking the MD5 digest of the current
flash image against a MD5 checksum placed on the TFTP server to ensure
integrity of the whole flashing process.


Bootloader functionality:
=========================

As just mentioned above, this bootloader is just a normal Nut/OS
application and therefor the code starts with initialisation of 
the UART (used to show debug informations) if debug output is enbled.

Next, the function check_or_save_md5() is called. Depending of it's 
return code (0 == image is ok, -1 == image broken / saved md5 digest 
does not match) the image is started or the network will be initialised 
and the tftp download gets started.

Booting of the application code is implemeted in function boot(), which
is defined as ram resident code. To avoid any malfunction caused by
interrupts generated during application initialisation (in crtxxx.S) all 
interrupts have to be disabled befor jumping into the application code.
Same is done with all peripheral clocks. From the application point of
view the jump to the application start address (0x0010C000 in this case)
should behave the same way as if the application would be linked to address
0x000000000 and therefor would be started with a fresh initialised CPU.


Bootloader configuration:
=========================

Debug output:

Debug output is send to UART0 at 115200,8,N,1

IP and TFTP settings:

Take a look to debug.h, all configuration settings can be found there...
Currently the bootloader just support a static configuration (static IP, static
server IP etc.). One idea for enhancements would be support of the bootp 
protocol.


Makefile / Nut/OS environment:

Just compile this application in the same way as any other Nut/OS application 
too. Use the standard Nut/OS configuration (e.g. enet_sam7x.conf or 
olimex-sam7-ex256.conf

Please adjust the path to the Makedefs and Makerules to fit your Nut/OS 
application directory settings.


Application configuration:
==========================

As the application code is placed into the flash with a configured offset
of 48kByte (at address 0x0010C000) you have to make sure to use the correct
linker script. Please change the following settings in the Nut/OS configurator:

Nut/OS Components --> Tools --> GCC-Setings --> Linker script 

to use one of the bootrom linker scripts for the at91sam7x256_bootrom.ld or 
at91sam7x512_bootrom.ld


How to retrigger the software download:

If an application image is flashed, the download procedure will not start again 
after a power cycle or reset but the bootloader will directly start the 
application.

To trigger the bootloader again, the MD5 checksum in the confnet struct (saved 
in the configured flash sector or NVMem) needs to be cleared.

See the simple demo application code in the "demo" folder for an example...










