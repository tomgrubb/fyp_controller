/*
 * File:   systemConfig.c
 * Author: Thomas M. Grubb
 *
 * Setup and configuration of all modules and hardware is contained here. 
 * 
 * Created on July 8, 2018, 4:18 PM
 */


#include <xc.h>
#include <pic18f85j94.h>
#include "global.h"

// Switch de-bounce timer
void setupTMR0(void)
{
    T0CONbits.TMR0ON = 0x0;         // Timer 0 is OFF
    T0CONbits.T08BIT = 0x1;         // 8 bit operation
    T0CONbits.T0CS = 0x1;           // Fosc/4 is clock source
    T0CONbits.PSA = 0x0;            // prescaler is assigned
    T0CONbits.T0PS = 0x6;           // 1:128 prescale    
}

void setupTMR1(void)
{
    T1CONbits.TMR1ON = 0x0;         // Timer 1 is OFF
    T1CONbits.TMR1CS = 0x0;         // Fosc/4 is clock source
    T1CONbits.TCKPS = 0x3;          // 1:8 prescale
    T1CONbits.RD16 = 0x1;           // read write 16 bits in one operation    
}

// Tap Tempo Timer
void setupTMR2(void)
{
    // Timer interrupt period is 1.024ms
    T2CONbits.TMR2ON = 0x0;         // Timer 2 is OFF
    T2CONbits.T2CKPS = 0x3;         // 1:16 prescale
    T2CONbits.T2OUTPS = 0x1;        // 1:2 postscale
}

// Timer for TAP LED
void setupTMR4(void)
{
    // Timer interrupt period is 1.024ms
    T4CONbits.TMR4ON = 0x0;         // Timer 4 is OFF
    T4CONbits.T4CKPS = 0x3;         // 1:16 prescale
    T4CONbits.T4OUTPS = 0x1;        // 1:2 postscale
}

// switch clock from FRC to FRC w/ PLL (32 MHz)
void clockSwitch(void)
{
    OSCCON4bits.PLLEN = 0x1;        // enable PLL for warm-up
    OSCCON3bits.IRCF = 0x1;         // FRC divided by 2 (4 MHz)
    OSCCON2bits.CLKLOCK = 0x0;      // clear CLOCK LOCK
    OSCCONbits.NOSC = 0x1;          // new oscillator is FRCDIV w/ PLL
    
    while(OSCCONbits.COSC != 0x1);  // wait for clock to switch
    while (!OSCCON2bits.LOCK);      // wait for PLL to lock
}

void setupPWM(void)
{
    PR2 = 0xFF;                     // set period
    CCPR4L = 0x0A;                  // CCP4 period LOW byte
    CCP4CONbits.DC4B = 0x4;         // CCP4 period upper bits
    CCP4CONbits.CCP4M = 0xFF;       // PWM mode selected
    CCPTMRS1bits.C4TSEL = 0x00;     // CCP7 is based off TMR1/TMR2
    TRISCbits.TRISC2 = 0;           // RB3 is an OUTPUT
    CCPR4H = 0x00;                  // CCP4 period HIGH byte
    T2CON = 0x01;                   // set pre-scaler
    TMR2ON = 1;                     // turn TMR2 ON
}

void setupI2C1(void)
{
    SSP1CON1bits.SSPEN = 0x0;       // DISABLE I2C Module
    
    TRISCbits.TRISC3 = 0x1;         // RC3 in an OUTPUT (SCL1)
    TRISCbits.TRISC4 = 0x1;         // RC4 is an INPUT (SDA1)
    
    // Master Mode, Clock = Fosc / (4*(SSP1ADD + 1))
    SSP1ADD = 0x13;                 // Clock speed is 100 kHz
    SSP1CON1bits.SSPM = 0x8;        // Master Mode, Slave is idle
    
    SSP1BUF = 0x00;                 // Clear buffer
    
    SSP1CON1bits.SSPEN = 0x1;       // ENABLE I2C Module
}

