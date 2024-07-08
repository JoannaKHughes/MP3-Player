/*
 * keypad.c
 *
 *  Created on:  July 21, 2019
 *  Modified on: Aug 4, 2020
 *      Author: khughes
 *
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL and GPIO definitions.
#include "ppb.h"
#include "sysctl.h"
#include "gpio.h"

// Prototypes for peripherals.
#include "osc.h"

// Initialize the specified row and column ports
void initKeypad(void) {
  
  // enable port clocks
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTC;
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTC;
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTH;
    SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTH;

  // enable row pins as outputs without pull-ups and with open-drain
    GPIO_PORTH[GPIO_ODR] |= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;//open drain
    GPIO_PORTH[GPIO_DIR] |= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;//outputs
    GPIO_PORTH[GPIO_PUR] &= ~GPIO_PIN_0 & ~GPIO_PIN_1 & ~GPIO_PIN_2 & ~GPIO_PIN_3;//not pull up
    GPIO_PORTH[GPIO_DEN] |= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;//enable

    GPIO_PORTH[GPIO_PIN_0] |= GPIO_PIN_0;
    GPIO_PORTH[GPIO_PIN_1] |= GPIO_PIN_1;
    GPIO_PORTH[GPIO_PIN_2] |= GPIO_PIN_2;
    GPIO_PORTH[GPIO_PIN_3] |= GPIO_PIN_3;


  // enable col pins as inputs with pull-ups and no open-drain
    GPIO_PORTC[GPIO_DIR] &= ~GPIO_PIN_4 & ~GPIO_PIN_5 & ~GPIO_PIN_6 & ~GPIO_PIN_7;//outputs
    GPIO_PORTC[GPIO_PUR] |= GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;//pull up
    GPIO_PORTC[GPIO_ODR] &= ~GPIO_PIN_4 & ~GPIO_PIN_5 & ~GPIO_PIN_6 & ~GPIO_PIN_7;//not open drain
    GPIO_PORTC[GPIO_DEN] |= GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;//enable

}

uint8_t rowCheck = 0;

// Check for a key in a row.  If a key is pressed, the procedure returns true and
// the row and column are set.  If a key is not pressed, the procedure returns
// false and row and columns don't matter.
bool getKey( uint8_t *col, uint8_t *row ) {
  // Is anything low in the row?
  // if no, change to the next row and return false
   GPIO_PORTH[1<<rowCheck] &= ~(1<<rowCheck);

  // If yes, now figure out which column it is and return true
    if(GPIO_PORTC[GPIO_PIN_4] == 0){
        *row = rowCheck;
        *col = 0;
        GPIO_PORTH[GPIO_PIN_0] |= (1<<rowCheck);
        return true;
    }
    else if(GPIO_PORTC[GPIO_PIN_5] == 0){
        *row = rowCheck;
        *col = 1;
        GPIO_PORTH[1<<rowCheck] |= (1<<rowCheck);
        return true;
        }
    else if(GPIO_PORTC[GPIO_PIN_6] == 0){
        *row = rowCheck;
        *col = 2;
        GPIO_PORTH[1<<rowCheck] |= (1<<rowCheck);
        return true;
        }
    else if(GPIO_PORTC[GPIO_PIN_7] == 0){
        *row = rowCheck;
        *col = 3;
        GPIO_PORTH[1<<rowCheck] |= (1<<rowCheck);
        return true;
        }
    else {
        GPIO_PORTH[1<<rowCheck] |= 1<<rowCheck;
        if(rowCheck == 3) rowCheck = 0;
        else rowCheck++;
        return false;
    }
}
