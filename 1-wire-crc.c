#include <stdint.h>

static uint8_t _crc = 0;

void OneWireCrcReset()
{
    _crc = 0;
}
void OneWireCrcAddByte(uint8_t data)
{
	_crc = _crc ^ data;

	for (int i = 0; i < 8; i++)
    {
		if (_crc & 0x01) _crc = (_crc >> 1) ^ 0x8c;
		else             _crc >>= 1;
	}
}

uint8_t OneWireCrcGetResult()
{
    return _crc;
}