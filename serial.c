/*
 * File:   serial.c
 * Author: Thomas M. Grubb
 *
 * Functions that action the receipt and transmission of data
 * via serial communications (I2C, SPI etc.) are found here.
 * 
 * Created on July 9, 2018, 10:37 AM
 */


#include <xc.h>
#include "global.h"

void I2C1_Write_EEPROM(char devAddr, char memValue, char value)
{
    char wrDevAddr = devAddr;       // format for write command
    
    SSP1CON1bits.SSPEN = 0x1;       // ENABLE I2C Module  
    __delay_us(5);
    
    SSP1CON2bits.SEN = 0x1;         // send 'START' bit
    while(SSP1CON2bits.SEN);        // wait here
    PIR1bits.SSP1IF = 0x0;          // clear flag
    SSP1BUF = wrDevAddr;            // send device address
    
    while (SSP1CON2bits.ACKSTAT);   // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    PIR1bits.SSP1IF = 0x0;          // clear flag
    SSP1BUF = memValue;             // send memory address
    
    while (SSP1CON2bits.ACKSTAT);   // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    PIR1bits.SSP1IF = 0x0;          // clear flag
    SSP1BUF = value;                // send data
    
    while (SSP1CON2bits.ACKSTAT);   // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    SSP1CON2bits.PEN = 0x1;         // send 'STOP' bit
    while(SSP1CON2bits.PEN);        // wait here
    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON1bits.SSPEN = 0x1;       // DISABLE I2C Module 
}

char I2C1_Read_EEPROM(char devAddr, char memValue)
{
    char dummyWrite = devAddr;
    char rdAddr = (devAddr|0x01);
    char value = 0x0;
    
    SSP1CON1bits.SSPEN = 0x1;       // ENABLE I2C Module  
    __delay_us(5);
    
    SSP1CON2bits.SEN = 0x1;         // send 'START' bit
    while(SSP1CON2bits.SEN);        // wait for 'START' to send
    while(!PIR1bits.SSP1IF);        // wait here
    PIR1bits.SSP1IF = 0x0;          // clear flag
    SSP1BUF = dummyWrite;           // send dummy write
    
    if(SSP1CON2bits.ACKSTAT);       // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    PIR1bits.SSP1IF = 0x0;          // clear flag
    SSP1BUF = memValue;             // send memory address
    
    while(SSP1CON2bits.ACKSTAT);    // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here 
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON2bits.RSEN = 0x1;        // send 'START' bit (repeated)
    
    while(SSP1CON2bits.RSEN);       // wait for 'START' to send
    while(!PIR1bits.SSP1IF);        // wait here
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1BUF = rdAddr;               // send read command
    
    while(SSP1CON2bits.ACKSTAT);    // wait for ACK    
    while(!PIR1bits.SSP1IF);        // wait here
    PIR1bits.SSP1IF = 0x0;          // reset flag
    
    SSP1CON2bits.RCEN = 0x1;        // enable receipt
    
    while(!SSP1STATbits.BF);        // wait for data
    while (!PIR1bits.SSP1IF);       // wait here
    
    value = SSP1BUF;
    
    SSP1CON2bits.ACKDT = 0x1;       // set bit for NACK
    SSP1CON2bits.ACKEN = 0x1;       // send NACK
    
    while(SSP1CON2bits.ACKEN);      // wait here    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON2bits.PEN = 0x1;         // send 'STOP' bit
    while(SSP1CON2bits.PEN);        // wait here
    
    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON1bits.SSPEN = 0x1;       // DISABLE I2C Module 
    
    return value;                   // return data
}

void I2C1_Write_DigiPot(char value)
{
    char wrCommand = (DigiPotAddr << 1);
    
    SSP1CON1bits.SSPEN = 0x1;       // ENABLE I2C Module  
    __delay_us(5);
    
    SSP1CON2bits.SEN = 0x1;         // send 'START' bit
    while(SSP1CON2bits.SEN);        // wait here
    PIR1bits.SSP1IF = 0x0;          // clear flag
    SSP1BUF = wrCommand;            // send device address
    
    while (SSP1CON2bits.ACKSTAT);   // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    PIR1bits.SSP1IF = 0x0;          // clear flag
    SSP1BUF = value;                // send data
    
    while (SSP1CON2bits.ACKSTAT);   // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    SSP1CON2bits.PEN = 0x1;         // send 'STOP' bit
    while(SSP1CON2bits.PEN);        // wait here
    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON1bits.SSPEN = 0x1;       // DISABLE I2C Module 
}

void sendParam(void)
{
    dataTarget = SSP2BUF;       // read address from buffer
    while(SSP2STATbits.BF);     // wait until buffer clear

    // set pointer to required data
    if (dataTarget == 0xA1)
    {
        dataPtr = feedbackValue;
    }
    else if (dataTarget == 0xA5)
    {
        dataPtr = levelValue;
    }
    else if (dataTarget == 0xA9)
    {
        dataPtr = timeValue;
    }

    SSP2BUF = dataPtr;          // load value into buffer
    dataTarget ^= 1;            // toggle target data
    SSP2CON1bits.CKP = 0x1;     // release clock
    PIR2bits.SSP2IF = 0x0;      // ensure SSP2 flag is reset
}
