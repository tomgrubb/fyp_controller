#ifndef GLOBAL_H
#define	GLOBAL_H

// DEFINE GLOBAL MACROS
#define _XTAL_FREQ 32000000
#define DigiPotAddr 0b00101111
#define EEPROM_ADD  0b10100000 
#define tShort 75

// INPUT/OUTPUT MACROS
#define TapLED LATE0
#define BypassLED LATD2 
#define CTRL LATD3
#define TAILS LATB5
#define PARAM_NUM 12

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
void I2C1_Write_EEPROM(char memValue, char value);
void I2C1_Page_Write_EEPROM(int slot, int *data[], int bytes);
void I2C1_Block_Read_EERPOM(int slot, int *data[], int bytes);
char I2C1_Read_EEPROM(char devAddr, char memValue);
void I2C1_Write_DigiPot(char value);
void readInterval(int select);
void tapLED(void);
void flashLED(void);
void checkSwitches(void);
void serviceSwitches(void);
void startupSequence(void);
void sendParam(void); 
void processTaps(void);
void updatePresetLEDs(int psNum);
void updateLineLEDs(int lineA, int lineB, int lineC);
int readToggle(int target);
int absVal(int val);
void presetCtrl(int slot);

int intA = 0;
int intB = 0;
int intC = 0;
int swX = 1;
int shiftAction = 0;

// Switch Flags
int switch1 = 0;
int switch2 = 0;
int switch3 = 0;
int switch4 = 0;
int switch5 = 0;
int fsw1 = 0;
int fsw2 = 0;
int bounceCount = 0;

int muxADC = 0;

int bypass = 0;
int preset = 0;
int targPreset = 0;
int presetPend = 0;
int savePend = 0;
int slotUsed = 0;
int pFlashCount = 0;
int pFlash = 0;

int bypMode = 1;

// Tap Tempo LED variables
int tapFlash = 0;
int tapOnTime = 5;
int tapDispCount = 0;
int currentTapTime = 500;
int armFlashCount = 0;
int armFlash = 0;
int armFlashToggle = 0;

// Parameter variables
int parameter[12]; 

int shift = 0;
int armA = 0;
int armB = 0;
int armC = 0;
int linesArmed = 0;

int dataTarget = 0;
int *dataPtr;

int fbkA = 0;
int lvlA = 0;
int timeA = 0;

int fbkB = 0;
int lvlB = 0;
int timeB = 0;

int fbkC = 0;
int lvlC = 0;
int timeC = 0;

// Tap Tempo Variables
int tapTimeCount = 0;
int tapPrev = 0;
long int tapAccum = 0;
int tap = 0;
int tapTime = 0;
int newTempo = 0;
int timeoutCount = 0;

int setupComplete = 0;
int sync = 0;
#endif /* GLOBAL_H */