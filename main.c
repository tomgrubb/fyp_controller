  /* ========================================================================
  *                        PITCH SHIFTER / DELAY
  *          
  *                            USER INTERFACE   
  * 
  *                          FINAL YEAR PROJECT
  *                          MONASH UNIVERSITY
  *                                2018
  * 
  *      File:   main.c
  *      Author: Thomas M. Grubb
  * 
  *                      (c) Sieger Audio 2018
  *
  * ------------------------- [CHANGE LOG] ---------------------------------
  *                  
  *     05/03/2018 - Initial configuration of ADC module.
  *     11/05/2018 - ADC functioning for manually triggered readings.
  *                - Initial configuration of PWM module; tests OK.
  *     13/05/2018 - Added switch debouncing and reading; tests OK.
  *                - Configure and test clock switching w/ PLL (32 MHz). 
  *     14/05/2018 - Ported Tap-Tempo code from previous project, need to
  *                  fix time out function for max delay time.   
  *     03/07/2018 - Configure and test I2C Slave operation.
  *                - Masking for 7-bit addressing successful and efficient.           
  *                - Communication link can send multiple parameters.
  *     08/07/2018 - Configure second I2C module for Master operation.
  *                - Organize functions into multiple .c files.   
  *     15/07/2018 - Write drivers for EEPROM I2C communication (read/write).
  *                - Test at 100 kHz Clock; tests OK.
  *     22/08/2018 - Configuration and test of all hardware I/O.
  *                - Define macros for hardware.
  *     24/09/2018 - ADC reconfigured for 12 bit resolution (scaled down to 8 except for time)
  *                - Factory settings added for default preset bank
  *                - Setup complete check bit added to signal system is ready for operation
  *     03/10/2018 - Data collection and streaming for all nine delay parameters
  *                - Changed masking and parameterized slave I2C transmissions               
  *       
  * ------------------------------------------------------------------------
  * 
  *     Created on March 5th, 2018, 5:14 PM
  * 
  *     Last edited on October 3rd, 2018, 12:49 PM
  * 
  * ======================================================================== */


// CONFIG1L
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset (Disabled)
#pragma config XINST = OFF      // Extended Instruction Set (Disabled)

// CONFIG1H
#pragma config BOREN = OFF      // Brown-Out Reset Disable (Controlled with SBOREN bit, disabled in Deep Sleep)
#pragma config BORV = 1         // Brown-out Reset Voltage (1.8V)
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)

// CONFIG2L
#pragma config FOSC = FRCDIV    // Oscillator (Fast RC Oscillator w/ PLL (FRCPLL))
#pragma config SOSCSEL = DIG    // T1OSC/SOSC Power Selection Bits (Low Power T1OSC/SOSC circuit selected)
#pragma config CLKOEN = OFF     // Clock Out Enable Bit (CLKO output enabled on the RA6 pin)
#pragma config IESO = OFF       // Internal External Oscillator Switch Over Mode (Disabled)

// CONFIG2H
#pragma config PLLDIV = PLL8X   // PLL Frequency Multiplier Select bits (No PLL used - PLLGO bit not available to user)

// CONFIG3L
#pragma config POSCMD = NONE    // Primary Oscillator Select (Primary oscillator disabled)
#pragma config FSCM = CSECMD    // Clock Switching and Monitor Selection Configuration bits (Clock switching is enabled, fail safe clock monitor is disabled)

// CONFIG3H

// CONFIG4L
#pragma config WPFP = WPFP255   // Write/Erase Protect Page Start/End Boundary (Write Protect Program Flash Page 255)

// CONFIG4H
#pragma config WPDIS = WPDIS    // Segment Write Protection Disable (Disabled)
#pragma config WPEND = WPENDMEM // Segment Write Protection End Page Select (Write Protect from WPFP to the last page of memory)
#pragma config WPCFG = WPCFGDIS // Write Protect Configuration Page Select (Disabled)

// CONFIG5L
#pragma config T5GSEL = T5G     // TMR5 Gate Select bit (TMR5 Gate is driven by the T5G input)
#pragma config CINASEL = DEFAULT// CxINA Gate Select bit (C1INA and C3INA are on their default pin locations)
#pragma config EASHFT = ON      // External Address Shift bit (Address Shifting enabled)
#pragma config ABW = XM16       // Address Bus Width Select bits (16-bit address bus)
#pragma config BW = 16          // Data Bus Width (16-bit external bus mode)
#pragma config WAIT = OFF       // External Bus Wait (Disabled)

// CONFIG5H
#pragma config IOL1WAY = ON     // IOLOCK One-Way Set Enable bit (Once set, the IOLOCK bit cannot be cleared)
#pragma config LS48MHZ = SYSX2  // USB Low Speed Clock Select bit (Divide-by-2 (System clock must be 12 MHz))
#pragma config MSSPMSK2 = MSK7  // MSSP2 7-Bit Address Masking Mode Enable bit (7 Bit address masking mode)
#pragma config MSSPMSK1 = MSK7  // MSSP1 7-Bit Address Masking Mode Enable bit (7 Bit address masking mode)

// CONFIG6L
#pragma config WDTWIN = PS25_0  // Watch Dog Timer Window (Watch Dog Timer Window Width is 25 percent)
#pragma config WDTCLK = FRC     // Watch Dog Timer Clock Source (Use FRC when WINDIS = 0, system clock is not INTOSC/LPRC and device is not in Sleep; otherwise, use INTOSC/LPRC)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale (1:32768)

