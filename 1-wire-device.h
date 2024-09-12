#include <stdint.h>

#define ONE_WIRE_DEVICE_LIST_SIZE 4

#define ONE_WIRE_DEVICE_ID_EMPTY     0x5000
#define ONE_WIRE_DEVICE_ID_NOT_FOUND 0x5001
#define ONE_WIRE_DEVICE_DATA_NOT_SET 0x5002

extern int16_t  OneWireDeviceGetDataByIndex(int index);
extern int16_t  OneWireDeviceGetDataById(uint64_t id);
extern uint64_t OneWireDeviceGetIdByIndex(int index);

extern void     OneWireDeviceInit(void);

extern void     OneWireDeviceAddIdFromSearch(uint64_t id);
extern void     OneWireDeviceAddDataByIndex(int index, int16_t data);
extern void     OneWireDeviceHandleEndOfScan(void);