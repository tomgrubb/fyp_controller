/*
 * File:   hardware.c
 * Author: Thomas M. Grubb
 *
 * All functions that pertain to updating, reading or servicing
 * inputs and outputs are contained here.
 * 
 * Created on July 8, 2018, 4:25 PM
 */


#include <xc.h>
#include "global.h"

void maxTapCheck(void)
{
    if (tapTimeCount > 3000)
    {
        tapTimeCount = 0;
        tap = 0;
    }
    else if (tapTimeCount >= 1500)
    {
        newTempo = 1;
        tap = 0;
        currentTapTime = 1500;
    }
}

void processTaps(void)
{
    int tapDiff = 0;
    
    if (tap < 1)    // first tap
    {       
        T2CONbits.TMR2ON = 0x1;     // start timing taps
        tapAccum = 0;
        tapPrev = 0;
        tap++;
    }
    else
    {
        T2CONbits.TMR2ON = 0;       // Timer 2 is OFF
        tapFlash = 1;
        tapDispCount = 0;
        TapLED = 0x1;
        
        if (tap < 2)
        {
            currentTapTime = tapTimeCount;
            tapPrev = tapTimeCount;
            tap++;
            maxTapCheck();
        }
        else
        {
            tapDiff = tapTimeCount - tapPrev;
            tapDiff = absVal(tapDiff);           
            if (tapDiff >= 500)
            {
                tap = 1;
            }
            else
            {
                currentTapTime = (tapTimeCount + tapPrev)/2;
                tapPrev = tapTimeCount;
                tap++;
            }
        }
        T2CONbits.TMR2ON = 1;       // Timer 1 is ON
    }
    tapTimeCount = 0;
}

void tapLED(void)
{
//    if (newTempo)
//    {
//        tapFlash = 1;
//        tapDispCount = 0;
//        newTempo = 0;
//    }
    
    if(tapFlash)
        {
            tapDispCount++;
            if (tapDispCount >= tapOnTime)
            {
                tapDispCount = 0;
                TapLED = 0; // LED OFF
                tapFlash = 0;
            }              
        }
        else
        {
            tapDispCount++;
            if (tapDispCount >= (currentTapTime-tapOnTime))
            {
                tapDispCount = 0;
                TapLED = 1; // LED ON
                tapFlash = 1;
            }
        }
}

void checkSwitches(void)
{
    char flag = 0;
    
    if(IOCF1)   // SW 4 pressed
    {
        switch4 = 1;                // set pressed flag
        flag = 1;
        IOCFbits.IOCF1 = 0;         // reset IOC4 flag
    }
    
    if (IOCF2)  // SW 2  pressed
    {
        switch2 = 1;                // set pressed flag
        flag = 1;
        IOCFbits.IOCF2 = 0;         // reset IOC4 flag
    }
    
    if (IOCF3)  // SW 1 pressed
    {
        switch1 = 1;                // set pressed flag
        flag = 1;
        IOCFbits.IOCF3 = 0;         // reset IOC4 flag
    }
    
    if (IOCF4)  // FSW 1 pressed
    {
        fsw1 = 1;                   // set pressed flag
        flag = 1;
        IOCFbits.IOCF4 = 0;         // reset IOC4 flag
    }
    
    if (IOCF5)  // FSW 2 pressed
    {
        fsw2 = 1;                   // set pressed flag
        flag = 1;
        IOCFbits.IOCF5 = 0;         // reset IOC4 flag
    }
    
    if (IOCF6)  // SW 3 pressed
    {
        switch3 = 1;                // set pressed flag
        flag = 1;
        IOCFbits.IOCF6 = 0;         // reset IOC4 flag
    }
    
    if (IOCF7)  // SW 5 pressed
    {
        switch5 = 1;                // set pressed flag
        flag = 1;
        IOCFbits.IOCF7 = 0;         // reset IOC4 flag
    }
    
    if (flag)
    {
        T0CONbits.TMR0ON = 0x1;     // Timer 0 is ON
    }
}

