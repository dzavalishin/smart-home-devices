#include <dev/debug.h>


#include <sys/device.h>
#include <sys/file.h>

#include <arch/arm.h>
#include <dev/s3c4510b_hw.h>



static NUTFILE dbgfile;
/*!
 * \brief Handle I/O controls for debug device.
 *
 * The debug device doesn't support any.
 *
 * \return Always -1.
 */
static int DebugIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    return -1;
}

/*!
 * \brief Initialize debug device.
 *
 * Simply enable the device.

 * Baudrate divisor set to x for
 * 19200 Baud at 50 MHz.
 *
 * \return Always 0.
 */
static int DebugInit(NUTDEVICE * dev)
{
    outl(0x03,  DEBUG_UARTLCON_BASE);
	outl(0x09,  DEBUG_UARTCONT_BASE);
	outl(0x500, DEBUG_UARTBRD_BASE);

    return 0;
}

/*!
 * \brief Send a single character to debug device.
 *
 * A newline character will be automatically prepended
 * by a carriage return.
 */
static void DebugPut(char ch)
{
    if (ch == '\n')
        DebugPut('\r');

	while (!(inl(DEBUG_CHK_STAT_BASE) & 0x40));
	outl(ch, DEBUG_TX_BUFF_BASE);
}

/*!
 * \brief Read a single character from debug device.
 *
 * Non blocking version of DebugGet(). If I/O operation is pending
 * this function returns -1.
 */
static int DebugGetNB(void)
{
	if (inl(DEBUG_CHK_STAT_BASE) & 0x20)
		return (int)inl(DEBUG_RX_BUFF_BASE);

	return -1;
}

/*!
 * \brief Read one character from debug device.
 *
 * This function doesn't wait for a character. If I/O operation is pending,
 * it returns /0/-1
 * \return Number of characters read.
 */
static int DebugRead(NUTFILE * fp, void *buffer, int len)
{
    int c, l = 0;
    char *cp = buffer;

    while (l < len) {
        c = DebugGetNB();
        if (c == -1) {
            *cp++ = '\0';
            return 0;
        }
        cp[l++] = (unsigned char) c;
    }

    return l;
}

/*!
 * \brief Send characters to debug device.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
static int DebugWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    int c = len;
    CONST char *cp = buffer;

    while (c--)
        DebugPut(*cp++);
    return len;
}

/*!
 * \brief Open debug device.
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *DebugOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    dbgfile.nf_next = 0;
    dbgfile.nf_dev = dev;
    dbgfile.nf_fcb = 0;

    return &dbgfile;
}

/*!
 * \brief Close debug device.
 *
 * \return Always 0.
 */
static int DebugClose(NUTFILE * fp)
{
    return 0;
}

/*!
 * \brief Debug device information structure.
 */
NUTDEVICE devDebug0 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', 'a', 'r', 'm', 'd', 0} /* uartarmd */
    ,                           /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    0,                          /*!< Driver control block. */
    DebugInit,                  /*!< Driver initialization routine. */
    DebugIOCtl,                 /*!< Driver specific control function. */
    DebugRead,
    DebugWrite,
    /* Write from program space data to device. not havard structure */
    DebugOpen,
    DebugClose,
    0
};

