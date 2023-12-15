#include <stdbool.h>

extern void LcdInit(uint8_t i2cAddr);
extern void LcdMain(void);
extern bool LcdIsReady(void);
extern bool LcdIsOn(void);
extern void LcdTurnOn(void);
extern void LcdTurnOff(void);
extern void LcdSendText(const char* pText0, const char* pText1);
