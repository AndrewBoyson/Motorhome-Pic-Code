#include <stdint.h>

extern char EepromReadChar(unsigned char eadr);
extern void EepromSaveChar(unsigned char eadr, char byte);

extern void EepromReadData(unsigned char eadr, unsigned char length, char *p);
extern void EepromSaveData(unsigned char eadr, unsigned char length, char *p);

extern uint8_t  EepromReadU8 (unsigned char eadr);
extern void     EepromSaveU8 (unsigned char eadr,   uint8_t v);

extern  int8_t  EepromReadS8 (unsigned char eadr);
extern void     EepromSaveS8 (unsigned char eadr,    int8_t v);

extern uint16_t EepromReadU16 (unsigned char eadr);	
extern void     EepromSaveU16 (unsigned char eadr, uint16_t v);	

extern  int16_t EepromReadS16 (unsigned char eadr);	
extern void     EepromSaveS16 (unsigned char eadr,  int16_t v);

extern uint32_t EepromReadU32 (unsigned char eadr);	
extern void     EepromSaveU32 (unsigned char eadr, uint32_t v);	

extern  int32_t EepromReadS32 (unsigned char eadr);	
extern void     EepromSaveS32 (unsigned char eadr,  int32_t v);

extern uint64_t EepromReadU64 (unsigned char eadr);	
extern void     EepromSaveU64 (unsigned char eadr, uint64_t v);	

extern  int64_t EepromReadS64 (unsigned char eadr);	
extern void     EepromSaveS64 (unsigned char eadr,  int64_t v);	
