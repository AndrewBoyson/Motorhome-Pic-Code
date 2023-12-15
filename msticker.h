extern char MsTickerHadInterrupt(void);
extern void MsTickerHandleInterrupt(void);
extern void MsTickerInit(unsigned char eepromAddrMsTickCountU16);

extern void     MsTickerSetLength(uint16_t length);
extern uint16_t MsTickerGetLength(void);

extern int32_t  MsTickerGetExtMinusIntMs(void);
extern void     MsTickerRegulate(uint32_t second1970);