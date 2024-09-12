#include <stdint.h>
#include "1-wire-device.h"

static uint64_t _ids               [ONE_WIRE_DEVICE_LIST_SIZE];
static   int8_t _scansSinceLastSeen[ONE_WIRE_DEVICE_LIST_SIZE];
static  int16_t _datas             [ONE_WIRE_DEVICE_LIST_SIZE];
static   int8_t _scansSinceLastData[ONE_WIRE_DEVICE_LIST_SIZE];
 
#define MAX_SCANS_SINCE_LAST_SEEN 10
#define MAX_SCANS_SINCE_LAST_DATA 10

int16_t OneWireDeviceGetDataByIndex(int index)
{
    if (_ids[index] == 0)                                        return ONE_WIRE_DEVICE_ID_EMPTY;
    if (_scansSinceLastData[index] >= MAX_SCANS_SINCE_LAST_DATA) return ONE_WIRE_DEVICE_DATA_NOT_SET;
    return _datas[index];
}
int16_t OneWireDeviceGetDataById(uint64_t id)
{
    if (id == 0) return ONE_WIRE_DEVICE_ID_EMPTY;
    for (int index = 0; index < ONE_WIRE_DEVICE_LIST_SIZE; index++)
    {
        if (_ids[index] == id) return OneWireDeviceGetDataByIndex(index);
    } 
    return ONE_WIRE_DEVICE_ID_NOT_FOUND;
}
uint64_t OneWireDeviceGetIdByIndex(int index)
{
    return _ids[index];
}

void OneWireDeviceInit()
{
    for (int index = 0; index < ONE_WIRE_DEVICE_LIST_SIZE; index++)
    {
        _ids               [index] = 0;
        _scansSinceLastData[index] = MAX_SCANS_SINCE_LAST_DATA;
    }
}
static int getIndexOfIdOrNegativeIfNotFound(uint64_t id)
{
    for (int index = 0; index < ONE_WIRE_DEVICE_LIST_SIZE; index++)
    {
        if (_ids[index] == id) return index;
    }
    return -1;
}
static int getFirstEmptyOrOldest()
{
    //Return the first empty slot if there is one
    for (int index = 0; index < ONE_WIRE_DEVICE_LIST_SIZE; index++)
    {
        if (!_ids[index]) return index;
    }
    //Return the oldest existing entry
    int    oldestIndex = 0;
    for (int index = 1; index < ONE_WIRE_DEVICE_LIST_SIZE; index++)
    {
        if (_scansSinceLastSeen[index] > _scansSinceLastSeen[oldestIndex]) oldestIndex = index;
    }
    return oldestIndex;
}

void OneWireDeviceAddIdFromSearch(uint64_t id)
{   
    //If the id exists then just zero the scans since last seen and return
    int index = getIndexOfIdOrNegativeIfNotFound(id);
    if (index >= 0)
    {
        _scansSinceLastSeen[index] = 0;
        return;
    }
    
    //The id was not found so add it in the first available space
    index = getFirstEmptyOrOldest();
    _ids               [index] = id;
    _scansSinceLastSeen[index] = 0;
    _scansSinceLastData[index] = MAX_SCANS_SINCE_LAST_DATA;
}

void OneWireDeviceAddDataByIndex(int index, int16_t data)
{
    _datas[index] = data;
    _scansSinceLastData[index] = 0;
}

void OneWireDeviceHandleEndOfScan()
{
    for (int index = 0; index < ONE_WIRE_DEVICE_LIST_SIZE; index++)
    {
        if (!_ids[index]) continue;
        
        if (_scansSinceLastSeen[index] < MAX_SCANS_SINCE_LAST_SEEN)
        {
            _scansSinceLastSeen[index]++;
        }
        else
        {
            _ids[index] = 0;
        }
        
        if (_scansSinceLastData[index] < MAX_SCANS_SINCE_LAST_DATA)
        {
            _scansSinceLastData[index]++;
        }
    }
}