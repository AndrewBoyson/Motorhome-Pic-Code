#include "mstimer.h"

#include "1-wire-interface.h"

enum todos
{
    TODO_RESET_CMD,
    TODO_RESET_WAIT,
    TODO_SKIP_ROM_CMD,
    TODO_SKIP_ROM_WAIT,
    TODO_CONV_CMD,
    TODO_SPU_WAIT,
    TODO_SPU_OFF_CMD,
};
static enum todos _todo;
void OneWireOpConvTempInit()
{
     _todo = 0;
}
int OneWireOpConvTemp() //returns 0 to continue, -1 on error, +1 if finished
{
    static uint32_t msTimerStrongPullUp = 0;
    switch (_todo)
    {
        default:
        case TODO_RESET_CMD:              OneWireInterfaceResetBus      ()    ; _todo++; return 0;
        case TODO_RESET_WAIT:        if (!OneWireInterfaceGetBusIsBusy  ()    ) _todo++; return 0;
        case TODO_SKIP_ROM_CMD:           OneWireInterfaceWriteByteToBus(0xCC); _todo++; return 0;
        case TODO_SKIP_ROM_WAIT:     if (!OneWireInterfaceGetBusIsBusy  ()    ) _todo++; return 0;
        case TODO_CONV_CMD:
            OneWireInterfaceSetStrongPullUp(); //Applied after next command
            OneWireInterfaceWriteByteToBus(0x44); //Convert T function
            msTimerStrongPullUp = MsTimerCount;
            _todo++;
            return 0;
        case TODO_SPU_WAIT:          if (MsTimerRelative(msTimerStrongPullUp, 1000))  _todo++; return 0;
        case TODO_SPU_OFF_CMD:            OneWireInterfaceSetNormalPullUp();          _todo++; return 1;
    }
}
