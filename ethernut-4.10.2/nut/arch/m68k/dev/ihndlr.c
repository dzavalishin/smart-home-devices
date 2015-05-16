/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * $Log$
 * Revision 1.1  2005/07/26 18:02:41  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*! 
 * \fn SIG_INTERRUPT0(void)
 * \brief External interrupt 0 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT0:iv_INT0
#endif
SIGNAL(SIG_INTERRUPT0)
{
    CallHandler(&irqHandler[IRQ_INT0]);
}

/*! \fn SIG_INTERRUPT1(void)
 * \brief External interrupt 1 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT1:iv_INT1
#endif
SIGNAL(SIG_INTERRUPT1)
{
    CallHandler(&irqHandler[IRQ_INT1]);
}

/*! \fn SIG_INTERRUPT2(void)
 * \brief External interrupt 2 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT2:iv_INT2
#endif
SIGNAL(SIG_INTERRUPT2)
{
    CallHandler(&irqHandler[IRQ_INT2]);
}

/*! \fn SIG_INTERRUPT3(void)
 * \brief External interrupt 3 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT3:iv_INT3
#endif
SIGNAL(SIG_INTERRUPT3)
{
    CallHandler(&irqHandler[IRQ_INT3]);
}

/*! \fn SIG_INTERRUPT4(void)
 * \brief External interrupt 4 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT4:iv_INT4
#endif
SIGNAL(SIG_INTERRUPT4)
{
    CallHandler(&irqHandler[IRQ_INT4]);
}

/*! \fn SIG_INTERRUPT5(void)
 * \brief External interrupt 5 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT5:iv_INT5
#endif
SIGNAL(SIG_INTERRUPT5)
{
    CallHandler(&irqHandler[IRQ_INT5]);
}

/*! \fn SIG_INTERRUPT6(void)
 * \brief External interrupt 6 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT6:iv_INT6
#endif
SIGNAL(SIG_INTERRUPT6)
{
    CallHandler(&irqHandler[IRQ_INT6]);
}

/*! \fn SIG_INTERRUPT7(void)
 * \brief External interrupt 7 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT7:iv_INT7
#endif
SIGNAL(SIG_INTERRUPT7)
{
    CallHandler(&irqHandler[IRQ_INT7]);
}

/*! \fn SIG_OUTPUT_COMPARE2(void) 
 * \brief Timer 2 output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE2:iv_TIMER2_COMP
#endif
SIGNAL(SIG_OUTPUT_COMPARE2)
{
    CallHandler(&irqHandler[IRQ_TIMER2_COMP]);
}

/*! \fn SIG_OVERFLOW2(void)
 * \brief Timer 2 overflow interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OVERFLOW2:iv_TIMER2_OVF
#endif
SIGNAL(SIG_OVERFLOW2)
{
    CallHandler(&irqHandler[IRQ_TIMER2_OVF]);
}

/*! \fn SIG_INPUT_CAPTURE1(void)
 * \brief Timer 1 input capture interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INPUT_CAPTURE1:iv_TIMER1_CAPT
#endif
SIGNAL(SIG_INPUT_CAPTURE1)
{
    CallHandler(&irqHandler[IRQ_TIMER1_CAPT]);
}

/*! \fn SIG_OUTPUT_COMPARE1A(void)
 * \brief Timer 1A output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE1A:iv_TIMER1_COMPA
#endif
SIGNAL(SIG_OUTPUT_COMPARE1A)
{
    CallHandler(&irqHandler[IRQ_TIMER1_COMPA]);
}

/*! \fn SIG_OUTPUT_COMPARE1B(void)
 * \brief Timer 1B output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE1B:iv_TIMER1_COMPB
#endif
SIGNAL(SIG_OUTPUT_COMPARE1B)
{
    CallHandler(&irqHandler[IRQ_TIMER1_COMPB]);
}

/*! \fn SIG_OVERFLOW1(void)
 * \brief Timer 1 overflow interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OVERFLOW1:iv_TIMER1_OVF
#endif
SIGNAL(SIG_OVERFLOW1)
{
    CallHandler(&irqHandler[IRQ_TIMER1_OVF]);
}

/*! \fn SIG_OUTPUT_COMPARE0(void)
 * \brief Timer 0 output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE0:iv_TIMER0_COMP
#endif
SIGNAL(SIG_OUTPUT_COMPARE0)
{
    CallHandler(&irqHandler[IRQ_TIMER0_COMP]);
}

/*! \fn SIG_OVERFLOW0(void)
 * \brief Timer 0 overflow interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OVERFLOW0:iv_TIMER0_OVF
#endif
SIGNAL(SIG_OVERFLOW0)
{
    CallHandler(&irqHandler[IRQ_TIMER0_OVF]);
}

/*! \fn SIG_SPI(void)
 * \brief SPI interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_SPI:iv_SPI_STC
#endif
SIGNAL(SIG_SPI)
{
    CallHandler(&irqHandler[IRQ_SPI_STC]);
}

#ifdef __AVR_ATmega128__

/*! \fn SIG_UART0_TRANS(void)
 * \brief Uart0 transmit complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART0_TRANS:iv_USART0_TX
#endif
SIGNAL(SIG_UART0_TRANS)
{
    CallHandler(&irqHandler[IRQ_UART_TX]);
}

/*! \fn SIG_UART0_DATA(void)
 * \brief Uart0 data register empty interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART0_DATA:iv_USART0_UDRE
#endif
SIGNAL(SIG_UART0_DATA)
{
    CallHandler(&irqHandler[IRQ_UART_UDRE]);
}

/*! \fn SIG_UART0_RECV(void)
 * \brief Uart0 receive complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART0_RECV:iv_USART0_RX
#endif
SIGNAL(SIG_UART0_RECV)
{
    CallHandler(&irqHandler[IRQ_UART_RX]);
}

#else

/*! \fn SIG_UART_TRANS(void)
 * \brief Uart0 transmit complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART_TRANS:iv_UART_TX
#endif
SIGNAL(SIG_UART_TRANS)
{
    CallHandler(&irqHandler[IRQ_UART_TX]);
}

/*! \fn SIG_UART_DATA(void)
 * \brief Uart0 data register empty interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART_DATA:iv_UART_UDRE
#endif
SIGNAL(SIG_UART_DATA)
{
    CallHandler(&irqHandler[IRQ_UART_UDRE]);
}

/*! \fn SIG_UART_RECV(void)
 * \brief Uart0 receive complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART_RECV:iv_UART_RX
#endif
SIGNAL(SIG_UART_RECV)
{
    CallHandler(&irqHandler[IRQ_UART_RX]);
}

#endif

/*! \fn SIG_ADC(void)
 * \brief ADC conversion complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_ADC:iv_ADC
#endif
SIGNAL(SIG_ADC)
{
    CallHandler(&irqHandler[IRQ_ADC]);
}

#ifdef __AVR_ATmega128__

/*! \fn SIG_EEPROM_READY(void)
 * \brief EEPROM ready interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_EEPROM_READY:iv_EE_READY
#endif
SIGNAL(SIG_EEPROM_READY)
{
    CallHandler(&irqHandler[IRQ_EE_RDY]);
}

#else

/*! \fn SIG_EEPROM(void)
 * \brief EEPROM ready interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_EEPROM:iv_EE_READY
#endif
SIGNAL(SIG_EEPROM)
{
    CallHandler(&irqHandler[IRQ_EE_RDY]);
}

#endif

/*! \fn SIG_COMPARATOR(void)
 * \brief Analog comparator interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_COMPARATOR:iv_ANALOG_COMP
#endif
SIGNAL(SIG_COMPARATOR)
{
    CallHandler(&irqHandler[IRQ_ANA_COMP]);
}

#ifdef __AVR_ATmega128__

/*! \fn SIG_OUTPUT_COMPARE1C(void)
 * \brief Timer 1C output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE1C:iv_TIMER1_COMPC
#endif
SIGNAL(SIG_OUTPUT_COMPARE1C)
{
    CallHandler(&irqHandler[IRQ_TIMER1_COMPC]);
}

/*! \fn SIG_INPUT_CAPTURE3(void)
 * \brief Timer 3 input capture interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INPUT_CAPTURE3:iv_TIMER3_CAPT
#endif
SIGNAL(SIG_INPUT_CAPTURE3)
{
    CallHandler(&irqHandler[IRQ_TIMER3_CAP]);
}

/*! \fn SIG_OUTPUT_COMPARE3A(void)
 * \brief Timer 3A output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE3A:iv_TIMER3_COMPA
#endif
SIGNAL(SIG_OUTPUT_COMPARE3A)
{
    CallHandler(&irqHandler[IRQ_TIMER3_COMPA]);
}

/*! \fn SIG_OUTPUT_COMPARE3B(void)
 * \brief Timer 3B output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE3B:iv_TIMER3_COMPB
#endif
SIGNAL(SIG_OUTPUT_COMPARE3B)
{
    CallHandler(&irqHandler[IRQ_TIMER3_COMPB]);
}

/*! \fn SIG_OUTPUT_COMPARE3C(void)
 * \brief Timer 3C output compare interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OUTPUT_COMPARE3C:iv_TIMER3_COMPC
#endif
SIGNAL(SIG_OUTPUT_COMPARE3C)
{
    CallHandler(&irqHandler[IRQ_TIMER3_COMPC]);
}

/*! \fn SIG_OVERFLOW3(void)
 * \brief Timer 3 overflow interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_OVERFLOW3:iv_TIMER3_OVF
#endif
SIGNAL(SIG_OVERFLOW3)
{
    CallHandler(&irqHandler[IRQ_TIMER3_OVF]);
}

/*! \fn SIG_UART1_RECV(void)
 * Uart1 receive complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART1_RECV:iv_USART1_RX
#endif
SIGNAL(SIG_UART1_RECV)
{
    CallHandler(&irqHandler[IRQ_UART1_RX]);
}

/*! \fn SIG_UART1_DATA(void)
 * Uart1 data register empty interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART1_DATA:iv_USART1_UDRE
#endif
SIGNAL(SIG_UART1_DATA)
{
    CallHandler(&irqHandler[IRQ_UART1_UDRE]);
}

/*! \fn SIG_UART1_TRANS(void)
 * Uart1 transmit complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART1_TRANS:iv_USART1_TX
#endif
SIGNAL(SIG_UART1_TRANS)
{
    CallHandler(&irqHandler[IRQ_UART1_TX]);
}


/*! \fn SIG_2WIRE_SERIAL(void)
 * \brief Two-wire serial interface interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_2WIRE_SERIAL:iv_TWI
#endif
SIGNAL(SIG_2WIRE_SERIAL)
{
    CallHandler(&irqHandler[IRQ_I2C]);
}

/*! \fn SIG_SPM_READY(void)
 * \brief Store program memory interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_SPM_READY:iv_SPM_READY
#endif
SIGNAL(SIG_SPM_READY)
{
    CallHandler(&irqHandler[IRQ_SPM_RDY]);
}

#endif

/*@}*/
