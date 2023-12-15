#include <xc.h>

/*
For this to work during a device programming you must protect the eeprom:
Project properties>PicKit3>Memories to program>Preserve EEPROM memory 0-3FF.

Parameter D122 section 28.3 typical erase/write cycle time = 4ms so saving 64 byte string will take 256ms.
*/

char EepromReadChar(unsigned char eadr)
{
	while (EECON1) EECON1 = 0;    //Wait for any previous write to complete
	EEADR = eadr;
	EECON1bits.RD = 1;
	return EEDATA;
}	
void EepromSaveChar(unsigned char eadr, char byte)
{
	while (EECON1) EECON1 = 0;    //Wait for any previous write to complete
	EEADR = eadr;
	EECON1bits.RD = 1;            //Read the current contents
	if (EEDATA == byte) return;   //Stop if they are the same
	EEDATA = byte;
	EECON1bits.WREN  = 1;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
	EECON1bits.WREN  = 0;
}

void  EepromReadData (unsigned char eadr, unsigned char length, char *p) { while (length--) *(p++) = EepromReadChar(eadr++); }	
void  EepromSaveData (unsigned char eadr, unsigned char length, char *p) { while (length--)  EepromSaveChar(eadr++, *(p++)); }

uint8_t  EepromReadU8  (unsigned char eadr)              { return (uint8_t) EepromReadChar(eadr);          }	
void     EepromSaveU8  (unsigned char eadr,   uint8_t v) {                  EepromSaveChar(eadr, (char)v); }

 int8_t  EepromReadS8  (unsigned char eadr)              { return ( int8_t) EepromReadChar(eadr);          }	
void     EepromSaveS8  (unsigned char eadr,    int8_t v) {                  EepromSaveChar(eadr, (char)v); }

uint16_t EepromReadU16 (unsigned char eadr)              { uint16_t v = 0;  EepromReadData(eadr, 2, (char*)&v); return v; }	
void     EepromSaveU16 (unsigned char eadr, uint16_t v)  {                  EepromSaveData(eadr, 2, (char*)&v);           }	

 int16_t EepromReadS16 (unsigned char eadr)              {  int16_t v = 0;  EepromReadData(eadr, 2, (char*)&v); return v; }	
void     EepromSaveS16 (unsigned char eadr,  int16_t v ) {                  EepromSaveData(eadr, 2, (char*)&v);           }

uint32_t EepromReadU32 (unsigned char eadr)              { uint32_t v = 0;  EepromReadData(eadr, 4, (char*)&v); return v; }	
void     EepromSaveU32 (unsigned char eadr, uint32_t v)  {                  EepromSaveData(eadr, 4, (char*)&v);           }	

 int32_t EepromReadS32 (unsigned char eadr)              {  int32_t v = 0;  EepromReadData(eadr, 4, (char*)&v); return v; }	
void     EepromSaveS32 (unsigned char eadr,  int32_t v)  {                  EepromSaveData(eadr, 4, (char*)&v);           }

uint64_t EepromReadU64 (unsigned char eadr)              { uint64_t v = 0;  EepromReadData(eadr, 8, (char*)&v); return v; }	
void     EepromSaveU64 (unsigned char eadr, uint64_t v)  {                  EepromSaveData(eadr, 8, (char*)&v);           }	

 int64_t EepromReadS64 (unsigned char eadr)              {  int64_t v = 0;  EepromReadData(eadr, 8, (char*)&v); return v; }	
void     EepromSaveS64 (unsigned char eadr,  int64_t v)  {                  EepromSaveData(eadr, 8, (char*)&v);           }	

