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
    wait, scan, set, bad, write, debounce_press, debounce_release
} stateType;

volatile stateType state;

int main(void)
{
    // * INITIALIZE * //
    SYSTEMConfigPerformance(10000000);
    init_timer_1();
    init_lcd();
    init_keypad();
    clear_lcd();
    enableInterrupts();
    state = scan;
    //----------------//
    
    char * enter_s    = "Enter:";
    char * set_s      = "Set:";
    char * bad_s      = "Bad!";
    char * display_s  = "Enter:";
    
    unsigned char key   = -1;
    int key_cnt         = 0;
    

    while(1)
    {
        
        switch(state){
            case wait:
                move_cursor_lcd(0,1);
                print_string_lcd(display_s);
                break;
            case scan:
                key = scan_keypad();
                if(key_cnt == 0 && key == '*')
                {
                    clear_lcd();
                    display_s = set_s;
                }   
                else if(key == '*' || key == '#') 
                {
                    clear_lcd();
                    display_s = bad_s;
                }
                break;
            case write:
                if(key_cnt == 3)        clear_lcd();
                move_cursor_lcd(key_cnt, 2);
                print_char_lcd(key);
                key_cnt++;
                if(key_cnt == 3) key_cnt = 0;
                state = scan;
                break;
            case bad:
                break;
            case set:
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

// Keypad Interrupt //
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
