#include <stdint.h>

#include "1-wire-interface.h"
#include "1-wire-roms.h"
#include "1-wire-ops.h"

void OneWireInit(uint8_t i2cAddress)
{
    OneWireInterfaceInit(i2cAddress);
    OneWireRomDataInit();
    OneWireOpsInit();
}

void OneWireMain()
{
    OneWireOpsMain();
}