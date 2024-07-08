/*
 * UI.c
 *
 *  Created on:  Nov 1, 2018
 *  Modified on: Aug 9, 2019
 *  Modified on: Oct 27, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Peripherals
#include "osc.h"
#include "gptm.h"
#include "gpio.h"
#include "ppb.h"
#include "sysctl.h"
#include "keypad.h"
#include "lookup.h"
#include "ID3.h" // add
// Various hooks for MP3 player
#include "control.h"
#include "sound.h"
#include "UI.h"
uint32_t mode = 0;
uint32_t shuff =0;
// Definitions for function keys

struct id3tags tags; // needed for the tags

enum keycmds_t {
  PLAY_PAUSE    = 'A',
  SHUFFLE       = 'B',
  VOLUME_UP     = 'C',
  VOLUME_DOWN   = 'D',
  SKIP_BACKWARD = '*',
  SKIP_FORWARD  = '#',
  FAVORITE      = '1',
  FAVORITE_MODE = '2',
  INFO_MODE     = '3',
  TIMED_MODE_1  = '4',
  TIMED_MODE_10 = '5',
  TIMED_MODE_30 = '6',
  TIMED_MODE_OFF= '7',
  SCROLL_ALBUM = '0' //add
};

static bool scrollAlbum = true; //add

// Return key value upon initial key press.  Since keys are
// ASCII, they cannot have a value greater than 0xff, so when
// no new key is pressed the procedure returns UINT16_MAX.
static uint16_t UIKey( void ) { 
  static enum {NOT_PRESSED, PRESSED} state = NOT_PRESSED;
  uint8_t column, row, key;

#define ADDR keymap
#define SIZE 4

  switch( state ) {
  case NOT_PRESSED:
    if( getKey( &column, &row ) == true ) {
      key = lookup(row,column,SIZE,*ADDR);
      state = PRESSED;
      return (uint16_t)key;
    }
    break;
  case PRESSED:
    if( getKey( &column, &row ) == false )
      state = NOT_PRESSED;
    break;
  }
  return UINT16_MAX;
}

// Interrupt handler for user interface.  When called it scans the
// keypad and if a new key is detected, performs necessary actions.
void UIHandler( void ) {
  uint16_t key = UIKey( ); 

  if( key != UINT16_MAX ) {
    switch( (enum keycmds_t)key ) {
    case PLAY_PAUSE:    // 'A'
        setPaused(!isPaused() );
     // setPaused( isPaused() == true );
      break;
    case SHUFFLE:       // 'B'
        if (shuff==0){
            shuff=1;
      setShuffle( true );
        }
        else {
            shuff=0;
            setShuffle(false);
        }
      break;
    case VOLUME_UP:     // 'C'works
      upVolume();
      break;
    case VOLUME_DOWN:   // 'D'works
      downVolume();
      break;
    case SKIP_BACKWARD:// '*'
        setPrevious(true);
        setDone();
                break;
    case SKIP_FORWARD:  // '#'works
      setDone();
      break;
   case FAVORITE:      // '1'
        setFavorite (true);
        break;
    case FAVORITE_MODE: //'2'
        if (mode==0){
                 mode=1;
             setFavoriteMode(true);
             setDone();
             }
             else if (mode==1){
                 mode=0;
                 setFavoriteMode(false);
               setDone();
             }
        break;

    case INFO_MODE:       // '3'
      Info();
      break;

   case TIMED_MODE_1:       // '4'
      clock_t startTime = clock();
      setTimerMode(true, 1, startTime);
      //printf(startTime);
      break;
      
    case TIMED_MODE_10:       // '5'
      startTime = clock();
      setTimerMode(true, 10, startTime);
      break;

    case TIMED_MODE_30:       // '6'
       startTime = clock();
      setTimerMode(true, 30, startTime);
      break;

    case TIMED_MODE_OFF:       // '7'
      offTimerMode();
      break;
      
    case SCROLL_ALBUM: //'0' //add
        if (scrollAlbum){
            scrollAlbum = false;
            displayInfo(&tags,20);
        }

        else
        {
            scrollAlbum = true;
            displayInfo(&tags,0);
        }
      break;

    default:            // Numeric keys
      break;
    }
  }

  // Clear the time-out.
  GPTM_TIMER5[GPTM_ICR] |= GPTM_ICR_TATOCINT;
}

//#define KEYPAD_ALPHA
//#define KEYPAD_ABT
#define PHONE


static const uint8_t info[4][4] = {

#ifdef PHONE
  {'1','2','3','A'},
   {'4','5','6','B'},
   {'7','8','9','C'},
   {'*','0','#','D'},
#endif

};

void initUI( void ) {
  // Prep the keypad.
  initKeypad(info);

  // Enable Run Clock Gate Control
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER5;
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER5;

  // 32-bit periodic timer.
  GPTM_TIMER5[GPTM_CFG] &= ~GPTM_CFG_M;
  GPTM_TIMER5[GPTM_CFG] |= GPTM_CFG_32BITTIMER;
  GPTM_TIMER5[GPTM_TAMR] &= ~GPTM_TAMR_TAMR_M;
  GPTM_TIMER5[GPTM_TAMR] |= GPTM_TAMR_TAMR_PERIODIC;

  // Set reload value for 20Hz
  GPTM_TIMER5[GPTM_TAILR] = (MAINOSCFREQ/20)-1;

  // Enable the interrupt.
  GPTM_TIMER5[GPTM_IMR] |= GPTM_IMR_TATOIM;

  // Enable interrupt for timer.
  PPB[PPB_EN2] |= PPB_EN2_TIMER5A;

  // Set priority level to 1 (lower priority than Timer2A).
  PPB[PPB_PRI16] = ( PPB[PPB_PRI16] & PPB_PRI_INTB_M ) | ( 1 << PPB_PRI_INTB_S );

  // Clear the time-out.
  GPTM_TIMER5[GPTM_ICR] |= GPTM_ICR_TATOCINT;

  // Enable the timer.
  GPTM_TIMER5[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;
}

void displayInfo(struct id3tags * tags, uint8_t x){ //displays the songs for the album
    clearLCD();
    positionLCD(0,-x);
    stringLCD(tags->album);
}





