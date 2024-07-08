/*
 * led7seg.c
 *
 *  Created on: July 29, 2018
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL and GPIO definitions.
#include "sysctl.h"
#include "gpio.h"

// Table containing data to display characters on LED.  Assumes segment A is
// MSB, segment N is LSB.  "0b" prefix used to define value in binary
static uint8_t displayData[] = {//NJFEDCBA
  0b00111111,           // '0'
  0b00000110,           // '1'
  0b11011011,            // '2'
  0b11001111,            // '3'
  0b11100110,            // '4'
  0b11101101,            //'5'
  0b11111101,            //'6'
  0b00000111,              //'7'
  0b11111111,              //'8'
  0b11101111,               //'9'
  0b11110111,           // 'A'
  0b11111100,           // 'b'
  0b11111001,           // 'c'
  0b11011110,           // 'd'
  0b11111001,           // 'E'
  0b11110001,           // 'F'
};

uint8_t getDisplayDataSize( void ) {
  // Return the number of items in the displayData array.  Remember, this
  // must be calculated, not hard-coded.
    int size = *(&displayData + 1) - displayData;
    return size;
}

void init7SegLED( void ) {
  // Enable PORTD and PORTE gate clocks.  The second write is only necessary
  // as a delay so that the clock is stable before writing to the GPIO
  // registers.  If we don't delay, we may get a fault.
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTD;
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTE;

  // To use PD7, the commit register must be written.  Before doing this,
  // a special code must be written to the lock register to enable this.
  // See the datasheet page on GPIOLOCK for details.
  GPIO_PORTD[GPIO_LOCK] = 0x4c4f434b;
  GPIO_PORTD[GPIO_CR] = GPIO_ALLPINS;
  GPIO_PORTD[GPIO_LOCK] = 0;

  // Configure output pins on PORTD and PORTE.
  GPIO_PORTD[GPIO_DIR] |= GPIO_ALLPINS;
 GPIO_PORTE[GPIO_DIR] |= (GPIO_PIN_0 | GPIO_PIN_1);

  // Enable pins on both ports.
  GPIO_PORTD[GPIO_DEN] |= GPIO_ALLPINS;
  GPIO_PORTE[GPIO_DEN] |= (GPIO_PIN_0 | GPIO_PIN_1);
}

void displayVal(uint8_t display, uint8_t value) {
  // Activate and deactivate the two transistors to turn on only one display.
    if(display){
        GPIO_PORTE[GPIO_PIN_0] = 0;
        GPIO_PORTE[GPIO_PIN_1] = GPIO_PIN_1;
    }
    else{
        GPIO_PORTE[GPIO_PIN_1] = 0;
        GPIO_PORTE[GPIO_PIN_0] = GPIO_PIN_0;
    }

  // Write data to the active LED display.
  GPIO_PORTD[GPIO_ALLPINS] = displayData[value];
}