void setupI2C2(void)
{
    SSP2CON1bits.SSPEN = 0x0;       // DISABLE I2C Module
    
    TRISDbits.TRISD5 = 0x1;         // RD5 is an INPUT (SDA2)
    TRISDbits.TRISD6 = 0x1;         // RD6 is an INPUT (SCL2)
    
    // I2C Slave mode, 7 bit addressing, START and STOP interrupts DISABLED
    SSP2CON1bits.SSPM = 0x9;
    
    // acknowledged addresses are 0xA0, 0xA4, 0xA4, 0xAC
    SSP2ADD = 0b10100000;
    SSP2MSK = 0b11000000;
    SSP2CON1bits.SSPM = 0x6;        // 7-bit masking enabled
    
    SSP2CON1bits.CKP = 0x1;         // Release Clock
    SSP2CON2bits.SEN = 0x1;         // Stretch Condition Enable bit
    SSP2CON2bits.GCEN = 0x0;        // General Call Address DISABLED
    SSP2CON3bits.SCIE = 0x0;        // Start Condition Interrupt DISABLED
    
    SSP2BUF = 0x00;                 // clear buffer
    SSP2CON1bits.SSPEN = 0x1;       // ENABLE I2C Module
}

void setupADC(void)
{
    ADCON1Hbits.ADON = 0;           // turn off ADC
    ADCON1Hbits.MODE12 = 1;         // 12-bit mode
    ADCON1Hbits.FORM = 0;           // abs dec result, unsigned, right-justified    
    ADCON1Lbits.SSRC = 0;           // SAMP must be cleared by software
    ADCON1Lbits.ASAM = 0;           // sampling starts when SAMP is set
    
    ADCON2Hbits.PVCFG = 0;          // AVdd is positive voltage reference
    ADCON2Hbits.NVCFG0 = 0;         // AVss is negative voltage reference
    ADCON2Hbits.BUFREGEN = 1;       // result is loaded into channel buffer
    
    ADCON3Hbits.ADRC = 0;           // conversion clock derived from system clock
    ADCON3Hbits.PUMPEN = 0;         // charge pump DISABLED
    ADCON3Lbits.ADCS = 0x20;        // Tad = 2/Fosc
    
    ADCON5Hbits.ASENA = 0;          // auto-scan is DISABLED
    ADCHS0Lbits.CH0SA = 0;          // + input for Channel 0, Sample A is AN0
    ADCHS0Lbits.CH0NA = 0;          // Vref-/Avss
    
    // Disable ALL Comparators
    CM1CONbits.CON = 0;
    CM2CONbits.CON = 0;
    CM3CONbits.CON = 0;
    
    ANCON1bits.ANSEL0 = 1;          // RA0 is defined as ANALOG  
    ANCON1bits.ANSEL1 = 1;          // RA1 is defined as ANALOG  
    ANCON1bits.ANSEL2 = 1;          // RA2 is defined as ANALOG  
    ANCON1bits.ANSEL3 = 1;          // RA3 is defined as ANALOG 
    ANCON1bits.ANSEL4 = 1;          // RA4 is defined as ANALOG 
    ANCON1bits.ANSEL5 = 1;          // RA5 is defined as ANALOG
    ANCON2bits.ANSEL11 = 1;         // RF7 is defined as ANALOG
    ANCON1bits.ANSEL7 = 1;          // RG7 is defined as ANALOG
    ANCON2bits.ANSEL8 = 1;          // RC2 is defined as ANALOG
    ANCON3bits.ANSEL22 = 1;         // RH6 is defined as ANALOG
    
    TRISAbits.TRISA0 = 1;           // RA0 is an INPUT (AN0) -> CV0
    TRISAbits.TRISA1 = 1;           // RA1 is an INPUT (AN1) -> CV1
    TRISAbits.TRISA2 = 1;           // RA2 is an INPUT (AN2) -> CV2
    TRISAbits.TRISA3 = 1;           // RA3 is an INPUT (AN3) -> CV3
    TRISAbits.TRISA5 = 1;           // RA5 is an INPUT (AN4) -> CV4
    TRISFbits.TRISF7 = 1;           // RF7 is an INPUT (AN5) -> CV5
    TRISAbits.TRISA4 = 1;           // RA4 is an INPUT (AN6) -> CV6
    TRISHbits.TRISH6 = 1;           // RH6 is an INPUT (AN14) -> CV7
    TRISGbits.TRISG0 = 1;           // RG0 is an INPUT (AN8) -> CV8
    TRISCbits.TRISC2 = 1;           // RC2 is an INPUT (AN9) -> CV9    
}

