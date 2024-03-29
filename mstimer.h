#include <stdint.h>
#include <stdbool.h>

extern uint32_t MsTimerCount;
extern  int16_t MsTimerScanTime;

extern bool     MsTimerAbsolute  (uint32_t untilMs);
extern bool     MsTimerRelative  (uint32_t   baseMsCount, uint32_t intervalMs);
extern bool     MsTimerRepetitive(uint32_t* pBaseMsCount, uint32_t intervalMs);

extern void     MsTimerTickHandler(void);
extern void     MsTimerMain(void);