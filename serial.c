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
#include <stdint.h>
#include "global.h"

#define MAX 12

int paramAddress[] =
{
    0xA1, 0xA3, 0xA5, 0xA7, 0xA9, 0xAB, 0xAD, 0xAF,
    0xB1, 0xB3, 0xB5, 0xB7, 0xB9, 0xBB, 0xBD, 0xBF
};

int presetAdd[] = 
{
    0x00,
    0x10,
    0x20,
    0x30,
    0x40
};

void I2C1_Write_EEPROM(char memValue, char value)
{
    char wrDevAddr = EEPROM_ADD;    // format for write command
    
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
    
    SSP1CON2bits.PEN = 0x1;         // send 'STOP' bit
    
    while (SSP1CON2bits.ACKSTAT);   // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    SSP1CON2bits.PEN = 0x1;         // send 'STOP' bit
    while(SSP1CON2bits.PEN);        // wait here
    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON1bits.SSPEN = 0x1;       // DISABLE I2C Module 
}

void I2C1_Page_Write_EEPROM(int slot, int *data[], int bytes)
{
    char wrDevAddr = EEPROM_ADD;
    char memValue = presetAdd[slot];
    int n = 0;
    
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
    
    for (n = 0; n < bytes; n++)
    {
        while (SSP1CON2bits.ACKSTAT);   // wait for ACK
        while(!PIR1bits.SSP1IF);        // wait here
        PIR1bits.SSP1IF = 0x0;          // clear flag
        SSP1BUF = data[n];              // send data
    }
    
    while (SSP1CON2bits.ACKSTAT);   // wait for ACK
    while(!PIR1bits.SSP1IF);        // wait here
    SSP1CON2bits.PEN = 0x1;         // send 'STOP' bit
    while(SSP1CON2bits.PEN);        // wait here
    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON1bits.SSPEN = 0x1;       // DISABLE I2C Module 
}

void I2C1_Block_Read_EERPOM(int slot, int *data[], int bytes)
{
    char dummyWrite = EEPROM_ADD;
    char rdAddr = (dummyWrite|0x01);
    char memValue = presetAdd[slot];
    int n = 0;  
    
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
    data[n] = SSP1BUF;  
    
    for (n = 1; n < bytes; n++)
    {
        SSP1CON2bits.ACKDT = 0x0;       // set bit for ACK
        SSP1CON2bits.ACKEN = 0x1;       // send ACK
        while(SSP1CON2bits.ACKEN);      // wait here
        PIR1bits.SSP1IF = 0x0;          // reset flag
        SSP1CON2bits.RCEN = 0x1;        // enable receipt
        
        while(!SSP1STATbits.BF);        // wait for data
        while (!PIR1bits.SSP1IF);       // wait here
        data[n] = SSP1BUF;               
    }
    
    SSP1CON2bits.ACKDT = 0x1;       // set bit for NACK
    SSP1CON2bits.ACKEN = 0x1;       // send NACK
    
    while(SSP1CON2bits.ACKEN);      // wait here    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON2bits.PEN = 0x1;         // send 'STOP' bit
    while(SSP1CON2bits.PEN);        // wait here
    
    
    PIR1bits.SSP1IF = 0x0;          // reset flag
    SSP1CON1bits.SSPEN = 0x1;       // DISABLE I2C Module  
}

void presetCtrl(int slot)
{
    char updateValue;
    char bitSet = 2^slot;
    
    updateValue = (slotUsed | bitSet);
    
    I2C1_Write_EEPROM(0x40, updateValue);
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
    int n = 0;
    
    dataTarget = SSP2BUF;       // read address from buffer
 
    for (n = 0; n < 15; n++)
    {
        if (dataTarget == paramAddress[n])
        {
            if (n == 15)
            {
                dataPtr = setupComplete;
            }
            else dataPtr = parameter[n];
            break;
        }
    }
       
    SSP2BUF = dataPtr;          // load value into buffer
    SSP2CON1bits.CKP = 0x1;     // release clock
    PIR2bits.SSP2IF = 0x0;      // ensure SSP2 flag is reset
}
