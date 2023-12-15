
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#include "i2c.h"
#include "mstimer.h"
#include "lcd-1602.h"

static uint8_t _i2cAddr = 0;


/*
 Bits
 ====
 p7 D7
 p6 D6
 p5 D5
 p4 D4
 p3 BL 1. Backlight on                        0. Backlight off
 p2 EN An enable signal for writing or reading data. 
 p1 RW 1. Read mode                           0. Write mode
 p0 RS 1. Data Register (for read and write)  0. Instruction Register (for write), Busy flag-Address Counter (for read)
 
 Write nibble
 ============
 Set E=1, BL, RS, RW, D
 pause 150nS set up time
 Set E=0, BL, RS, RW, D
 pause 20nS hold time
 
 Initialise
 ==========
 Wait 30mS
 Send RS=0 (instruction register); RW=0 (write); D7-4=0x3 (function set 8 bit)
 pause 40mS
 Send RS=0 (instruction register); RW=0 (write); D7-4=0x3 (function set 8 bit)
 pause 5mS
 Send RS=0 (instruction register); RW=0 (write); D7-4=0x3 (function set 8 bit)
 pause 100uS
 Send RS=0 (instruction register); RW=0 (write); D7-4=0x2 (function set 4 bit)
 pause 100uS
 Send RS=0 (instruction register); RW=0 (write); D7-4=0x2 (function set 4 bit)
 Send RS=0 (instruction register); RW=0 (write); D3-0=0xC (function set 2 line, 5x10 dots)
 pause 100uS
 Send RS=0 (instruction register); RW=0 (write); D7-4=0x0
 Send RS=0 (instruction register); RW=0 (write); D3-0=0xC (display control - display on)
 pause 100uS
 Send RS=0 (instruction register); RW=0 (write); D7-4=0x0
 Send RS=0 (instruction register); RW=0 (write); D3-0=0x1 (display clear)
 pause 100uS
 
 For memory map see https://web.alfredstate.edu/faculty/weimandn/lcd/lcd_addressing/lcd_addressing_index.html
*/


#define LCD_LINE_LENGTH 16

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_DISPLAYCONTROL 0x08
#define LCD_FUNCTIONSET 0x20

// flags for display on/off control
#define LCD_DISPLAYON 0x04

// flag for entry mode
#define LCD_ENTRYLEFT 0x02

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_2LINE 0x08
#define LCD_5x10DOTS 0x04

static uint8_t data[2]; //This must be static as it is used in the main routine
static int _result = I2C_RESULT_SUCCESS;


static void sendToInterface(uint8_t nibble, char EN, char RS, char RW)
{
    char BL = LcdIsOn();
    uint8_t byte = (uint8_t)(nibble << 4);
    if (RS) byte |= 1;
    if (RW) byte |= 2;
    if (EN) byte |= 4;
    if (BL) byte |= 8;
    I2CSend(_i2cAddr, 1, &byte, &_result);
}

static void sendNibble(uint8_t nibble, char RS, char RW)
{
    sendToInterface(nibble, 1, RS, RW); //Set E to latch RW and RS
    sendToInterface(nibble, 0, RS, RW); //Reset E to latch the nibble
}

static void command8bitMode(uint8_t cmd)
{
    sendNibble(cmd >> 4, 0, 0);
}

static void command(uint8_t byte)
{
    sendNibble(byte >> 4,  0, 0); //Send the most  significant nibble
    sendNibble(byte & 0xF, 0, 0); //Send the least significant nibble
}
static void write(uint8_t byte)
{
    sendNibble(byte >> 4,  1, 0); //Send the most  significant nibble
    sendNibble(byte & 0xF, 1, 0); //Send the least significant nibble
}

#define TODO_INIT_START         0
#define TODO_INIT_FUNCTION_S_0  1
#define TODO_INIT_FUNCTION_S_1  2
#define TODO_INIT_FUNCTION_S_2  3
#define TODO_INIT_FUNCTION_S_3  4
#define TODO_INIT_FUNCTION      5
#define TODO_INIT_CONTROL       6
#define TODO_INIT_CLEAR         7

#define TODO_SEND_START        10
#define TODO_SEND_CURSOR0      11
#define TODO_SEND_TEXT0        12
#define TODO_SEND_CURSOR1      13
#define TODO_SEND_TEXT1        14

#define TODO_TURN_OFF          20
#define TODO_NOTHING_OFF       21
#define TODO_NOTHING_ON        22

static int _todo = TODO_INIT_START;

