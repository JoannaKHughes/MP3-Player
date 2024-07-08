/*
 * control.c
 *
 *  Created on:  Aug 13, 2018
 *  Modified on: May 29, 2020
 *      Author: khughes
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "control.h"
#include "lcd.h"
#include <time.h>

// The total number of songs on the MicroSD card.
static uint8_t numSongs;
// The next song to play.
static uint8_t song = 0;
// Indicates termination of current song by user.
static bool done = false;
// Indicates whether decoding/playing is playing or paused.
static bool paused = false;
// Indicates if the MP3 player is in song shuffle mode.
static bool shuffle = false;
// Indicates if MP3 is doing previous song
static bool prev = false;
static bool initial = false;
//time user inputs as amount of time to play songs
static uint8_t timeDuration;
static bool timerM = false;

static bool timeron = false;
clock_t start, end, duration;
//sets favorite mode
static bool favor = false;
static bool FavorMode = false;
//array to hold shuffled songs for prev, n refers to location
static uint8_t array[30];
static uint8_t farray[30];
static uint8_t favorites[30];
uint32_t n = 0;
uint32_t f = 0;
uint32_t fmax = 0;
uint32_t frand = 0;
int info;

void pusharr(uint8_t *array, int n,uint8_t song){
    array[n]=song;
}

uint8_t poparr(uint8_t *array, int n){
    song=array[n];
    return song;
}

void getPrevious(uint8_t v){
    song = v - 1;
      if( song <= 0 ) {
        song = 0;
      }
      prev=false;
}


// Return the number of the song to play.  Initially, just
// return the next sequential song, and wrap back to 0 when all
// songs have been returned.
uint8_t getSong( void ) {

  // On the first call, always return the first song.
  if( initial == false ) {
    initial = true;
    return song = 0;
  }
  if(timeron == true){
  if(timerM == true){
    timerM = timerMode();//check if time is over
    if(timerM == false){
        timeron = false;
      setPaused( isPaused() == true );//pause music and turn off screen if timer ran out
      clearLCD();
    }
  }
  }

  // Otherwise pick the next song to play.
  if (favor==true){
  fmax=fmax+1;
  favorites[fmax]=song;
  }
  if (FavorMode==false){
  if( shuffle == false  && prev == false) {
    song = ( song + 1 ) % numSongs;
  }
  else if(shuffle ==true && prev == false) {
     song = rand()%30;
     pusharr(array, n, song);
     n=n+1;
    //add shuffled to stack
  }

  else if (prev == true && shuffle == false ){
    //getPrevious(song);
      song = song - 1;
           if( song <= 0 ) {
             song = 0;
           }
           prev=false;
  }
  else if (prev == true && shuffle == true ){
      n=n-1;
      poparr(array, n);
      prev=false;
  }
  // Return song number.

}
  else if (FavorMode==true){

      if( shuffle == false  && prev == false) {
          song = ( favorites[f] + 1 ) % numSongs;
        }
        else if(shuffle ==true && prev == false) {
           song = rand()%fmax;
           pusharr(farray, frand, song);
           frand=frand+1;
          //add shuffled to stack
        }

        else if (prev == true && shuffle == false ){
            song = (favorites[f-1]);
        }
        else if (prev == true && shuffle == true ){
            frand=frand-1;
            poparr(farray, frand);
        }
        // Return song number.
      }
  return song;
  }
// Store the total number of songs (needed by getSong()).
void setNumSongs( uint8_t num ) {
  numSongs = num;
}

// Store the total number of songs (needed by getSong()).
uint8_t getNumSongs( void ) {
  return numSongs;
}

// Set next song explicitly.  Since getSongs() will advance the song by 1 in
// sequential mode after the first call, "decrement" the value before 
// storing.
void setSong( uint8_t v ) {
  song = v - 1;
  if( song >= numSongs ) {
    song = numSongs - 1;
  }

}

bool isPrevious ( void ){
    return prev;
}

void setPrevious (bool v){
    prev=v;
}

// add Favorites.
void setFavorite (bool v){
     favor=v;
}

// Favorites mode
void setFavoriteMode (bool v){
     FavorMode=v;
}
// Indicates whether the current MP3 song should be terminated early.
bool isDone( void ) {
  bool tmp = done;
  done = false;
  return tmp;
}

// Set song to terminate.
void setDone( void ) {
  done = true;
}

// Indicates whether the MP3 player is paused.
bool isPaused( void ) {
  return paused;
}

// Set state of pause.
void setPaused( bool v ) {
  paused = v;
}

// Indicates state of shuffle mode.
bool isShuffle( void ) {
  return shuffle;
}

// Set state of shuffle mode.
void setShuffle( bool v ) {
  shuffle = v;
}

//sets time to play
void setTime(uint8_t timeSet) {
    timeDuration = timeSet;
}

//plays music for set amount of time, if bool timerOn is true, timed mode is on -> when times runs out it is set to false
bool timerMode(void) {
    end = clock();
    duration = (end - start);
    if (duration = timeDuration) return false;
    else return true;
}


void setTimerMode(bool timerm, uint8_t minTime, clock_t startTime) {//input in minutes, convert to seconds
  timerM = timerm;
  timeDuration = minTime*60;
  start = startTime;
}

void offTimerMode(void){
  timerM = false;
}


void Info (void){
   if (info==0){
    positionLCD (3,0);
    stringLCD("Song # ");
   // uint8_t c[]= song+"0";
   // stringLCD(c);
    info=1;
   }
   else if (info==1){
       clearLine3();
       if(shuffle == true){
           positionLCD (3,0);
         stringLCD("Shuffle: ON");
         info=2;
       }
         else{
         positionLCD (3,0);
         stringLCD("Shuffle: OFF");
            info=2;
         }
   }
   else if (info==2){
       clearLine3();
       if(FavorMode==true){
           positionLCD (3,0);
           stringLCD("Playlist: Favorite");
           info=3;
       }
       else{
           positionLCD (3,0);
          stringLCD("Playlist: Normal");
           info=3;
       }
   }
   else{
       clearLine3();
       info=0;
   }
}


