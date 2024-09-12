#include <stdint.h>

extern void    OneWireCrcReset(void);
extern void    OneWireCrcAddByte(uint8_t data);
extern uint8_t OneWireCrcGetResult(void);

