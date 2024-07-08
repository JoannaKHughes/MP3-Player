/*
 * lookup_c.c
 *
 *  Created on: Sep 28, 2022
 *      Author: j hughes
 */
#include <stdint.h>
#include <stdbool.h>

#include <sysctl.h>
#include <gpio.h>

//parameters a row index and a column index, the size of a 2D array, and a pointer to the array

uint8_t lookup(uint8_t row, uint8_t col, uint8_t size, uint8_t *table){
    if((row*col) > (size*size)) return 0;

    return table[row*size+col];


}