void serviceSwitches(void)
{   
    if (fsw1)   // bypass foot switch
    {
        fsw1 = 0;
        if (!BypassFSW)
        {
            bypass ^= 1;           // toggle bypass state
            if (bypMode)
            {
                CTRL = !bypass;         // switch relay
                TAILS = bypass;
            }
            else
            {
                CTRL = 0;
                TAILS = bypass;
            }
            BypassLED = bypass;    // switch LED
        }
    }
    
    if (fsw2)   // tap tempo footswitch
    {
        fsw2 = 0;
        if (AltFSW)
        {
            processTaps();
        }         
    }
    
    if (switch1)    // Switch 1 was pressed
    {
        switch1 = 0;
        if (!SW1)
        {
            intA++;
            if (intA >= 3)
            {
                intA = 0;
            }
            
            if (intA == 0)
            {
                ALED_UP = 1;
                ALED_DWN = 0;
            }
            else if (intA == 1)
            {
                ALED_UP = 0;
                ALED_DWN = 1;
            }
            else
            {
                ALED_UP = 1;
                ALED_DWN = 1;
            }
        }
    }

    if (switch2)    // Switch 2 was pressed
    {
        switch2 = 0;
        if (!SW2)
        {
            intB++;
            if (intB >= 3)
            {
                intB = 0;
            }
            
            if (intB == 0)
            {
                BLED_UP = 1;
                BLED_DWN = 0;
            }
            else if (intB == 1)
            {
                BLED_UP = 0;
                BLED_DWN = 1;
            }
            else
            {
                BLED_UP = 1;
                BLED_DWN = 1;
            }
        }
    }
        
    if (switch3)    // Switch 3 was pressed
    {
        switch3 = 0;
        if (!SW3)
        {
            intC++;
            if (intC >= 3)
            {
                intC = 0;
            }
            
            if (intC == 0)
            {
                CLED_UP = 1;
                CLED_DWN = 0;
            }
            else if (intC == 1)
            {
                CLED_UP = 0;
                CLED_DWN = 1;
            }
            else
            {
                CLED_UP = 1;
                CLED_DWN = 1;
            }
        }
    }
    
    if (switch4)
    {
        switch4 = 0;
        if (!SW4)
        {
            preset++;
            if (preset >= 6)
            {
                preset = 0;
            }
            updatePresetLEDs(preset);
        }
    }
    
    if (switch5)    // Switch 3 was pressed
    {
        switch5 = 0;
        if (!SW5)
        {
            swX ^= 1;
            bypMode = swX;
            if (swX)
            {
                LEDB = 1;
                LEDR = 0;
            }
            else
            {
                LEDB = 0;
                LEDR = 1;
            }
        }
    }
}

int ADC_Read(int channel)
{
    int value;
    ADCHS0Lbits.CH0SA = channel;    // + input for Channel 0, Sample A is AN0
    __delay_us(100);
    ADCON1Hbits.ADON = 1;           // ADC is ON
    ADCON1Lbits.SAMP = 1;           // begin sampling
    __delay_us(100);                // wait required delay (check Tad)
    ADCON1Lbits.SAMP = 0;           // stop sampling, start conversion    
    while (!ADCON1Lbits.DONE);      // wait until conversion is done     
    ADCON1Hbits.ADON = 0;           // ADC is OFF
    
    // fetch correct result from channel buffer
    if (channel == 0)
    {
        value = ADCBUF0;
    }
    else if (channel == 1)
    {
        value = ADCBUF1;
    }
    else if (channel == 2)
    {
        value = ADCBUF2;
    }
    else if (channel == 3)
    {
        value = ADCBUF3;
    }
    else if (channel == 4)
    {
        value = ADCBUF4;
    }
    else if (channel == 5)
    {
        value = ADCBUF5;
    }
    else if (channel == 6)
    {
        value = ADCBUF6;
    }
    else if (channel == 14)
    {
        value = ADCBUF14;
    }
    else if (channel == 8)
    {
        value = ADCBUF8;
    }
    else if (channel == 9)
    {
        value = ADCBUF9;
    }
    else value = 0;
    return (value);                 // return result               
}

