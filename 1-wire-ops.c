
#include "1-wire-interface.h"
#include "1-wire-op-reset.h"
#include "1-wire-op-conv-temp.h"
#include "1-wire-op-search.h"
#include "1-wire-op-read-next.h"

enum todos
{
    TODO_NOTHING,
    TODO_RESET,
    TODO_SEARCH_ROMS,
    TODO_CONV_TEMP,
    TODO_READ_NEXT
};

int _todo = TODO_NOTHING;

void OneWireOpsInit()
{
    OneWireOpResetInit();
    _todo = TODO_RESET;
}
void OneWireOpsMain()
{
    switch (_todo)
    {
        default:
        case TODO_NOTHING:                                                                                      break;
        case TODO_RESET:        if (OneWireOpReset   ()) { _todo = TODO_SEARCH_ROMS; OneWireOpSearchInit  (); } break;
        case TODO_SEARCH_ROMS:  if (OneWireOpSearch  ()) { _todo = TODO_CONV_TEMP;   OneWireOpConvTempInit(); } break;
        case TODO_CONV_TEMP:    if (OneWireOpConvTemp()) { _todo = TODO_READ_NEXT;   OneWireOpReadNextInit(); } break;
        case TODO_READ_NEXT:    if (OneWireOpReadNext()) { _todo = TODO_SEARCH_ROMS; OneWireOpSearchInit  (); } break;
    }
}