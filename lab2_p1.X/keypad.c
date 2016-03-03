
#include <xc.h>
#include "keypad.h"
#include "timer.h"

/* Initialize the rows as ODC outputs and the columns as inputs with pull-up
 * resistors. Don't forget about other considerations...
 */
// KEYPAD LAYOUT
//  -----------
//  |1   2   3| PIN 2
//  |4   5   6| PIN 7
//  |7   8   9| PIN 6
//  |*   0   #| PIN 4
//  -----------
//  |3| |1| |5| <-- COL PIN

// COL 1 - PIN 3 - RF1
// COL 2 - PIN 1 - RG13
// COL 3 - PIN 5 - RD6

// ROW 1 - PIN 2 - RG0
// ROW 2 - PIN 7 - RD3
// ROW 3 - PIN 6 - RD1
// ROW 4 - PIN 4 - RD12

#define INPUT   1
#define OUTPUT  0

#define ENABLE  1
#define DISABLE 0

#define OPEN    1
#define CLOSED  0

#define ROW1_LAT    LATGbits.LATG0
#define ROW2_LAT    LATDbits.LATD3
#define ROW3_LAT    LATDbits.LATD1 
#define ROW4_LAT    LATDbits.LATD12

#define COL1_PORT   PORTFbits.RF1
#define COL2_PORT   PORTGbits.RG13
#define COL3_PORT   PORTDbits.RD6

void init_keypad(void){
    
    //------- SETUP ODC ROWS -------//
    
    // USE ANALOG PINS AS DIGITAL INPUTS
    ANSELDbits.ANSD1    = 0;
    ANSELDbits.ANSD3    = 0;
    
    // SET ROW AS OUTPUTS
    TRISGbits.TRISG0    = OUTPUT;
    TRISDbits.TRISD3    = OUTPUT;
    TRISDbits.TRISD1    = OUTPUT;
    TRISDbits.TRISD12   = OUTPUT;
    
    // ENABLE ODC
    ODCGbits.ODCG0      = ENABLE;
    ODCDbits.ODCD3      = ENABLE;
    ODCDbits.ODCD1      = ENABLE;
    ODCDbits.ODCD12     = ENABLE;
    
    // OPEN DRAIN
    ROW1_LAT    = OPEN;
    ROW2_LAT    = OPEN;
    ROW3_LAT    = OPEN;
    ROW4_LAT    = OPEN;
    
   //------- SETUP COLUMNS -------// 
    // SET AS INPUTS
    TRISFbits.TRISF1    = INPUT;
    TRISGbits.TRISG13   = INPUT;
    TRISDbits.TRISD6    = INPUT;
    
    // ENABLE INTERNAL PULL-UP
    CNPUFbits.CNPUF1    = 1;
    CNPUGbits.CNPUG13   = 1;
    CNPUDbits.CNPUD6    = 1;
    
    // ENABLE CN INTERRUPTS
    CNCONFbits.ON       = 1;
    CNCONGbits.ON       = 1;
    CNCONDbits.ON       = 1;
    
    // ENABLE SPECIFIC INTERRUPTS
    CNENFbits.CNIEF1    = 1;
    CNENGbits.CNIEG13   = 1;
    CNENDbits.CNIED6    = 1;
    
    // ENABLE OVERALL INTERRUPTS
    IEC1bits.CNFIE      = 1;
    IEC1bits.CNGIE      = 1;
    IEC1bits.CNDIE      = 1;
    
    // PUT DOWN FLAGS
    IFS1bits.CNFIF      = 0;
    IFS1bits.CNGIF      = 0;
    IFS1bits.CNDIF      = 0;
    
    IPC8bits.CNIP       = 7;
}

/* This function will be called AFTER you have determined that someone pressed
 * SOME key. This function is to figure out WHICH key has been pressed.
 * This function should return -1 if more than one key is pressed or if
 * no key is pressed at all. Otherwise, it should return the ASCII character of
 * the key that is pressed. The ascii character c programmatically is just 'c'
 */
unsigned char scan_keypad(void){
    
    unsigned char key = -1;
    
    // SCAN ROW 1 //
    ROW1_LAT    = CLOSED;
    ROW2_LAT    = OPEN;
    ROW3_LAT    = OPEN;
    ROW4_LAT    = OPEN;
    
    delay_us(1000);
    
    if(COL1_PORT == 0 && COL2_PORT == 1 && COL3_PORT == 1) return key = '1';
    if(COL2_PORT == 0 && COL1_PORT == 1 && COL3_PORT == 1) return key = '2';
    if(COL3_PORT == 0 && COL1_PORT == 1 && COL2_PORT == 1) return key = '3';

    // ---------- //
    
    // SCAN ROW 2 //
    ROW1_LAT    = OPEN;
    ROW2_LAT    = CLOSED;
    ROW3_LAT    = OPEN;
    ROW4_LAT    = OPEN;
    
    delay_us(1000);
    
    if(COL1_PORT == 0 && COL2_PORT == 1 && COL3_PORT == 1) return key = '4';
    if(COL2_PORT == 0 && COL1_PORT == 1 && COL3_PORT == 1) return key = '5';
    if(COL3_PORT == 0 && COL1_PORT == 1 && COL2_PORT == 1) return key = '6';
    
    // ---------- //
    
    // SCAN ROW 3 //
    ROW1_LAT    = OPEN;
    ROW2_LAT    = OPEN;
    ROW3_LAT    = CLOSED;
    ROW4_LAT    = OPEN;
    
    delay_us(1000);

    if(COL1_PORT == 0 && COL2_PORT == 1 && COL3_PORT == 1) return key = '7';
    if(COL2_PORT == 0 && COL1_PORT == 1 && COL3_PORT == 1) return key = '8';
    if(COL3_PORT == 0 && COL1_PORT == 1 && COL2_PORT == 1) return key = '9';
    
    // ---------- //
    
    // SCAN ROW 4 //
    ROW1_LAT    = OPEN;
    ROW2_LAT    = OPEN;
    ROW3_LAT    = OPEN;
    ROW4_LAT    = CLOSED;
    
    delay_us(1000);
    
    if(COL1_PORT == 0 && COL2_PORT == 1 && COL3_PORT == 1) return key = '*';
    if(COL2_PORT == 0 && COL1_PORT == 1 && COL3_PORT == 1) return key = '0';
    if(COL3_PORT == 0 && COL1_PORT == 1 && COL2_PORT == 1) return key = '#';
    
    // ---------- //
    if(key != -1) return key;
}
