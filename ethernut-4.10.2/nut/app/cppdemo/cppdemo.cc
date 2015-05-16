/*
 * Copyright (c) 2005 by Oliver Schulz (MPI)
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*!
 * $Id$
 */

/*!
 * \example cppdemo/cppdemo.cc
 *
 * This trivial sample demonstrates the usage of Nut/OS with C++.
 *
 * You should carefully think about using C++ with tiny embedded systems.
 * This sample just proofs, that it basically works.
 */

#include <cpp/nutcpp.h>

extern "C" {
#include <dev/board.h>
#include <sys/version.h>
#include <inttypes.h>
#include <io.h>
#include <stdio.h>
}



template<class tp_type> class TemplateCounter
{
protected:
    tp_type m_value;

public:
    tp_type value() const { return m_value; }
    void inc() { m_value++; }
    void dec() { m_value--; }
    void set(const tp_type &newValue) { m_value = newValue; }
};



class Counter: public TemplateCounter<uint8_t>
{
public:
    void print(FILE *stream);

    Counter(uint8_t initValue=10);
};


void Counter::print(FILE* stream)
{
    fprintf(stream, "\nCounter value = %i\n", value());
}


Counter::Counter(uint8_t initValue)
{
    m_value = initValue;
}



int main(void) {
    u_long baud = 115200;

    NutRegisterDevice(&DEV_UART0, 0, 0);
    FILE *stream = fopen(DEV_UART0_NAME, "r+");
    _ioctl(_fileno(stream), UART_SETSPEED, &baud);

    fprintf(stream, "\n\nC++ Demo on Nut/OS %s ready.\n", NutVersionString());

    Counter counter;
    counter.print(stream);

    for (;;) {
        char c;
        fread(&c, sizeof(c), 1, stream);

        switch (c) {
        case '+':
            counter.inc();
            counter.print(stream);
            break;
        case '-':
            counter.dec();
            counter.print(stream);
            break;
        case 'r':
            counter.set(0);
            counter.print(stream);
            break;
        default:
            fprintf(stream, "Unknown command.\n");
        }
    }
    return 0;
}


