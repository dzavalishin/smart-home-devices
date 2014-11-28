#include "child-1wire.h"
//#include "i2c.h"

#include <dev/twif.h>
//#include <sys/thread.h>
#include <sys/timer.h>


#define CH1W_DEBUG 1

#if CH1W_DEBUG
#include <stdio.h>
#endif


#define SLEEP_BETWEEN_W_AND_R_MSEC 25
#define WR_WAIT_TIME_MSEC 50
#define RD_WAIT_TIME_MSEC 50

//#define CH1W_ADDR ((TWI_SLAVE_ADDRESS+0)<<1)
// NutOS does it itself
#define CH1W_ADDR (TWI_SLAVE_ADDRESS+0)



static uint8_t ch1w_SendCmd(uint8_t cmd, uint8_t *reply, uint8_t reply_size);
static uint8_t ch1w_SendCmdArg(uint8_t cmd, uint8_t arg, uint8_t *reply, uint8_t reply_size);



void
    child_1wire_init(void)
{
#if CH1W_DEBUG
    printf("1-wire child processor init...");
#endif

    ch1w_reset();

    NutSleep(500);

#if CH1W_DEBUG
    ch1w_read_temperature( 0 );
    printf(" done\n");
#endif

    NutSleep(500);

}









static uint8_t ch1w_SendCmd(uint8_t cmd, uint8_t *reply, uint8_t reply_size)
{
#if CH1W_DEBUG
    printf("send cmd 0x%02X\n", cmd);
#endif

    int ret = TwMasterTransact(CH1W_ADDR, &cmd, 1, 0, 0, WR_WAIT_TIME_MSEC );

    if(ret < 0)
    {
        return TwMasterError();
    }

    NutSleep(SLEEP_BETWEEN_W_AND_R_MSEC);

    if(reply_size <= 0)
        return 0;

    ret = TwMasterTransact(CH1W_ADDR, 0, 0, reply, reply_size, RD_WAIT_TIME_MSEC );

    if(ret < 0)
    {
        return TwMasterError();
    }

    return ret == reply_size ? 0 : -1;
}


static uint8_t ch1w_SendCmdArg(uint8_t cmd, uint8_t arg, uint8_t *reply, uint8_t reply_size)
{
#if CH1W_DEBUG
    printf("send cmd 0x%02X arg 0x%02X\n", cmd, arg);
#endif
    uint8_t data[2];

    // prepare command
    data[0] = cmd;
    data[1] = arg;

    // 50 msec is too much?
    int ret = TwMasterTransact(CH1W_ADDR, &data, 2, 0, 0, WR_WAIT_TIME_MSEC );

#if CH1W_DEBUG
    printf("ret = %d\n", ret);
#endif

    if(ret < 0)
    {
        return TwMasterError();
    }

    NutSleep(SLEEP_BETWEEN_W_AND_R_MSEC);

    if(reply_size <= 0)
        return 0;

#if CH1W_DEBUG
    printf("read reply %d bytes\n", reply_size);
#endif
    ret = TwMasterTransact(CH1W_ADDR, 0, 0, reply, reply_size, RD_WAIT_TIME_MSEC );

#if CH1W_DEBUG
    printf("ret = %d\n", ret);
#endif

    if(ret < 0)
    {
        return TwMasterError();
    }


    return ret == reply_size ? 0 : -1;
}



















uint8_t ch1w_reset(void)
{
    //    return ch1w_SendCmd(TWI_CMD_RESET_DEVICE, 0, 0 );
    return ch1w_SendCmdArg(TWI_CMD_RESET_DEVICE, 0, 0, 0 ); // 4 debug
}


uint16_t ch1w_read_temperature(uint8_t channel_number )
{
    uint8_t buf[3];
    uint8_t i2cStat = ch1w_SendCmdArg( TWI_CMD_READ_TEMPERATURE, channel_number, buf, sizeof(buf) );


    if(i2cStat)
    {
        printf("I2C child temperature read error %d\r\n", i2cStat);
        return -1;
    }

    if(buf[0] != TWI_CMD_READ_TEMPERATURE )
    {
        //printf("I2C child temperature read invalid reply: %02X %02X %02X\r\n", buf[0], buf[1], buf[2]);
        return -1;
    }

    uint16_t ret = buf[1]; ret |= ((buf[2] << 8) & 0xFF00u );


#if CH1W_DEBUG
    printf("I2C child temp on chan %d is 0x%04X\n", channel_number, ret );
#endif

    return ret;
}

