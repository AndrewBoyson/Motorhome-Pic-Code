
#include <xc.h>
#include <stdint.h>

#include "eeprom.h"
#include "mstimer.h"

#define RESET_COUNT TMR1H = (~_length) >> 8;  TMR1L = (~_length) & 0xFF;

#define ONE_MS 8000U
#define ONE_MS_MAX (ONE_MS + (ONE_MS >> 3)) 
#define ONE_MS_MIN (ONE_MS - (ONE_MS >> 3))
#define STORE_INTERVAL_MS (3600 * 1000UL) //Number of changes before storing the length

static uint16_t _length = 0;
static int32_t  _extMinusIntMs = 0;
static uint32_t _msSaved = 0;

static unsigned char _eepromAddrMsTickCountU16 = 0;

static void setLength(uint16_t newLength)
{
    if (newLength < ONE_MS_MIN) newLength = ONE_MS_MIN;
    if (newLength > ONE_MS_MAX) newLength = ONE_MS_MAX;
    di();
        _length = newLength;
    ei();
}
static void saveLength()
{
    _msSaved = MsTimerCount;
    EepromSaveU16(_eepromAddrMsTickCountU16, _length);
}
static void adjustLength(uint16_t newLength)
{
    setLength(newLength);
    if (MsTimerCount > _msSaved + STORE_INTERVAL_MS) saveLength();
}

int32_t MsTickerGetExtMinusIntMs()
{
    return _extMinusIntMs;
}
uint16_t MsTickerGetLength()
{
    return _length;
}
void MsTickerSetLength(uint16_t newLength)
{
    setLength(newLength);
    saveLength();
}

void MsTickerRegulate(uint32_t second1970)
{
    static uint32_t prevSecond1970 = 0;
    static uint32_t prevMsCount    = 0;
    if (prevSecond1970)
    {
        uint32_t extElapsedMs = (second1970 - prevSecond1970) * 1000;
        uint32_t intElapsedMs = MsTimerCount - prevMsCount;
        _extMinusIntMs = (int32_t)(extElapsedMs - intElapsedMs);
        if (_extMinusIntMs > 0) adjustLength(_length - 1); //Ticker is too slow
        if (_extMinusIntMs < 0) adjustLength(_length + 1); //Ticker is too quick
    }
    prevSecond1970 = second1970;
    prevMsCount    = MsTimerCount;
}
char MsTickerHadInterrupt()
{
    return TMR1IF;
}
void MsTickerHandleInterrupt()
{
    RESET_COUNT
    TMR1IF = 0;
}
void MsTickerInit(unsigned char eepromAddrMsTickCountU16)
{
    _eepromAddrMsTickCountU16 = eepromAddrMsTickCountU16;
    uint16_t newLength = EepromReadU16(_eepromAddrMsTickCountU16);
    setLength(newLength);
    RESET_COUNT
    TMR1IF = 0;
    TMR1IE = 1;
    
    T1GCONbits.TMR1GE = 0;   //Disable gate function
    
    T1CONbits.TMR1CS = 1;    //Timer1 clock source is from the instruction clock (Fosc)) (8MHz)
    T1CONbits.T1CKPS = 0;    //Prescale by 1 (8MHz)
    T1CONbits.SOSCEN = 0;    //Disable the external SOSC
    T1CONbits.nT1SYNC = 1;   //Don't synchronise the external clock
    T1CONbits.RD16 = 1;      //Enable reading in one operation
    T1CONbits.TMR1ON = 1;    //Start timer 1
}

