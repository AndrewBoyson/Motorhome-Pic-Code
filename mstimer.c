#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#include "mstimer.h"
#include "msticker.h"

uint32_t _msCount = 0;     //This is incremented by interrupt. Do not use directly
uint32_t MsTimerCount = 0; //This is interrupt protected copied from _msCount every scan

uint32_t _lastCount = 0;
 int16_t MsTimerScanTime = 0;

bool MsTimerAbsolute(uint32_t untilMs)                            //This uses signed comparison so it is limited to 24 days
{
    return (int)(MsTimerCount - untilMs) > 0;
}

bool MsTimerRelative(uint32_t baseMsCount, uint32_t intervalMs) //This uses unsigned comparison so it is limited to 49 days
{
    return MsTimerCount - baseMsCount >= intervalMs;
}

bool MsTimerRepetitive(uint32_t* pBaseMsCount, uint32_t intervalMs)
{
    if (MsTimerCount - *pBaseMsCount >= intervalMs) //All unsigned wrap around arithmetic
    {
        *pBaseMsCount += intervalMs;
        return true;
    }
    return false;
}

void MsTimerTickHandler()
{
    _msCount++;
}
void MsTimerMain()
{
    di();
        MsTimerCount = _msCount;
    ei();
    
    int16_t scanTime = (int16_t)(MsTimerCount - _lastCount);
    if (scanTime > MsTimerScanTime) MsTimerScanTime++;
    if (scanTime < MsTimerScanTime) MsTimerScanTime--;
    
    _lastCount = MsTimerCount;
    
}