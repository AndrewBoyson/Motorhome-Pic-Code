
#include <xc.h>
#include <stdint.h>

// ECAN bitrate define, choose only ONE rate
#define F_ECAN_100    1       // 1 sets ECAN module for 100kbps
#define F_ECAN_125    0       // 1 sets ECAN module for 125kbps
#define F_ECAN_500    0       // 1 sets ECAN module for 500kbps
#define F_ECAN_1000   0       // 1 sets ECAN module for 1000kbps

unsigned char temp_EIDH;    //Extended Identifier, high byte
unsigned char temp_EIDL;    //Extended Identifier, low byte
unsigned char temp_SIDH;    //Standard Identifier, high byte
unsigned char temp_SIDL;    //Standard Identifier, low byte
unsigned char temp_DLC;     //Data Length Control value, 0 to 8
unsigned char temp_D0;      //Data byte 0 through...
unsigned char temp_D1;
unsigned char temp_D2;
unsigned char temp_D3;
unsigned char temp_D4;
unsigned char temp_D5;
unsigned char temp_D6;
unsigned char temp_D7;      //Data byte 7


void CanInterfaceOn()
{
    LATBbits.LB4 = 0;
}
void CanInterfaceStandby()
{
    LATBbits.LB4 = 1;
}

void CanInit()
{
    //MCP2551 RS pin: high is standby, low is high speed)
    TRISBbits.TRISB4 = 0; //RB4 (pin 25)
    LATBbits.LB4 = 0;    //Turn on CAN
    
    // Place CAN module in configuration mode, see CANCON register data
    CANCON = 0x80;    //REQOP bits <2:0> = 0b100
    while(!(CANSTATbits.OPMODE ==0x04));    //Wait for op-mode bits in the
                                            //CANSTAT register to = 0b100
                                            //to indicate config mode OK

    // Enter CAN module into Mode 0, standard legacy mode; see ECANCON register
    ECANCON = 0x00;
    
    //Go for max oscillator tolerance
    //SyncSeg = 1TQ
    //PropSeg = 1TQ
    //PS1     = 4TQ
    //PS2     = 4TQ
    //SJWmax  = 4TQ
    BRGCON1 = 0xC3; //11, 000011     //SJW=4TQ, BRP=divide by 8
    BRGCON2 = 0x98; //1, 0, 011, 000 //SEG2PHTS=freely programmable, SAM=sampled once,  PS1=4TQ,  PRSEG=1TQ
    BRGCON3 = 0x03; //0000 0011      //PS2  4TQ
    
    //CIOCON =  0x71; // 0, 1, 1, 1, 000, 1: TX2SRC = 0; TX2EN = 1; ENDRHI = 1; CANCAP=1; CLKSEL=1
        
    // Initialize Receive Masks, see registers RXMxEIDH, RXMxEIDL, etc...
    // Mask 0 (M0) will accept NO extended addresses, but any standard address
    RXM0EIDH = 0x00;    // Extended Address receive acceptance mask, high byte 
    RXM0EIDL = 0x00;    // Extended Address receive acceptance mask, low byte
    //RXM0SIDH = 0xFF;    // Standard Address receive acceptance mask, high byte
    //RXM0SIDL = 0xE0;    // Standard Address receive acceptance mask, low byte
    RXM0SIDH = 0x00;    // Standard Address receive acceptance mask, high byte
    RXM0SIDL = 0x00;    // Standard Address receive acceptance mask, low byte
    
    // Mask 1 (M1) will accept NO extended addresses, but any standard address
    RXM1EIDH = 0x00;    // Extended Address receive acceptance mask, high byte    
    RXM1EIDL = 0x00;    // Extended Address receive acceptance mask, low byte
    //RXM1SIDH = 0xFF;    // Standard Address receive acceptance mask, high byte
    //RXM1SIDL = 0xE0;    // Standard Address receive acceptance mask, low byte
    RXM1SIDH = 0x00;    // Standard Address receive acceptance mask, high byte
    RXM1SIDL = 0x00;    // Standard Address receive acceptance mask, low byte
    
    // Mode 0 allows use of receiver filters RXF0 through RXF5. Enable filters
    // RXF0 and RXF1, all others disabled. See register RXFCONn.
    //  Only using two filters
    //RXFCON0 = 0x03;     //Enable Filter-0 and Filter-1; disable others 
    RXFCON0 = 0x00;     //Enable Filter-0 and Filter-1; disable others 
    RXFCON1 = 0x00;     //Disable Filters 8 through 15
    
    // Initialize Receive Filters
    //  Filter 0 = 0x32C0
    //  Filter 1 = 0x33C0
   
    RXF0EIDH = 0x00;    //Extended Address Filter-0 unused, set high byte to 0
    RXF0EIDL = 0x00;    //Extended Address Filter-0 unused, set low byte to 0
    //RXF0SIDH = 0x32;    //Standard Address Filter-0 high byte set to 0x32
    //RXF0SIDL = 0xC0;    //Standard Address Filter-0 low byte set to 0xC0

    RXF2EIDH = 0x00;    //Extended Address Filter-0 unused, set high byte to 0
    RXF2EIDL = 0x00;    //Extended Address Filter-0 unused, set low byte to 0
    //RXF2SIDH = 0x33;    //Standard Address Filter-0 high byte set to 0x33
    //RXF2SIDL = 0xC0;    //Standard Address Filter-0 low byte set to 0xC0
    
    // After configuring CAN module with above settings, return it
    // to Normal mode
    CANCON = 0x00;
    while(CANSTATbits.OPMODE==0x00);        //Wait for op-mode bits in the
                                            //CANSTAT register to = 0b000
                                            //to indicate Normal mode OK
    
    // Set Receiving Modes for receiver buffers 0 and 1
    RXB0CON = 0x00;     // See register RXB0CON
    RXB1CON = 0x00;     // See register RXB1CON
}
void (*CanReceive)(uint16_t id, uint8_t length, void* pData);