// CONFIG6H
#pragma config WDTEN = OFF      // Watchdog Timer Enable (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WINDIS = WDTSTD  // Windowed Watchdog Timer Disable (Standard WDT selected; windowed WDT disabled)
#pragma config WPSA = 128       // WDT Prescaler (WDT prescaler ratio of 1:128)

// CONFIG7L
#pragma config RETEN = OFF      // Retention Voltage Regulator Control Enable (Retention not available)
#pragma config VBTBOR = OFF     // VBAT BOR Enable (VBAT BOR is disabled)
#pragma config DSBOREN = OFF    // Deep Sleep BOR Enable (BOR disabled in Deep Sleep)
#pragma config DSBITEN = OFF    // DSEN Bit Enable bit (Deep Sleep is controlled by the register bit DSEN)

// CONFIG7H

// CONFIG8L
#pragma config DSWDTPS = DSWDTPS1F// Deep Sleep Watchdog Timer Postscale Select (1:68719476736 (25.7 Days))

// CONFIG8H
#pragma config DSWDTEN = OFF    // Deep Sleep Watchdog Timer Enable (DSWDT Disabled)
#pragma config DSWDTOSC = LPRC  // DSWDT Reference Clock Select (DSWDT uses LPRC as reference clock)

// INCLUDED HEADERS
#include <xc.h>
#include <pic18f85j94.h>
#include "global.h"
#include "factorySettings.h"


// Interrupt Handler
void __interrupt(high_priority) highPriorityISR(void)
{
    
    if(SSP2IF) // data requested via I2C
    {
        PIR2bits.SSP2IF = 0x0;      // reset SSP2 flag
        sendParam();                // send values to DSP system
    }
   
    if (TMR4IF) // Timer for LED flashing (triggered every ~1ms)
    {
        TMR4IF = 0;                 // reset Timer 4 flag
        tapLED();                   // update tap LED
        flashLED();                 // flash LEDs if needed
        T4CONbits.TMR4ON = 1;       // re-start timer
    }
    
    if (TMR2IF) // TAP TEMPO TIMER
    { 
        TMR2IF = 0;                 // reset Timer 2 flag
        tapTimeCount++;             // increment tap time
        if (tapTimeCount >= 3000)   // time between taps is > 3 sec
        {
            tapTimeCount = 3000;
            T2CONbits.TMR2ON = 0;   // Timer 2 is OFF
        }
        else T2CONbits.TMR2ON = 1;  // Timer 2 is ON
    }
    
    checkSwitches(); // IOC event triggered by switch
    
    if (TMR0IF) // de-bounce of switches complete
    {
        TMR0IF = 0;                 // reset Timer 0 flag
        bounceCount++;
        if (bounceCount < 2)
        {
            T0CONbits.TMR0ON = 0x1;     // Timer 0 is ON  
        }
        else
        {       
            bounceCount = 0;
            serviceSwitches();          // action switch states
            T0CONbits.TMR0ON = 0x0;     // Timer 0 is OFF 
        }
    }
}

int absVal(int val) // return absolute value of 'val'   
{ 
    if (val < 0)
        return -val;
    else return val;
}

void readControls(void)
{
    // Delay Line A
    timeA = ADC_Read(14);
    parameter[0] = timeA >> 8;
    parameter[1] = (timeA & 0xFF);
            
    fbkA = ADC_Read(4);
    parameter[2] = fbkA>>4;
    
    lvlA = ADC_Read(3);
    parameter[3] = lvlA>>4;
    
    // Delay Line B
    timeB = ADC_Read(6);
    parameter[4] = timeB >> 8;
    parameter[5] = (timeB & 0xFF);
    
    fbkB = ADC_Read(2);
    parameter[6] = fbkB>>4;
    
    lvlB = ADC_Read(9);
    parameter[7] = lvlB>>4;
    
    // Delay Line C
    timeC = ADC_Read(1);
    parameter[8] = timeC >> 8;
    parameter[9] = (timeC & 0xFF);
    
    fbkC = ADC_Read(5);
    parameter[10] = fbkC>>4;
    
    lvlC = ADC_Read(0);
    parameter[11] = lvlC>>4;
}

void updatePreset(int slot, int values[])
{
    int i;
    
    for (i = 0; i < 12; i++)
    {
        userParams[slot][i] = values[i];
    }
}

void updateParams(int slot)
{
    int i;
    
    for (i = 0; i < 12; i++)
    {
        parameter[i] = userParams[slot][i];
    }
}

void fetchPreset(void)
{
    int test = 0;
    int i = 0;
    int values[12];
    
    for (i = 0; i < 5; i++)
    {
        I2C1_Block_Read_EERPOM(i, values, 12);
        updatePreset(i+1, values);
    }
}

void main(void)
{ 

    int dryLevel = 0;
    int prevDry = 0;
    int diff = 0;
    
    systemInit();
               
    fetchPreset();
   
    T4CONbits.TMR4ON = 1;
    
    I2C1_Write_DigiPot(0);
    
    while(1)
    {
        if (savePend)
        {
            I2C1_Page_Write_EEPROM((preset-1), parameter, 12);
            updatePreset(preset, parameter);
            savePend = 0;
        }
        
        if (preset > 0)
        {
            updateParams(preset);
        }
        else
        {   
            readControls();
            dryLevel = ADC_Read(8);
            dryLevel = dryLevel>>5;
            diff = dryLevel - prevDry;
            diff = absVal(diff);

            if (diff >= 2)
            {
                I2C1_Write_DigiPot(dryLevel);
                prevDry = dryLevel;
            } 
        }

        // signal that we're ready to go
        if (!setupComplete)
        {
            setupComplete = 1;
        } 
    }
}      
