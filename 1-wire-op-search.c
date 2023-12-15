
#include "1-wire-interface.h"
#include "1-wire-roms.h"

//Search state
static int LastDiscrepancy;
static int LastFamilyDiscrepancy;
static char LastDeviceFlag;
static int romCount;
static unsigned char crc8;

//Inner loop variables
static uint8_t Rom[8];
static int id_bit_number;
static int last_zero;
static int rom_byte_number;
static int search_result;
static char busy;
static char id_bit; //was int
static char cmp_id_bit; //was int
static char search_direction; //was unsigned char
static unsigned char rom_byte_mask;

enum todos
{
    TODO_FIRST,
    TODO_NEXT,
    TODO_RESET_CMD,
    TODO_RESET_WAIT,
    TODO_SEARCH_CMD,
    TODO_SEARCH_WAIT,
    TODO_LOOP_START,
    TODO_TRIPLET_CMD,
    TODO_TRIPLET_STATUS,
    TODO_LOOP_END,
};
static enum todos _todo;
void OneWireOpSearchInit()
{
     _todo = 0;
}
int OneWireOpSearch() //returns 0 to continue, -1 on error, +1 if finished
{
    uint8_t status = 0;
    switch (_todo)
    {
        default:
        case TODO_FIRST:
            // reset the search state
            LastDiscrepancy = 0;
            LastDeviceFlag = 0;
            LastFamilyDiscrepancy = 0;
            romCount = 0;
            _todo++;
            return 0;
        case TODO_NEXT:
            // leave the search state alone
            // initialize for search
            id_bit_number = 1;
            last_zero = 0;
            rom_byte_number = 0;
            rom_byte_mask = 1;
            search_result = 0;
            crc8 = 0;
            _todo++;
            return 0;
        case TODO_RESET_CMD:           OneWireInterfaceResetBus      ()    ; _todo++; return 0;
        case TODO_RESET_WAIT:     if (!OneWireInterfaceGetBusIsBusy  ()    ) _todo++; return 0;
        case TODO_SEARCH_CMD:          OneWireInterfaceWriteByteToBus(0xF0); _todo++; return 0;//Send SEARCH ROM 0x33
        case TODO_SEARCH_WAIT:    if (!OneWireInterfaceGetBusIsBusy  ()    ) _todo++; return 0;
        case TODO_LOOP_START:
            // if this discrepancy is before the Last Discrepancy
            // on a previous next then pick the same as last time
            if (id_bit_number < LastDiscrepancy)
            {
               if ((Rom[rom_byte_number] & rom_byte_mask) > 0) search_direction = 1;
               else                                            search_direction = 0;
            }
            else
            {
               // if equal to last pick 1, if not then pick 0
               if (id_bit_number == LastDiscrepancy) search_direction = 1;
               else                                  search_direction = 0;
            }
            _todo++;
            return 0;
        case TODO_TRIPLET_CMD:
            // Perform a triple operation on the DS2482 which will perform 2 read bits and 1 write bit
            OneWireInterfaceGenerateTripletOnBus(search_direction);
            _todo++;
            return 0;
        case TODO_TRIPLET_STATUS:
            //Handle the triplet result
            OneWireInterfaceGetTripletResult(&busy, &id_bit, &cmp_id_bit, &search_direction);
            
            //Return if still busy
            if (busy) return 0;
            
            // check for no devices on 1-Wire
            if (id_bit && cmp_id_bit)
            {
                _todo = TODO_LOOP_END;
                return 0;
            }
            
            if (!id_bit && !cmp_id_bit && !search_direction)
            {
               last_zero = id_bit_number;

               // check for Last discrepancy in family
               if (last_zero < 9) LastFamilyDiscrepancy = last_zero;
            }

            // set or clear the bit in the ROM byte rom_byte_number with mask rom_byte_mask
            if (search_direction == 1) Rom[rom_byte_number] |=  rom_byte_mask;
            else                       Rom[rom_byte_number] &= ~rom_byte_mask;

            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum byte rom_byte_number
            // and reset mask
            if (rom_byte_mask == 0)
            {
               //calc_crc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
               rom_byte_number++;
               rom_byte_mask = 1;
            }
               
            if (rom_byte_number < 8) _todo = TODO_LOOP_START;
            else                     _todo = TODO_LOOP_END;
            return 0;
        case TODO_LOOP_END:
            //store the rom
            for (int i = 0; i < 8; i++)
            {
                OneWireRom[romCount] <<= 8;
                OneWireRom[romCount] |= Rom[i];
            }
            LastDiscrepancy = last_zero;
            if (LastDiscrepancy == 0) LastDeviceFlag = 1;
            romCount++;
            if (romCount >= MAX_ROMS || LastDeviceFlag)
            {
                for (int i = romCount; i < MAX_ROMS; i++) OneWireRom[i] = 0;
                _todo++;
                return 1;
            }
            else
            {
                _todo = TODO_NEXT;
                return 0;
            }
    }
}
