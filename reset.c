#include <xc.h>

char ResetConfigMismatch = 0;
char ResetInstruction = 0;
char ResetWatchdog = 0;
char ResetPowerOn  = 0;
char ResetBrownOut = 0;
char ResetStackFull = 0;
char ResetStackUnderflow = 0;
char ResetMclr = 0;

void ResetInit()
{
    //See table 5-3
    if (!RCONbits.nCM ) ResetConfigMismatch = 1;
    if (!RCONbits.nRI ) ResetInstruction = 1;
    if (!RCONbits.nTO ) ResetWatchdog = 1;
    if (!RCONbits.nBOR)
    {
        if (!RCONbits.nPOR) ResetPowerOn = 1;
        else                ResetBrownOut = 1;
    }
    
    if (STKFUL) ResetStackFull      = 1;
    if (STKUNF) ResetStackUnderflow = 1;
    
    if (RCONbits.nCM  &&
        RCONbits.nRI  &&
        RCONbits.nTO  &&
        RCONbits.nBOR && 
        RCONbits.nPOR &&
        !STKFUL       &&
        !STKUNF          )  ResetMclr = 1;
    
    RCONbits.nCM  = 1;
    RCONbits.nRI  = 1;
    RCONbits.nTO  = 1;
    RCONbits.nPD  = 1;
    RCONbits.nBOR = 1;
    RCONbits.nPOR = 1;
    STKFUL        = 0;
    STKUNF        = 0;
}
