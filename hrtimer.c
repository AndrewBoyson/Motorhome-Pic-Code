
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "hrtimer.h"

uint16_t HrTimerCount()
{
    uint16_t thisCount = TMR0L;
    thisCount += (uint16_t)TMR0H << 8;
    return thisCount;
}
uint16_t HrTimerSinceRepetitive(uint16_t* pLastCount)
{
    uint16_t thisCount = HrTimerCount();
    uint16_t period = thisCount - *pLastCount;    
    *pLastCount = thisCount;
    return period;
}
uint16_t HrTimerSince(uint16_t lastCount)
{
    return HrTimerCount() - lastCount; 
}

bool HrTimerRepetitiveTick(uint16_t* pLastCount, uint16_t interval)
{
    uint16_t thisCount = HrTimerCount();
    if (thisCount - *pLastCount >= interval) //All unsigned wrap around arithmetic
    {
        *pLastCount += interval;
        return true;
    }
    return false;
}

bool HrTimerRelativeMs(uint16_t baseHrCount, uint16_t intervalMs) //This uses unsigned comparison so it is limited to 30.5 seconds
{
    uint16_t interval = intervalMs * HR_TIMER_COUNT_PER_MS;
    uint16_t hrCount = HrTimerCount();
    return hrCount - baseHrCount >= interval;
}

void HrTimerInit()
{    
    T0CONbits.T08BIT = 0;    //Configure as a 16 bit timer
    T0CONbits.T0CS = 0;      //Select the instruction clock (Fcy/4) == 8MHz / 4 == 2MHz
    T0CONbits.PSA = 1;       //Prescaler is not assigned
    T0CONbits.TMR0ON = 1;    //Enable
}