void mapPeripheralPins(void)
{
    // PPS Initialisation
    OSCCON = (OSCCON & 0x5F);       // clear IOLOCK
    RPINR20_21bits.IOC3R = 0x9;     // RP39 is IOC3 (SW1)
    RPINR20_21bits.IOC2R = 0xA;     // RP42 is IOC2 (SW2)
    RPINR24_25bits.IOC6R = 0x8;     // RP34 is IOC6 (SW3)
    RPINR18_19bits.IOC1R = 0x5;     // RP21 is IOC1 (SW4)
    RPINR24_25bits.IOC7R = 0xA;     // RP43 is IOC7 (SW5)
    RPINR22_23bits.IOC4R = 0x6;     // RP24 is IOC4 (FSW1)
    RPINR22_23bits.IOC5R = 0x8;     // RP33 is IOC5 (FSW2)
    OSCCON  = (OSCCON | 0x40);      // set IOLOCK
}

void disableLCD(void)
{
    LCDCONbits.LCDEN = 0;
    LCDCONbits.LMUX = 0x0;
    
    LCDSE0 = 0x00;
    LCDSE1 = 0x00;
    LCDSE2 = 0x00;
    LCDSE3 = 0x00;
    LCDSE4 = 0x00;
    LCDSE5 = 0x00;
    LCDSE6 = 0x00;
    LCDSE7 = 0x00;    
}

void interfaceInit(void)
{
    //disableLCD();
    MEMCONbits.EBDIS = 1; 
    UCONbits.USBEN = 0;
    
    // ENSURE Secondary OSC is DISABLED
    OSCCON2bits.SOSCGO = 0;
    T1CONbits.SOSCEN = 0;
    T3CONbits.SOSCEN = 0;
    T5CONbits.SOSCEN = 0;
    
    // I/O Pin Initialisations
    
    // Switches
    ANCON2bits.ANSEL12 = 0;         // RG1 is DIGITAL
    TRISGbits.TRISG1 = 1;           // RG1 is INPUT (SW1)
    PADCFG1bits.RGPU = 1;
    
    ANCON2bits.ANSEL13 = 0;         // RG2 is DIGITAL
    TRISGbits.TRISG2 = 1;           // RG2 is INPUT (SW2)
    
    TRISEbits.TRISE6 = 1;           // RE6 is INPUT (SW3)
    
    TRISDbits.TRISD1 = 1;           // RD1 is INPUT (SW4)
    PADCFG1bits.RDPU = 1;
    
    ANCON2bits.ANSEL14 = 0;         // RG3 is DIGITAL
    TRISGbits.TRISG3 = 1;           // RG3 is INPUT (SW5)
    
    PADCFG1bits.REPU = 1;
    TRISDbits.TRISD4 = 1;           // RD4 is INPUT (FSW1)    
    TRISEbits.TRISE3 = 1;           // RE3 is INPUT (FSW2)
    
    // Tap and Bypass LEDs
    TRISEbits.TRISE0 = 0;           // RE0 is LED7 OUTPUT (Tap)
    TRISDbits.TRISD2 = 0;           // RD2 is LED6 OUTPUT (Byp)
    
    // RGB 'X2' LED
    TRISEbits.TRISE1 = 0;           // RE1 is LEDB OUTPUT (Blue)
    ANCON3bits.ANSEL17 = 0;         // RH1 is DIGITAL
    TRISHbits.TRISH1 = 0;           // RH1 is LEDR OUTPUT (Red) 
    
    // PRESET LEDs
    TRISDbits.TRISD0 = 0;           // RD0 is LED5 OUTPUT
    TRISEbits.TRISE7 = 0;           // RE7 is LED4 OUTPUT
    TRISEbits.TRISE5 = 0;           // RE5 is LED3 OUTPUT
    TRISEbits.TRISE4 = 0;           // RE4 is LED2 OUTPUT
    ANCON3bits.ANSEL18 = 0;         // RH2 is DIGITAL
    TRISHbits.TRISH2 = 0;           // RH2 is LED1 OUTPUT
    
    // Interval 'A' LEDs
    ANCON1bits.ANSEL6 = 0;          // RF2 is DIGITAL
    TRISFbits.TRISF2 = 0;           // RF2 is LED11 OUTPUT
    ANCON3bits.ANSEL23 = 0;         // RH7 is DIGITAL
    TRISHbits.TRISH7 = 0;           // RH7 is LED10 OUTPUT
    
    // Interval 'B' LEDs
    ANCON2bits.ANSEL10 = 0;         // RF6 is DIGITAL
    TRISFbits.TRISF6 = 0;           // RF6 is LED13 OUTPUT    
    ANCON2bits.ANSEL9 = 0;          // RF6 is DIGITAL
    TRISFbits.TRISF5 = 0;           // RF5 is LED12 OUTPUT
    
    // Interval 'C' LEDs
    ANCON3bits.ANSEL16 = 0;         // RH0 is DIGITAL
    TRISHbits.TRISH0 = 0;           // RH0 is LED8 OUTPUT
    TRISGbits.TRISG4 = 0;           // RG4 is LED9 OUTPUT
    ANCON2bits.ANSEL15 = 0;         // RG4 is DIGITAL 
    
    // Priority Encoder 'A'
    TRISCbits.TRISC7 = 1;           // RC7 is INT 'A0' INPUT
    TRISJbits.TRISJ4 = 1;           // RJ4 is INT 'A1' INPUT
    TRISJbits.TRISJ5 = 1;           // RJ5 is INT 'A2' INPUT
    
    // Priority Encoder 'B'
    TRISCbits.TRISC1 = 1;           // RC1 is INT 'B0' INPUT
    TRISCbits.TRISC0 = 1;           // RC0 is INT 'B1' INPUT
    TRISCbits.TRISC6 = 1;           // RC6 is INT 'B2' INPUT
    
    // Priority Encoder 'C'
    ANCON3bits.ANSEL21 = 0;         // RH5 is DIGITAL
    ANCON3bits.ANSEL20 = 0;         // RH4 is DIGITAL
    TRISFbits.TRISF3 = 1;           // RF3 is INT 'C0' INPUT
    TRISHbits.TRISH5 = 1;           // RH5 is INT 'C1' INPUT 
    TRISHbits.TRISH4 = 1;           // RH4 is INT 'C2' INPUT
    
    // 'Tails' and Relay Control Outputs
    TRISBbits.TRISB5 = 0;           // RB5 is OUTPUT (Tails)
    TRISDbits.TRISD3 = 0;           // RD3 is OUTPUT (Relay)
    TAILS = 1;
    CTRL = 0;
    
    // 'Mode' and 'Lock' Inputs
    TRISJbits.TRISJ0 = 1;           // RJ0 is INPUT (Mode A)
    TRISJbits.TRISJ1 = 1;           // RJ1 is INPUT (Mode B)
    TRISDbits.TRISD7 = 1;           // RD7 is INPUT (Lock)    
}

