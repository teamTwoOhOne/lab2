//-----------------------------------------//
// ECE 372A Spring 2016
// Lab Group 201:
// Code Written by Zachary Finsterwald
//-----------------------------------------//


#include <xc.h>
#include <sys/attribs.h>
#include "lcd.h"
#include "interrupt.h"
#include "timer.h"
#include "config.h"
#include "keypad.h"

#define press 0
#define release 1

typedef enum stateTypeEnum {
    wait, scan, write, debounce_press, debounce_release
} stateType;

volatile stateType state;

int main(void)
{
    //---INITIALIZE---//
    SYSTEMConfigPerformance(10000000);
    init_timer_1();
    init_lcd();
    init_keypad();
    clear_lcd();
    enableInterrupts();
    state = scan;
    //----------------//
    
    unsigned char key   = -1;
    int cursor_pos      = 0;
    
    while(1)
    {
        
        switch(state){
            case wait:
                if(cursor_pos == 32) cursor_pos = 0;
                break;
            case scan:
                key = scan_keypad();
                break;
            case write:
                if(cursor_pos == 0)     move_cursor_lcd(0,1);
                if(cursor_pos == 16)    move_cursor_lcd(0,2);
                cursor_pos++;
                print_char_lcd(key);
                state = scan;
                break;
            case debounce_press:
                delay_us(5000);
                state = wait;
				break;
            case debounce_release:
                delay_us(5000);
                state = write;
                break;
        }
    }
    
    return 0;
}

// Keypad Interrupt
void __ISR(_CHANGE_NOTICE_VECTOR, IPL7SRS) _CNInterrupt(void)
{
    // PUT DOWN FLAGS
    IFS1bits.CNFIF      = 0;
    IFS1bits.CNGIF      = 0;
    IFS1bits.CNDIF      = 0;
    
    PORTF;
    PORTG;
    PORTD;
    
    if      (state == scan) state = debounce_press;
    else if (state == wait) state = debounce_release;
    

}
