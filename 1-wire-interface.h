#include <stdint.h>

extern void OneWireInterfaceInit(uint8_t i2cAddress);

extern void OneWireInterfaceResetMaster(void);
extern char OneWireInterfaceGetMasterIsReset(void);

extern int  OneWireInterfaceGetStatusRegister(uint8_t* pByte);
extern int  OneWireInterfaceGetConfigRegister(uint8_t* pByte);
extern int  OneWireInterfaceGetDataRegister(uint8_t* pByte);
extern int  OneWireInterfaceSetConfigRegister(uint8_t byte);

extern int  OneWireInterfaceSetNormalPullUp(void);
extern int  OneWireInterfaceSetStrongPullUp(void);

extern char OneWireInterfaceGetBusIsBusy(void);
extern int  OneWireInterfaceResetBus(void);
extern int  OneWireInterfaceWriteByteToBus(uint8_t byte);
extern int  OneWireInterfaceReadByteFromBusToDataRegister(void);
extern int  OneWireInterfaceGenerateTripletOnBus(char direction);
extern int  OneWireInterfaceGetTripletResult(char* pBusy, char* pBit1, char* pBit2, char* pDir);
