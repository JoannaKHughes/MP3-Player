/*
 * keypad.h
 *
 *  Created on:  July 21, 2019
 *  Modified on: Oct 2, 2019 
 *  Modified on: Nov 18, 2020 
 *      Author: khughes
 */

#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <stdint.h>
#include <stdbool.h>

// Define the three keypad lookups here.  To declare yours, go into the
// properties for the lab04 project and under Build->ARM Compiler->
// Predefined Symbols, add the macro name in "Pre-define NAME".

const uint8_t keymap[4][4] = {
#ifdef KEYPAD_ALPHA
  {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'E','0','F','D'},
#endif
#ifdef KEYPAD_PHONE
  {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'*','0','#','D'},
#endif
#ifdef KEYPAD_ABT
 {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'SEQ','0','TKO','D'}
#endif
};

void initKeypad( const uint8_t table [4][4] );
bool getKey( uint8_t *col, uint8_t *row );

#endif //  __KEYPAD_H__