char CanTransmit(uint16_t id, uint8_t length, void *pData)
{
    unsigned char *p = (uint8_t*)pData;
    
    if (!TXB0CONbits.TXREQ)
    {
        TXB0SIDH = (uint8_t)(id >> 3);
        TXB0SIDL = (uint8_t)(id << 5);
        TXB0EIDH = 0x00;
        TXB0EIDL = 0x00;

        TXB0DLC = length;
        if (length > 0) TXB0D0 = *p++;
        if (length > 1) TXB0D1 = *p++;
        if (length > 2) TXB0D2 = *p++;
        if (length > 3) TXB0D3 = *p++;
        if (length > 4) TXB0D4 = *p++;
        if (length > 5) TXB0D5 = *p++;
        if (length > 6) TXB0D6 = *p++;
        if (length > 7) TXB0D7 = *p++;
    
        TXB0CONbits.TXREQ = 1; //Set the buffer to transmit
        
        return 0; //Message queued successfully
    }
    if (!TXB1CONbits.TXREQ)
    {
        TXB1SIDH = (uint8_t)(id >> 3);
        TXB1SIDL = (uint8_t)(id << 5);
        TXB1EIDH = 0x00;
        TXB1EIDL = 0x00;

        TXB1DLC = length;
        if (length > 0) TXB1D0 = *p++;
        if (length > 1) TXB1D1 = *p++;
        if (length > 2) TXB1D2 = *p++;
        if (length > 3) TXB1D3 = *p++;
        if (length > 4) TXB1D4 = *p++;
        if (length > 5) TXB1D5 = *p++;
        if (length > 6) TXB1D6 = *p++;
        if (length > 7) TXB1D7 = *p++;
    
        TXB1CONbits.TXREQ = 1; //Set the buffer to transmit
        
        return 0; //Message queued successfully
    }
    if (!TXB2CONbits.TXREQ)
    {
        TXB2SIDH = (uint8_t)(id >> 3);
        TXB2SIDL = (uint8_t)(id << 5);
        TXB2EIDH = 0x00;
        TXB2EIDL = 0x00;

        TXB2DLC = length;
        if (length > 0) TXB2D0 = *p++;
        if (length > 1) TXB2D1 = *p++;
        if (length > 2) TXB2D2 = *p++;
        if (length > 3) TXB2D3 = *p++;
        if (length > 4) TXB2D4 = *p++;
        if (length > 5) TXB2D5 = *p++;
        if (length > 6) TXB2D6 = *p++;
        if (length > 7) TXB2D7 = *p++;
    
        TXB2CONbits.TXREQ = 1; //Set the buffer to transmit
        
        return 0; //Message queued successfully
    }
    return 1;
}

static void receive(void)
{   
    uint16_t  id;
    uint8_t  length;
    uint8_t data[8];
    uint8_t* p = data;
    if (RXB0CONbits.RXFUL)
    {
        id  = (uint16_t)RXB0SIDH << 3;
        id |= (uint16_t)RXB0SIDL >> 5;
        length  = RXB0DLC;
        *p++ = RXB0D0;
        *p++ = RXB0D1;
        *p++ = RXB0D2;
        *p++ = RXB0D3;
        *p++ = RXB0D4;
        *p++ = RXB0D5;
        *p++ = RXB0D6;
        *p++ = RXB0D7;
        RXB0CONbits.RXFUL = 0;
        CanReceive(id, length, data);
    }
    if (RXB1CONbits.RXFUL)
    {
        id  = (uint16_t)RXB1SIDH << 3;
        id |= (uint16_t)RXB1SIDL >> 5;
        length  = RXB1DLC;
        *p++ = RXB1D0;
        *p++ = RXB1D1;
        *p++ = RXB1D2;
        *p++ = RXB1D3;
        *p++ = RXB1D4;
        *p++ = RXB1D5;
        *p++ = RXB1D6;
        *p++ = RXB1D7;
        RXB1CONbits.RXFUL = 0;
        CanReceive(id, length, data);
    }
}
void CanMain()
{
    receive();
}