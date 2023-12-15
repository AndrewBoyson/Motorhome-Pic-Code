#include <stdint.h>

#include "i2c.h"

uint8_t _i2cAddress = 0;

void OneWireInterfaceInit(uint8_t i2cAddress)
{
    _i2cAddress = i2cAddress;
}

static int sendByte(uint8_t byte)
{
    int result = 0;
    I2CSend(_i2cAddress, 1, &byte, &result);
    return result;
}
static int sendBytePair(uint8_t byte1, uint8_t byte2)
{
    int result = 0;
    uint8_t bytePair[2];
    bytePair[0] = byte1;
    bytePair[1] = byte2;
    I2CSend(_i2cAddress, 2, bytePair, &result);
    return result;
}
static int readByte(uint8_t* pByte)
{
    int result = 0;
    I2CReceive(_i2cAddress, 1, pByte, &result);
    return result;
}

void OneWireInterfaceResetMaster()
{
    sendByte(0xF0);
}
static int setReadPointer(uint8_t code)
{
    int result = 0;
    result = sendBytePair(0xE1, code);
    return result;
}

int OneWireInterfaceGetStatusRegister(uint8_t* pByte)
{
    int result = 0;
    result = setReadPointer(0xF0);
    if (result) return result;
    result = readByte(pByte);
    return result;
}
int OneWireInterfaceGetDataRegister(uint8_t* pByte)
{
    int result = 0;
    result = setReadPointer(0xE1);
    if (result) return result;
    result = readByte(pByte);
    return result;
}
int OneWireInterfaceGetConfigRegister(uint8_t* pByte)
{
    int result = 0;
    result = setReadPointer(0xC3);
    if (result) return result;
    result = readByte(pByte);
    return result;
}
int OneWireInterfaceSetConfigRegister(uint8_t config)
{
    int result = 0;
    result = sendBytePair(0xD2, config);
    return result;
}
int OneWireInterfaceSetNormalPullUp()
{
    return OneWireInterfaceSetConfigRegister(0xE1);
}
int OneWireInterfaceSetStrongPullUp()
{
    return OneWireInterfaceSetConfigRegister(0xA5);
}
int OneWireInterfaceResetBus()
{
    int result = 0;
    result = sendByte(0xB4);
    return result;
}
int OneWireInterfaceWriteByteToBus(uint8_t byte)
{
    int result = 0;
    result = sendBytePair(0xA5, byte);
    return result;
}
int OneWireInterfaceReadByteFromBusToDataRegister()
{
    int result = 0;
    result = sendByte(0x96);
    return result;
}
int OneWireInterfaceGenerateTripletOnBus(char direction)
{
    int result = 0;
    if (direction) result = sendBytePair(0x78, 0x80);
    else           result = sendBytePair(0x78, 0x00);
    return result;
}
char OneWireInterfaceGetMasterIsReset()
{
    uint8_t status = 0;
    OneWireInterfaceGetStatusRegister(&status);
    return status & 0x10;
}
char OneWireInterfaceGetBusIsBusy()
{
    uint8_t status = 0;
    OneWireInterfaceGetStatusRegister(&status);
    return status & 0x01;
}

int OneWireInterfaceGetTripletResult(char* pBusy, char* pBit1, char* pBit2, char* pDir)
{
    int result = 0;
    uint8_t status = 0;
    result = OneWireInterfaceGetStatusRegister(&status);
    
    *pBusy = !!(status & 0x01); //1-wire busy
    *pBit1 = !!(status & 0x20); //Single bit result
    *pBit2 = !!(status & 0x40); //Triplet second bit
    *pDir  = !!(status & 0x80); //Branch direction taken
    
    return result;
}