void startupSequence(void)
{
    // === RAMP UP SEQUENCE ==
    
    // Preset LEDS
    P1LED = 1;
    __delay_ms(tShort);
    P2LED = 1;
    __delay_ms(tShort);
    P3LED = 1;
    __delay_ms(tShort);
    P4LED = 1;
    __delay_ms(tShort);
    P5LED = 1;
    __delay_ms(tShort);
    P5LED = 0;
    __delay_ms(tShort);
    P4LED = 0;
    __delay_ms(tShort);
    P3LED = 0;
    __delay_ms(tShort);
    P2LED = 0;
    __delay_ms(tShort);
    P1LED = 0;
    __delay_ms(tShort);
    
    // Interval LEDs
    ALED_UP = 1;
    __delay_ms(tShort);
    ALED_DWN = 1;
    __delay_ms(tShort);
    BLED_UP = 1;
    __delay_ms(tShort);
    BLED_DWN = 1;
    __delay_ms(tShort);
    CLED_UP = 1;
    __delay_ms(tShort);
    CLED_DWN = 1;
    __delay_ms(tShort);
    ALED_UP = 0;
    __delay_ms(tShort);
    ALED_DWN = 0;
    __delay_ms(tShort);
    BLED_UP = 0;
    __delay_ms(tShort);
    BLED_DWN = 0;
    __delay_ms(tShort);
    CLED_UP = 0;
    __delay_ms(tShort);
    CLED_DWN = 0;
    __delay_ms(tShort);
            
    // Ping-Pong FSW LEDs
    BypassLED = 1;
    TapLED = 0;
    __delay_ms(100);
    BypassLED = 0;
    TapLED = 1;
    __delay_ms(100);
    BypassLED = 1;
    TapLED = 0;
    __delay_ms(100);
    BypassLED = 0;
    TapLED = 1;
    __delay_ms(100);
    BypassLED = 0;
    TapLED = 0;
    __delay_ms(100);
    BypassLED = 1;
    TapLED = 1;
    __delay_ms(tShort);
    BypassLED = 0;
    TapLED = 0;
    __delay_ms(tShort);
    BypassLED = 1;
    TapLED = 1;
    __delay_ms(tShort);
    BypassLED = 0;
    TapLED = 0;
}

void readInterval(int select)
{
    int bitA = 0;
    int bitB = 0;
    int bitC = 0;
    
    if (select == 0)
    {
        bitA = INTA0;
        bitB = INTA1;
        bitC = INTA2;
    }
    else if (select == 1)
    {
        bitA = INTB0;
        bitB = INTB1;
        bitC = INTB2;
    }
    else
    {
        bitA = INTC0;
        bitB = INTC1;
        bitC = INTC2;
    }
    P1LED = bitA;
    P2LED = bitB;
    P3LED = bitC;          
}

void updatePresetLEDs(int psNum)
{
    if (psNum == 1)
    {
        P1LED = 1;
        P2LED = 0;
        P3LED = 0;
        P4LED = 0;
        P5LED = 0;
    }
    else if (psNum == 2)
    {
        P1LED = 0;
        P2LED = 1;
        P3LED = 0;
        P4LED = 0;
        P5LED = 0;   
    }
    else if (psNum == 3)
    {
        P1LED = 0;
        P2LED = 0;
        P3LED = 1;
        P4LED = 0;
        P5LED = 0;    
    }
    else if (psNum == 4)
    {
        P1LED = 0;
        P2LED = 0;
        P3LED = 0;
        P4LED = 1;
        P5LED = 0;
    }
    else if (psNum == 5)
    {
        P1LED = 0;
        P2LED = 0;
        P3LED = 0;
        P4LED = 0;
        P5LED = 1;
    }
    else
    {
        P1LED = 0;
        P2LED = 0;
        P3LED = 0;
        P4LED = 0;
        P5LED = 0;
    }
}

int readToggle(int target)
{
    int temp = 0;
    int result = 0;
    
    if (target) // read MODE
    {
        temp = modeA;
        result = modeB;
        result = (modeA<<1)|(modeB);                
    }
    else result = lockIn;
    return result;
}

void killLEDs(void)
{
    TapLED = 0;
    BypassLED = 0;
    ALED_UP = 0;
    ALED_DWN = 0;
    BLED_UP = 0;
    BLED_DWN = 0;
    CLED_UP = 0;
    CLED_DWN = 0;
    LEDB = 0;
    LEDR = 0;
    P1LED = 0;
    P2LED = 0;
    P3LED = 0;
    P4LED = 0;
    P5LED = 0;
}
