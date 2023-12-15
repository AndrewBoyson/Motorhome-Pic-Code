#include <stdint.h>
#include <stdbool.h>

extern void I2CInit(void);

extern void I2CSend   (uint8_t addr, int length, uint8_t* pData, int* pResult);
extern void I2CReceive(uint8_t addr, int length, uint8_t* pData, int* pResult);

#define I2C_RESULT_SUCCESS   0
#define I2C_RESULT_TIMED_OUT 1
#define I2C_RESULT_NACK      2