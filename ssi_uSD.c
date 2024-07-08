/*
 * ssi_uSD.c
 *
 *  Created on:  Aug 8, 2018
 *  Modified on: Oct 7, 2019
 *  Modified on: Aug 1, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL, GPIO, and QSSI definitions.
#include "osc.h"
#include "sysctl.h"
#include "gpio.h"
#include "qssi.h"

// Slow bus speed = 400 KHz
#define  SLOW_SPEED        400000

// Fast bus speed = 12.5 MHz
#define  FAST_SPEED      12500000

// Prescaler
#define  PRESCALER       4;

// Constants for setting slow and fast speeds
#define SLOW_BAUD ((MAINOSCFREQ/SLOW_SPEED/PRESCALAR)-1)
#define FAST_BAUD ((MAINOSCFREQ/FAST_SPEED/PRESCALAR)-1)

// Initializes the QSSI3 module on PORTQ for the MicroSD card.
void initSSI3( void ) {
  // Enable PORTQ and QSSI3 gate clocks.
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTQ;
  SYSCTL[SYSCTL_RCGCSSI] |= SYSCTL_RCGCSSI_SSI3;

  // Set alternate function on PORTQ
  GPIO_PORTQ[GPIO_AFSEL] |=  GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_PORTQ[GPIO_PCTL]  &= ~(0xf << 0) & ~(0xf << 8) & ~(0xf << 12);
  GPIO_PORTQ[GPIO_PCTL]  |= (14 << 0) | (14 << 8) | (14 << 12);
  GPIO_PORTQ[GPIO_DEN]   |= GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;
  // Enable digital I/O for PQ1 on PORTQ
  GPIO_PORTQ[GPIO_DEN]   |= GPIO_PIN_1;
  GPIO_PORTQ[GPIO_DIR]   |= GPIO_PIN_1;
  GPIO_PORTQ[GPIO_PIN_1]   |= GPIO_PIN_1;

  // Disable SSI3
  QSSI3[QSSI_SSICR1] &= ~QSSI_CR1_SSE;

  // Set master mode
  QSSI3[QSSI_SSICR1] &= ~QSSI_CR1_MS;

  // Set prescaler4
  QSSI3[QSSI_SSICPSR] = PRESCALER;

  // Set CR0 for Freescale frame format, Mode 0 (clock normally low, data 
  //     captured on first clock edge), 8-bit data
  QSSI3[QSSI_SSICR0] &= ~QSSI_CR0_FRF_M;
  QSSI3[QSSI_SSICR0] |= QSSI_CR0_FRF_MOTO;
  //set mode
  //mask and set data
  QSSI3[QSSI_SSICR0] &= ~QSSI_CR0_DSS_M;
  QSSI3[QSSI_SSICR0] |= QSSI_CR0_DSS_8;

  //mask speed and set to slow
  QSSI3[QSSI_SSICR0] &= ~QSSI_CR0_SCR_M;
  QSSI3[QSSI_SSICR0] |= (SLOW_SPEED << QSSI_CR0_SCR_S);

  // Enable SSI3
  QSSI3[QSSI_SSICR1] |= QSSI_CR1_SSE;
}

// Sets the "baud rate" divisor to generate either a slow speed (~400KHz) 
// or high speed (~12.5MHz). If fast is false then low speed is configured; 
// this is used during card initialization.
void setSSI3Speed( bool fast ) {
  // Clear the current baud rate divisor value
  QSSI3[QSSI_SSICR0] &= ~QSSI_CR0_SCR_M;

  switch( fast ) {
  case false:
    QSSI3[QSSI_SSICR0] |= (SLOW_SPEED << QSSI_CR0_SCR_S);// 400KHz speed
    break;
  default:
    QSSI3[QSSI_SSICR0] |= (FAST_SPEED << QSSI_CR0_SCR_S);// 12.5MHz speed
    break;
  }
}

// Transmit the byte data to the slave over QSSI3.
void txByteSSI3( uint8_t data ) {
  // wait until TX FIFO is not full
    while((QSSI3[QSSI_SR] & QSSI_SR_TNF) == 0);
  // write data to TX FIFO
    QSSI3[QSSI_DR] = data;
  // wait until RX FIFO is not empty
    while((QSSI3[QSSI_SR] & QSSI_SR_RNE) == 0);
  // read value from RX FIFO and discard
    //uint8_t temp = data;
    uint8_t temp = QSSI3[QSSI_DR];
}

// Receive a byte from the slave over QSSI3.
uint8_t rxByteSSI3( void ) {
  // wait until TX FIFO is not full
    while((QSSI3[QSSI_SR] & QSSI_SR_TNF) == 0);
  // write to TX FIFO
    QSSI3[QSSI_DR] = 0xff;
  // wait until RX FIFO is not empty
    while((QSSI3[QSSI_SR] & QSSI_SR_RNE) == 0);
  // read value from RX FIFO
    //uint8_t temp = 20;
    //temp = QSSI3[QSSI_DR];
    return QSSI3[QSSI_DR];
}

// Set the chip select output level on PQ1. If on is true the pin is set 
// low. The pin must not be changed when QSSI3 is busy.
void assertCS( bool level ) {
    while((QSSI3[QSSI_SR] & QSSI_SR_BSY) == QSSI_SR_BSY);
    if(level) GPIO_PORTQ[GPIO_PIN_1] &= ~GPIO_PIN_1;
    else GPIO_PORTQ[GPIO_PIN_1] |= GPIO_PIN_1;
}
