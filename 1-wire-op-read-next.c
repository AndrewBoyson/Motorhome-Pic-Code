
#include "mstimer.h"

#include "1-wire-interface.h"
#include "1-wire-roms.h"

static uint8_t romIndex = 0;
static uint64_t romCode = 0;
static uint8_t matchByte = 0;
static uint8_t matchIndex = 0;
static uint8_t _tempLsb = 0;
static uint8_t _tempMsb = 0;

static int16_t getTemperature()
{
    uint16_t temp12bit = (uint16_t)_tempMsb << 8;
    temp12bit |= _tempLsb;
    return (int16_t)temp12bit;
}
enum todos
{
    TODO_START,
    TODO_RESET_CMD,
    TODO_RESET_WAIT,
    TODO_MATCH_ROM_CMD,
    TODO_MATCH_ROM_WAIT,
    TODO_SEND_MATCH_LOOP_START,
    TODO_SEND_MATCH_LOOP,
    TODO_MATCH_BYTE_SEND,
    TODO_MATCH_BYTE_WAIT,
    TODO_SEND_MATCH_LOOP_END,
    TODO_READ_SP_CMD,
    TODO_READ_SP_WAIT,
    TODO_LSB_CMD,
    TODO_LSB_WAIT,
    TODO_LSB_INT,
    TODO_MSB_CMD,
    TODO_MSB_WAIT,
    TODO_MSB_INT,
    TODO_SAVE_DATA,
    TODO_END
};
static enum todos _todo;
void OneWireOpReadNextInit()
{
     _todo = 0;
}
int OneWireOpReadNext() //returns 0 to continue, -1 on error, +1 if finished
{
    switch (_todo)
    {
        default:
        case TODO_START:
            _tempLsb = 0;
            _tempMsb = 0x50; //Invalid temperature - used before saving data
            matchIndex = 0;
            romCode = OneWireRom[romIndex];
            if (romCode >> 56 == 0x28) _todo++;
            else                       _todo = TODO_END;
            return 0;
        case TODO_RESET_CMD:              OneWireInterfaceResetBus                     ()            ; _todo++; return 0;
        case TODO_RESET_WAIT:        if (!OneWireInterfaceGetBusIsBusy                 ()            ) _todo++; return 0;
        
        case TODO_MATCH_ROM_CMD:          OneWireInterfaceWriteByteToBus               (0x55)        ; _todo++; return 0;
        case TODO_MATCH_ROM_WAIT:    if (!OneWireInterfaceGetBusIsBusy                 ()            ) _todo++; return 0;
        case TODO_SEND_MATCH_LOOP_START:
            matchIndex = 0;
            _todo++;
            return 0;
        case TODO_SEND_MATCH_LOOP:
            matchByte = (uint8_t)((romCode >> ((7 - matchIndex) * 8)) & 0xFF);
            _todo++;
            return 0;
        case TODO_MATCH_BYTE_SEND:         OneWireInterfaceWriteByteToBus              (matchByte)   ; _todo++; return 0;
        case TODO_MATCH_BYTE_WAIT:    if (!OneWireInterfaceGetBusIsBusy                ()            ) _todo++; return 0;
        case TODO_SEND_MATCH_LOOP_END:
            matchIndex++;
            if (matchIndex >= 8) _todo++;
            else                 _todo = TODO_SEND_MATCH_LOOP;
            return 0;
            
        case TODO_READ_SP_CMD:            OneWireInterfaceWriteByteToBus               (0xBE)        ; _todo++; return 0;
        case TODO_READ_SP_WAIT:      if (!OneWireInterfaceGetBusIsBusy                 ()            ) _todo++; return 0;
        case TODO_LSB_CMD:                OneWireInterfaceReadByteFromBusToDataRegister()            ; _todo++; return 0;
        case TODO_LSB_WAIT:          if (!OneWireInterfaceGetBusIsBusy                 ()            ) _todo++; return 0;
        case TODO_LSB_INT:                OneWireInterfaceGetDataRegister              (&_tempLsb)   ; _todo++; return 0;
        case TODO_MSB_CMD:                OneWireInterfaceReadByteFromBusToDataRegister()            ; _todo++; return 0;
        case TODO_MSB_WAIT:          if (!OneWireInterfaceGetBusIsBusy                 ()            ) _todo++; return 0;
        case TODO_MSB_INT:                OneWireInterfaceGetDataRegister              (&_tempMsb)   ; _todo++; return 0;
        case TODO_SAVE_DATA:
            if (_tempMsb != 0x50) //Save temperature if valid
            {
                OneWireRomData[romIndex] = getTemperature();
                OneWireRomMs  [romIndex] = MsTimerCount;
            }
            _todo++;
            return 0;
        case TODO_END:
            romIndex++;
            if (romIndex >= MAX_ROMS) romIndex = 0;
            _todo++;
            return 1;
    }
}