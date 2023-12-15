
#include "1-wire-interface.h"

enum todos
{
    TODO_RESET_CMD,
    TODO_RESET_WAIT,
    TODO_CONFIGURE_CMD,
    TODO_CONFIGURE_WAIT
};
static enum todos _todo;
void OneWireOpResetInit()
{
     _todo = 0;
}
int OneWireOpReset() //returns 0 to continue, -1 on error, +1 if finished
{
    switch (_todo)
    {
        default:
        case TODO_RESET_CMD:           OneWireInterfaceResetMaster     (); _todo++; return 0;
        case TODO_RESET_WAIT:     if ( OneWireInterfaceGetMasterIsReset()) _todo++; return 0;
        case TODO_CONFIGURE_CMD:       OneWireInterfaceSetNormalPullUp (); _todo++; return 0;
        case TODO_CONFIGURE_WAIT: if (!OneWireInterfaceGetMasterIsReset()) _todo++; return 1;
    }
}
