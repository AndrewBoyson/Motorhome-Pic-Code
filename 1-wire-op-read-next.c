
#include "mstimer.h"

#include "1-wire-interface.h"
#include "1-wire-device.h"
#include "1-wire-crc.h"

static uint8_t romIndex = 0;
static uint64_t romCode = 0;
static uint8_t matchByte = 0;
static uint8_t matchIndex = 0;
static uint8_t scratchpadLsb = 0;
static uint8_t scratchpadMsb = 0;
static uint8_t scratchpadByte2 = 0;
static uint8_t scratchpadByte3 = 0;
static uint8_t scratchpadConfig = 0;
static uint8_t scratchpadByte5 = 0; //Reserved - always FFh
static uint8_t scratchpadByte6 = 0; //Reserved
static uint8_t scratchpadByte7 = 0; //Reserved - always 10h
static uint8_t scratchpadCrc = 0;

static int16_t getTemperature()
{
    uint16_t temp12bit = (uint16_t)scratchpadMsb << 8;
    temp12bit |= scratchpadLsb;
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
    TODO_SP_LSB_CMD,
    TODO_SP_LSB_WAIT,
    TODO_SP_LSB_INT,
    TODO_SP_MSB_CMD,
    TODO_SP_MSB_WAIT,
    TODO_SP_MSB_INT,
    TODO_SP_BYTE2_CMD,
    TODO_SP_BYTE2_WAIT,
    TODO_SP_BYTE2_INT,
    TODO_SP_BYTE3_CMD,
    TODO_SP_BYTE3_WAIT,
    TODO_SP_BYTE3_INT,
    TODO_SP_CONFIG_CMD,
    TODO_SP_CONFIG_WAIT,
    TODO_SP_CONFIG_INT,
    TODO_SP_BYTE5_CMD,
    TODO_SP_BYTE5_WAIT,
    TODO_SP_BYTE5_INT,
    TODO_SP_BYTE6_CMD,
    TODO_SP_BYTE6_WAIT,
    TODO_SP_BYTE6_INT,
    TODO_SP_BYTE7_CMD,
    TODO_SP_BYTE7_WAIT,
    TODO_SP_BYTE7_INT,
    TODO_SP_CRC_CMD,
    TODO_SP_CRC_WAIT,
    TODO_SP_CRC_INT,
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
            scratchpadLsb    = 0;
            scratchpadMsb    = 0x50; //Invalid temperature - used before saving data
            scratchpadByte2  = 0;
            scratchpadByte3  = 0;
            scratchpadConfig = 0;
            scratchpadByte5  = 0;
            scratchpadByte6  = 0;
            scratchpadByte7  = 0;
            scratchpadCrc    = 0;
            matchIndex = 0;
            romCode = OneWireDeviceGetIdByIndex(romIndex);
            if (romCode >> 56 == 0x28) _todo++;
            else                       _todo = TODO_END;
            return 0;
        case TODO_RESET_CMD:              OneWireInterfaceResetBus                     ()              ; _todo++; return 0;
        case TODO_RESET_WAIT:        if (!OneWireInterfaceGetBusIsBusy                 ()              ) _todo++; return 0;
        
        case TODO_MATCH_ROM_CMD:          OneWireInterfaceWriteByteToBus               (0x55)          ; _todo++; return 0;
        case TODO_MATCH_ROM_WAIT:    if (!OneWireInterfaceGetBusIsBusy                 ()              ) _todo++; return 0;
        case TODO_SEND_MATCH_LOOP_START:
            matchIndex = 0;
            _todo++;
            return 0;
        case TODO_SEND_MATCH_LOOP:
            matchByte = (uint8_t)((romCode >> ((7 - matchIndex) * 8)) & 0xFF);
            _todo++;
            return 0;
        case TODO_MATCH_BYTE_SEND:         OneWireInterfaceWriteByteToBus              (matchByte)     ; _todo++; return 0;
        case TODO_MATCH_BYTE_WAIT:    if (!OneWireInterfaceGetBusIsBusy                ()              ) _todo++; return 0;
        case TODO_SEND_MATCH_LOOP_END:
            matchIndex++;
            if (matchIndex >= 8) _todo++;
            else                 _todo = TODO_SEND_MATCH_LOOP;
            return 0;
            
        case TODO_READ_SP_CMD:            OneWireInterfaceWriteByteToBus               (0xBE)             ; _todo++; return 0;
        case TODO_READ_SP_WAIT:      if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_LSB_CMD:             OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_LSB_WAIT:       if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_LSB_INT:             OneWireInterfaceGetDataRegister              (&scratchpadLsb   ); _todo++; return 0;
        case TODO_SP_MSB_CMD:             OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_MSB_WAIT:       if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_MSB_INT:             OneWireInterfaceGetDataRegister              (&scratchpadMsb   ); _todo++; return 0;
        case TODO_SP_BYTE2_CMD:           OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_BYTE2_WAIT:     if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_BYTE2_INT:           OneWireInterfaceGetDataRegister              (&scratchpadByte2 ); _todo++; return 0;
        case TODO_SP_BYTE3_CMD:           OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_BYTE3_WAIT:     if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_BYTE3_INT:           OneWireInterfaceGetDataRegister              (&scratchpadByte3 ); _todo++; return 0;
        case TODO_SP_CONFIG_CMD:          OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_CONFIG_WAIT:    if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_CONFIG_INT:          OneWireInterfaceGetDataRegister              (&scratchpadConfig); _todo++; return 0;
        case TODO_SP_BYTE5_CMD:           OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_BYTE5_WAIT:     if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_BYTE5_INT:           OneWireInterfaceGetDataRegister              (&scratchpadByte5 ); _todo++; return 0;
        case TODO_SP_BYTE6_CMD:           OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_BYTE6_WAIT:     if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_BYTE6_INT:           OneWireInterfaceGetDataRegister              (&scratchpadByte6 ); _todo++; return 0;
        case TODO_SP_BYTE7_CMD:           OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_BYTE7_WAIT:     if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_BYTE7_INT:           OneWireInterfaceGetDataRegister              (&scratchpadByte7 ); _todo++; return 0;
        case TODO_SP_CRC_CMD:             OneWireInterfaceReadByteFromBusToDataRegister()                 ; _todo++; return 0;
        case TODO_SP_CRC_WAIT:       if (!OneWireInterfaceGetBusIsBusy                 ()                 ) _todo++; return 0;
        case TODO_SP_CRC_INT:             OneWireInterfaceGetDataRegister              (&scratchpadCrc   ); _todo++; return 0;
        case TODO_SAVE_DATA:
            OneWireCrcReset();
            OneWireCrcAddByte(scratchpadLsb);
            OneWireCrcAddByte(scratchpadMsb);
            OneWireCrcAddByte(scratchpadByte2);
            OneWireCrcAddByte(scratchpadByte3);
            OneWireCrcAddByte(scratchpadConfig);
            OneWireCrcAddByte(scratchpadByte5);
            OneWireCrcAddByte(scratchpadByte6);
            OneWireCrcAddByte(scratchpadByte7);
            OneWireCrcAddByte(scratchpadCrc);
            if (OneWireCrcGetResult() == 0) OneWireDeviceAddDataByIndex(romIndex, getTemperature());
            //if (scratchpadMsb != 0x50 && scratchpadByte7 == 0x10) OneWireDeviceAddDataByIndex(romIndex, getTemperature());
            _todo++;
            return 0;
        case TODO_END:
            romIndex++;
            if (romIndex >= ONE_WIRE_DEVICE_LIST_SIZE) romIndex = 0;
            _todo++;
            return 1;
    }
}