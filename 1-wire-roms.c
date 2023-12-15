#include <stdint.h>

#include "mstimer.h"

#include "1-wire-roms.h"

uint64_t OneWireRom    [MAX_ROMS];
uint32_t OneWireRomMs  [MAX_ROMS];
 int16_t OneWireRomData[MAX_ROMS];
 
#define STALE_MS 30000

 int16_t OneWireRomDataFromCode(uint64_t code)
 {
     if (code == OneWireRom[0]) return MsTimerCount - OneWireRomMs[0] > STALE_MS ? ONE_WIRE_ROMS_STALE : OneWireRomData[0];
     if (code == OneWireRom[1]) return MsTimerCount - OneWireRomMs[1] > STALE_MS ? ONE_WIRE_ROMS_STALE : OneWireRomData[1];
     if (code == OneWireRom[2]) return MsTimerCount - OneWireRomMs[2] > STALE_MS ? ONE_WIRE_ROMS_STALE : OneWireRomData[2];
     if (code == OneWireRom[3]) return MsTimerCount - OneWireRomMs[3] > STALE_MS ? ONE_WIRE_ROMS_STALE : OneWireRomData[3];
     return ONE_WIRE_ROMS_NOT_FOUND;
 }
 
 void OneWireRomDataInit()
 {
     OneWireRomMs[0] = (uint32_t)-STALE_MS;
     OneWireRomMs[1] = (uint32_t)-STALE_MS;
     OneWireRomMs[2] = (uint32_t)-STALE_MS;
     OneWireRomMs[3] = (uint32_t)-STALE_MS;
 }