#include <stdint.h>
#include <stdbool.h>

extern uint16_t HrTimerCount(void);

extern bool     HrTimerRepetitiveTick (uint16_t* pLastCount, uint16_t interval);
extern bool     HrTimerRelativeMs     (uint16_t baseHrCount, uint16_t intervalMs); //This uses unsigned comparison so it is limited to 30.5 seconds

extern uint16_t HrTimerSinceRepetitive(uint16_t* pLastCount);
extern uint16_t HrTimerSince          (uint16_t   lastCount);

extern void     HrTimerInit(void);

#define HR_TIMER_COUNT_PER_SECOND 2000000U
#define HR_TIMER_COUNT_PER_MS        2000U
