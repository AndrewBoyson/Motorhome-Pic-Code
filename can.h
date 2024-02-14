extern void   CanInit(void);
extern void   CanMain(void);
extern void (*CanReceive)(uint16_t id, uint8_t length, void* pData);
extern char   CanTransmit(uint16_t id, uint8_t length, void* pData);

struct CanTransmitState
{
    uint32_t msSent;
    char     send;
    uint64_t lastValue;
};

extern void CanTransmitOnChange(struct CanTransmitState *pState, uint16_t node, uint16_t id, uint8_t length, void *pValue);