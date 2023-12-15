
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#include "i2c.h"
#include "hrtimer.h"

#define TIMEOUT_MS 10
#define _XTAL_FREQ 8000000

void I2CInit()
{
    TRISC3 = 1; //Setting as input as given in datasheet
    TRISC4 = 1; //Setting as input as given in datasheet
    
    //Unlock if a slave has SDA low
    for (int i = 0; i < 16 && !RC4; i++)
    {
        TRISC3 = 0; //Set SCL to an output
           RC3 = 0; //Set SCL low
        __delay_us(10);
        TRISC3 = 1; //Set SCL to an input floats SCL high
        __delay_us(10);
    }
    
    SSPCON1 = 0b00101000;   //SSP Module as Master
    SSPCON2 = 0;
    //SSPADD = 19;            //Setting Clock Speed to 100khz 2MHz / 100KHz - 1
    SSPADD = 4;            //Setting Clock Speed to 400khz 2MHz / 400KHz - 1
    SSPSTAT = 0;
}
static char wait()
{
    uint16_t hrTimeOut = HrTimerCount();
    while (SSPSTATbits.RW    //In Master mode: 1 = Transmit is in progress
       ||  SSPCON2bits.ACKEN //Acknowledge Sequence Enable bit
       ||  SSPCON2bits.RCEN  //Receive Enable bit (Master Receive mode only)
       ||  SSPCON2bits.PEN   //Stop Condition Enable bit
       ||  SSPCON2bits.RSEN  //Repeated Start Condition Enable bit
       ||  SSPCON2bits.SEN)  //Start Condition Enable bit
    {
        if (HrTimerRelativeMs(hrTimeOut, TIMEOUT_MS)) return 1; //Failure
    }
    return 0;
}
static char start()
{
    if (wait()) return I2C_RESULT_TIMED_OUT;
    SEN = 1;
    return 0;
}
static char stop()
{
    if (wait()) return I2C_RESULT_TIMED_OUT;
    PEN = 1;
    return 0;
}
static char sendAck(void)
{
    if (wait()) return I2C_RESULT_TIMED_OUT;
    ACKDT = 0; // 0 -> ACK, 1 -> NACK
    ACKEN = 1; // Send ACK Signal
    return 0;
}
static char sendNack(void)
{
    if (wait()) return I2C_RESULT_TIMED_OUT;
    ACKDT = 1; // 1 -> NACK, 0 -> ACK
    ACKEN = 1; // Send NACK Signal
    return 0;
}
static char writeByte(unsigned char Data)
{
    if (wait()) return I2C_RESULT_TIMED_OUT;
    SSPBUF = Data;
    if (wait()) return I2C_RESULT_TIMED_OUT;
    if (ACKSTAT) return I2C_RESULT_NACK;
    return 0;
}
static char readByte(uint8_t* pData, bool isLast)
{
    if (wait()) return I2C_RESULT_TIMED_OUT;
    RCEN = 1;
    if (wait()) return I2C_RESULT_TIMED_OUT;
    *pData = SSPBUF;
    if (isLast) sendNack();
    else        sendAck();
    return 0;
}

void I2CSend(uint8_t addr, int length, uint8_t* pData, int* pResult)
{
    *pResult = 0;
    *pResult = start();
    if (*pResult) return;
    *pResult = writeByte((uint8_t)(addr << 1) + 0); // send address + Write
    if (*pResult) return;
    for (int i = 0; i < length; i++)
    {
        *pResult = writeByte(pData[i]);
        if (*pResult) return;
    }
    *pResult = stop();
}
void I2CReceive(uint8_t addr, int length, uint8_t* pData, int* pResult)
{
    *pResult = 0;
    *pResult = start();
    if (*pResult) return;
    *pResult = writeByte((uint8_t)(addr << 1) + 1);      // send address + Read
    if (*pResult) return;
    for (int i = 0; i < length; i++)
    {
        *pResult = readByte(pData + i, i == length - 1);
        if (*pResult) return;
    }
    *pResult = stop();
}