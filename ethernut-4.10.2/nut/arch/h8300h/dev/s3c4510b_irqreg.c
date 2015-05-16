/** \file s3c4510b_irqreg.c.
 *  \brief .
 *  \version .
 *  \date .
 *  \bug .
 *  \warning .
 *  \todo
 *     \li .
 *
 *
****************************************************************************/
#include <arch/arm.h>
#include <dev/s3c4510b_hw.h>
#include <dev/s3c4510b_irqs.h>

IRQ_HANDLER InterruptHandlers[NR_IRQS];

/** \brief .
 *
 *  \param .
 *
 *  \return .
 *
 *  \note .
 *
****************************************************************************/
void ISR_IrqHandler(void)
{
    u_long off = IntOffset >> 2;

    CLEAR_PEND_INT(off);
    CallHandler(&InterruptHandlers[off]);
}

void s3c4510b_int_init()
{
	IntPend = 0x1FFFFF;
	IntMode = INT_MODE_IRQ;

	INT_ENABLE(INT_GLOBAL);
}
