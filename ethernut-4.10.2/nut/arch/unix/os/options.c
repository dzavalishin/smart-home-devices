/*
 * Copyright (C) 2000-2004 by ETH Zurich
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ETH ZURICH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ETH ZURICH
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * unix_options.c - parsing of command line options
 *
 * 2002.11.13 Oliver Kasten <oliver.kasten@inf.ethz.ch>
 * 2003.06.09 Jan Beutel <j.beutel@ieee.org> 
 * 2004.04.01 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */

/**

\file arch/unix/os/options.c

\brief Parsing of command line options. NOTE: Unix emulation only!

*/

#include <compiler.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

/* the command line options are stored here */
emulation_options_t emulation_options;

// default options
static uart_options_t uart_0_opt = { "stdio", 0, 0, -1 };
static uart_options_t uart_1_opt = { "stdio", 0, 0, -1 };
static uart_options_t uart_2_opt = { "stdio", 0, 0, -1 };

static void emulation_options_usage(void);
static void emulation_options_print_all(void);
static void emulation_options_print_uart(int);

/* -------------------------------------------------------------------------
 * Parse command line parameters
 * syntax: -uN (M|hci:O) -h -v
 *  N: uart nr (0/1)
 *  M: unix device name
 *  O: usb bluetooth number: 0..
 * ------------------------------------------------------------------------- */

void emulation_options_parse(int argc, char *argv[])
{

    int opt;

    // set default values
    emulation_options.verbose = 0;
    emulation_options.uart_options[0] = uart_0_opt;
    emulation_options.uart_options[1] = uart_1_opt;
    emulation_options.uart_options[2] = uart_2_opt;

    // parse args 
    while ((opt = getopt(argc, argv, "u:hv::")) != -1) {

        switch (opt) {

        case 'u':
            {

                int devno = 0, bautrate = 0, flowctrl = 0;
                char *device = NULL;

                devno = atoi(optarg);
                // printf( "Parsing options for UART %i\n", devno );

                if ((devno < 0) || (devno > 2)) {
                    printf("ERROR parsing arguments for UART device: " "device number out of range [0..2]\n");
                    exit(1);
                }

                device = strdup(argv[optind++]);
                // printf( "  device: %s\n", device );

                // terminal stdio?
                if (strcmp(device, "stdio") == 0) {

                // usb device ? hci:x
                } else if (strncmp(device, "hci", 3) == 0) {
                    emulation_options.uart_options[devno].usbnum = atoi(device + 3);
				} else {
					// tcp/ip ? host:port
                    // tty

                }
                emulation_options.uart_options[devno].device = device;
                emulation_options.uart_options[devno].bautrate = bautrate;
                emulation_options.uart_options[devno].flowcontrol = flowctrl;

            }
            break;

        case 'v':
            // verbose mode: -v [on|off]

            if (optarg) {
                if (strcmp(argv[optind], "off") == 0)
                    emulation_options.verbose = 0;
                else if (strcmp(argv[optind], "on") == 0)
                    emulation_options.verbose = 1;
                else {
                    printf("ERROR parsing verboseness (-v [on|off])option.\n");
                    exit(1);
                }
            } else {
                // -v without options turnes verbose mode on
                emulation_options.verbose = 1;
            }
            break;

        case 'h':
            emulation_options_usage();
            exit(0);
            break;

        default:
            printf("unrecognized option:\n");
            emulation_options_usage();
            exit(1);

            break;
        }
    }

    if (emulation_options.verbose)
        emulation_options_print_all();
}

void emulation_options_print_all()
{

    int i = 0;

    if (!emulation_options.verbose)
        // not in verbose mode
        return;

    printf("OPTIONS SUMMARY:\n");

    printf("  verbose: %i\n", emulation_options.verbose);

    for (i = 0; i < 3; i++) {
        emulation_options_print_uart(i);
    }

}                               // end _btn_options_print()

void emulation_options_print_uart(int no)
{

    uart_options_t uart_opt = emulation_options.uart_options[no];

    printf("Options for UART %i\n", no);

    printf("  device: %s\n", uart_opt.device);
    if (uart_opt.usbnum >= 0) {
        printf("  hci num : %i\n", (int) uart_opt.usbnum);
    } else {
        printf("  bautrate: %i\n", (int) uart_opt.bautrate);
        printf("  flowcontrol: %i\n", (int) uart_opt.flowcontrol);
    }
}                               // end _btn_uart_options_print()

static void emulation_options_usage()
{
    printf("To properly run an emulated Nut/OS app, some hardware might need to be connected.\n"
           "With the following options you can specify how the uarts are mapped.\n");
    printf("  -h  prints this help.\n");
    printf("  -v  verbose mode. print the options parsed.\n");
    printf("  -u<N> DEVICE \n");
    printf("     map uart N to unix DEVICE \n");
    printf("     example: ./prog -u0 /dev/ttyS0 \n");
    printf("  -u<N> HOST:PORT \n");
    printf("     map uart N to TCP/IP socket with HOST at PORT \n");
    printf("     example: ./prog -u0 localhost:7007 \n");
//    printf("  -u<N> hci<M> \n");
//    printf("     map uart N to the USB BT dongle nr. M\n");
//    printf("     example: ./prog -u0 hci0\n");

    printf("  \n");
    printf("  \n");

}
