#ifndef GLOBAL_H
#define	GLOBAL_H

// DEFINE GLOBAL MACROS
#define _XTAL_FREQ 32000000
#define DigiPotAddr 0b00101111
#define tShort 75

// INPUT/OUTPUT MACROS
#define TapLED LATE0
#define BypassLED LATD2 
#define CTRL LATD3
#define TAILS LATB5

#define SW1 RG1
#define SW2 RG2
#define SW3 RE6
#define SW4 RD1
#define SW5 RG3
#define BypassFSW RD4
#define AltFSW RE3
#define modeA RJ1
#define modeB RJ0
#define lockIn RD7

#define P1LED LATD0
#define P2LED LATE7
#define P3LED LATE5
#define P4LED LATE4
#define P5LED LATH2

#define LEDB LATE1
#define LEDR LATH1

#define INTA0 RC7
#define INTA1 RJ4
#define INTA2 RJ5
#define ALED_UP LATH7
#define ALED_DWN LATF2

#define INTB0 RC1
#define INTB1 RC0
#define INTB2 RC6
#define BLED_UP LATF5
#define BLED_DWN LATF6

#define INTC0 RF3
#define INTC1 RH5
#define INTC2 RH4
#define CLED_UP LATG4
#define CLED_DWN LATH0

// FUNCTION PROTOTYPES
void interruptInit(void);
void systemInit(void);
int ADC_Read(int channel);
void updateLEDs(void);
void killLEDs(void);
void I2C1_Write_EEPROM(char devAddr, char memValue, char value);
char I2C1_Read_EEPROM(char devAddr, char memValue);
void I2C1_Write_DigiPot(char value);
void readInterval(int select);
void tapLED(void);
void checkSwitches(void);
void serviceSwitches(void);
void startupSequence(void);
void sendParam(void); 
void processTaps(void);
void updatePresetLEDs(int psNum);
int readToggle(int target);
int absVal(int val); 

char intA = 0;
char intB = 0;
char intC = 0;
char swX = 0;

// Switch Flags
int switch1 = 0;
int switch2 = 0;
int switch3 = 0;
int switch4 = 0;
int switch5 = 0;
int fsw1 = 0;
int fsw2 = 0;

int muxADC = 0;

char bypass = 0;
char preset = 0;

// Tap Tempo LED variables
int tapFlash = 0;
int tapOnTime = 5;
int tapDispCount = 0;
int currentTapTime = 500;

// Parameter variables
int dataTarget = 0;
int *dataPtr;
int feedbackValue = 0;
int levelValue = 0;
int timeValue = 0;

// Tap Tempo Variables
int tapTimeCount = 0;
int tapPrev = 0;
long int tapAccum = 0;
int tap = 0;
int tapTime = 0;
int newTempo = 0;
int timeoutCount = 0;

#endif /* GLOBAL_H */