extern void   CanInit(void);
extern void   CanMain(void);
extern void (*CanReceive)(uint16_t id, uint8_t length, void* pData);
extern char   CanTransmit(uint16_t id, uint8_t length, void* pData);