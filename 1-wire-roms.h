#include <stdint.h>

#define ONE_WIRE_ROMS_STALE     0x5000
#define ONE_WIRE_ROMS_NOT_FOUND 0x5100

#define MAX_ROMS 4
extern uint64_t OneWireRom    [];
extern uint32_t OneWireRomMs  [];
extern  int16_t OneWireRomData[];

extern int16_t OneWireRomDataFromCode(uint64_t code);
extern void OneWireRomDataInit(void);