void interruptInit(void)
{
    INTCONbits.T0IE = 0x1;            // ENABLE Timer 0 interrupts
    PIE1bits.TMR1IE = 0x1;            // ENABLE Timer 1 interrupts
    PIE1bits.TMR2IE = 0x1;            // ENABLE Timer 2 interrupts
    PIE2bits.TMR3IE = 0x1;            // ENABLE Timer 3 interrupts
    PIE5bits.TMR4IE = 0x1;            // ENABLE Timer 4 interrupts
    
    PIE2bits.SSP2IE = 0x1;            // ENABLE MSSP2 interrupts
    
    INTCONbits.IOCIE = 0x1;           // ENABLE Interrupt On Change
    IOCNbits.IOCN1 = 0x1;             // falling edge for IOC1
    IOCNbits.IOCN2 = 0x1;             // falling edge for IOC2
    IOCNbits.IOCN3 = 0x1;             // falling edge for IOC3
    IOCNbits.IOCN4 = 0x1;             // falling edge for IOC4
    IOCNbits.IOCN5 = 0x1;             // falling edge for IOC5
    IOCNbits.IOCN6 = 0x1;             // falling edge for IOC6
    IOCNbits.IOCN7 = 0x1;             // falling edge for IOC7
    
    INTCONbits.PEIE = 0x1;            // ENABLE peripheral interrupts
    INTCONbits.GIE = 0x1;             // ENABLE global interrupts
}

void defaultLED(void)
{
    ALED_UP = 1;
    ALED_DWN = 1;
    BLED_UP = 1;
    BLED_DWN = 1;
    CLED_UP = 1;
    CLED_DWN = 1;
    P1LED = 1;
    P2LED = 1;
    P3LED = 1;
    P4LED = 1;
    P5LED = 1; 
}

void bypassSetup(void)
{
    TAILS = 1;
    CTRL = 1;
    LEDB = 1;
    LEDR = 0;
    BypassLED = 0;
}

void systemInit(void)
{
    clockSwitch();
    
    setupTMR0();
    setupTMR1();
    setupTMR2();
    setupTMR4();
    
    setupI2C1();
    setupI2C2();
    
    setupADC();
    //setupPWM();
    
    mapPeripheralPins();
    interfaceInit();
    bypassSetup();
    killLEDs();
    startupSequence();
    defaultLED();
    killLEDs();
    bypassSetup();
    interruptInit();
}
