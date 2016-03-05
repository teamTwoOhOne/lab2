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

typedef int bool;
#define true    1
#define false   0

typedef enum stateTypeEnum {
    wait, scan, set, message, enter, debounce_press, debounce_release
} stateType;

volatile stateType state;

char temp_pw [5]    = {'\0', '\0', '\0', '\0', '\0'};

char pw0 [5]  = {'0', '0', '0', '0', '\0'};
char pw1 [5]  = {'\0', '\0', '\0', '\0', '\0'};
char pw2 [5]  = {'\0', '\0', '\0', '\0', '\0'};
char pw3 [5]  = {'\0', '\0', '\0', '\0', '\0'};
char pw4 [5]  = {'\0', '\0', '\0', '\0', '\0'};


unsigned char key   = -1;
int key_cnt         = 0;


void reset_pw()
{
    temp_pw[0] = '\0';
    temp_pw[1] = '\0';
    temp_pw[2] = '\0';
    temp_pw[3] = '\0';
    key_cnt = 0;
}

bool check_pw(char * in)
{
    if( (in[0] == pw0[0]) && (in[1] == pw0[1]) && (in[2] == pw0[2]) && (in[3] == pw0[3]) ) return true;
    if( (in[0] == pw1[0]) && (in[1] == pw1[1]) && (in[2] == pw1[2]) && (in[3] == pw1[3]) ) return true;
    if( (in[0] == pw2[0]) && (in[1] == pw2[1]) && (in[2] == pw2[2]) && (in[3] == pw2[3]) ) return true;
    if( (in[0] == pw3[0]) && (in[1] == pw3[1]) && (in[2] == pw3[2]) && (in[3] == pw3[3]) ) return true;
    if( (in[0] == pw4[0]) && (in[1] == pw4[1]) && (in[2] == pw4[2]) && (in[3] == pw4[3]) ) return true;
    
    else return false;
}

bool add_pw(char * in)
{
    if (pw1[0] == '\0') {
        pw1[0] = in[0];
        pw1[1] = in[1];
        pw1[2] = in[2];
        pw1[3] = in[3];
        return true;
    } else if (pw2[0] == '\0') {
        pw2[0] = in[0];
        pw2[1] = in[1];
        pw2[2] = in[2];
        pw2[3] = in[3];
        return true;
    } else if (pw3[0] == '\0') {
        pw3[0] = in[0];
        pw3[1] = in[1];
        pw3[2] = in[2];
        pw3[3] = in[3];
        return true;
    } else if (pw4[0] == '\0') {
        pw4[0] = in[0];
        pw4[1] = in[1];
        pw4[2] = in[2];
        pw4[3] = in[3];
        return true;
    } else {
        return false;
    }
    
}

int main(void)
{
    // * INITIALIZE * //
    SYSTEMConfigPerformance(10000000);
    init_timer_1();
    init_timer_2();
    init_lcd();
    init_keypad();
    clear_lcd();
    enableInterrupts();
    state = scan;
    //----------------//
    
    char * enter_s      = "ENTER:";
    char * set_s        = "SET:";
    char * good_s       = "GOOD";
    char * bad_s        = "BAD";
    char * valid_s      = "VALID";
    char * invalid_s    = "INVALID";
    char * mem_full     = "MEMORY FULL";
    
    char * display_s    = "ENTER:";
    char * message_s    = "MESSAGE";
    
    bool in_set_state = false;
    
    while(1)
    {
        switch(state){
            case wait:
                move_cursor_lcd(0,1);
                print_string_lcd(display_s);
            break;
            case scan:
                move_cursor_lcd(0,1);
                print_string_lcd(display_s);
                if(key_cnt == 4) key_cnt = 0;
                key = scan_keypad();
                if(key_cnt == 0 && key == '*')
                {
                    clear_lcd();
                    reset_pw();
                    key = '\0';
                    key_cnt = -1;
                    display_s = "SET:    ";
                    in_set_state = true;
                }   
                else if(key == '*' || key == '#') 
                {
                    clear_lcd();
                    reset_pw();
                    if(in_set_state == true) message_s = invalid_s;
                    else message_s = bad_s;
                    state = message;
                }
            break;
            case enter:
                in_set_state = false;
                display_s = enter_s;
                temp_pw[key_cnt] = key;
                move_cursor_lcd(0, 2);
                print_string_lcd(temp_pw);
                key_cnt++;
                if(key_cnt == 4){
                    if (check_pw(temp_pw) == true) {
                        message_s = good_s;
                    } else {
                        message_s = bad_s;
                    }
                    state = message;
                } else {
                    state = scan;
                }
            break;
            case message:
                delay_sec();
                clear_lcd();
                move_cursor_lcd(0,1);
                print_string_lcd(message_s);
                delay_sec();
                clear_lcd();
                reset_pw();
                in_set_state = false;
                display_s    = "ENTER:";
                state = scan;
            break;
            case set:
                in_set_state = true;
                display_s = set_s;
                temp_pw[key_cnt] = key;
                move_cursor_lcd(0, 2);
                print_string_lcd(temp_pw);
                key_cnt++;
                if(key_cnt == 4){
                    if (add_pw(temp_pw) == true) {
                        message_s = valid_s;
                    } else {
                        message_s = mem_full;
                    }
                    state = message;
                } else {
                    state = scan;
                }
            break;
            case debounce_press:
                delay_us(5000);
                state = wait;
			break;
            case debounce_release:
                delay_us(5000);
                if(in_set_state == true) state = set;
                else state = enter;
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
