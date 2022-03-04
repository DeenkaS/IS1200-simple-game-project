#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getsw(void){

    volatile int check = (PORTD >> 8) & 0xf;
    return check;
}

int getbtns(void){
    volatile int check = (PORTD >> 5) & 0x7;
    return check;
}