static uint32_t _MsTimerWaitAfter = 0;
static uint32_t _waitAfterMs      = 0;

static const char* _pText0 = 0;
static const char* _pText1 = 0;
static int i = 0;
static bool hadEnd = false;

void LcdSendText(const char* pText0, const char* pText1)
{
    _pText0 = pText0;
    _pText1 = pText1;
    _todo = TODO_SEND_START;
}
void LcdTurnOn()
{
    _todo = TODO_NOTHING_ON;
}
void LcdTurnOff()
{
    _todo = TODO_TURN_OFF;
}

bool LcdIsReady()
{
    return _todo == TODO_NOTHING_ON;
}
bool LcdIsOn()
{
    return _todo != TODO_NOTHING_OFF && _todo != TODO_TURN_OFF;
}

void LcdInit(uint8_t i2cAddr)
{
    _i2cAddr = i2cAddr;
}
void LcdMain()
{
    if (!MsTimerRelative(_MsTimerWaitAfter, _waitAfterMs)) return;
    _waitAfterMs = 0; //Don't wait unless specified below
    _MsTimerWaitAfter = MsTimerCount;
    switch (_todo)
    {
        case TODO_INIT_START:
            _todo = TODO_INIT_FUNCTION_S_0;
            _waitAfterMs = 5;
            break;
        case TODO_INIT_FUNCTION_S_0:
            command8bitMode(LCD_FUNCTIONSET | LCD_8BITMODE); //8 bit mode
            _todo = TODO_INIT_FUNCTION_S_1;
            _waitAfterMs = 50;
            break;
        case TODO_INIT_FUNCTION_S_1:
            command8bitMode(LCD_FUNCTIONSET | LCD_8BITMODE); //8 bit mode
            _todo = TODO_INIT_FUNCTION_S_2;
            _waitAfterMs = 5;
            break;
        case TODO_INIT_FUNCTION_S_2:
            command8bitMode(LCD_FUNCTIONSET | LCD_8BITMODE); //8 bit mode
            _todo = TODO_INIT_FUNCTION_S_3;
            _waitAfterMs = 5;
            break;
        case TODO_INIT_FUNCTION_S_3:
            command8bitMode(LCD_FUNCTIONSET); //4 bit mode
            _todo = TODO_INIT_FUNCTION;
            _waitAfterMs = 2;
            break;
        case TODO_INIT_FUNCTION:
            command(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x10DOTS); //2 line
            _todo = TODO_INIT_CONTROL;
            _waitAfterMs = 2;
            break;
        case TODO_INIT_CONTROL:
            command(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
            _todo = TODO_INIT_CLEAR;
            _waitAfterMs = 2;
            break;
        case TODO_INIT_CLEAR:
            command(LCD_CLEARDISPLAY);
            _todo = TODO_NOTHING_ON;
            _waitAfterMs = 15;
            break;
        case TODO_SEND_START:
            _todo = TODO_SEND_CURSOR0;
            break;
        case TODO_SEND_CURSOR0:
            command(0x80); //Set DDRAM address to 0
            _todo = TODO_SEND_TEXT0;
            i = 0;
            hadEnd = false;
            _waitAfterMs = 10;
            break;
        case TODO_SEND_TEXT0:
            if (i < LCD_LINE_LENGTH)
            {
                if (!_pText0 || !_pText0[i]) hadEnd = true;
                if (!hadEnd) write(_pText0[i]);
                else         write(' ');
                i++;
            }
            else
            {
                _todo = TODO_SEND_CURSOR1;
            }
            _waitAfterMs = 5;
            break;
        case TODO_SEND_CURSOR1:
            command(0xc0); //Set DDRAM address to 64
            _todo = TODO_SEND_TEXT1;
            i = 0;
            hadEnd = false;
            _waitAfterMs = 10;
            break;
        case TODO_SEND_TEXT1:
            if (i < LCD_LINE_LENGTH)
            {
                if (!_pText1 || !_pText1[i]) hadEnd = true;
                if (!hadEnd) write(_pText1[i]);
                else         write(' ');
                i++;
            }
            else
            {
                _todo = TODO_NOTHING_ON;
            }
            _waitAfterMs = 5;
            break;
        case TODO_TURN_OFF:
            command(LCD_CLEARDISPLAY);
            _todo = TODO_NOTHING_OFF;
            break;
        case TODO_NOTHING_OFF:
            break;
        case TODO_NOTHING_ON:
            break;
    }
}

