#include <stdint.h>

#include "1-wire-interface.h"
#include "1-wire-device.h"
#include "1-wire-ops.h"

void OneWireInit(uint8_t i2cAddress)
{
    OneWireInterfaceInit(i2cAddress);
    OneWireDeviceInit();
    OneWireOpsInit();
}

void OneWireMain()
{
    OneWireOpsMain();
}