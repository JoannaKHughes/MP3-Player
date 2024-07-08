# MP3-Player
MP3 Player designed on TI Dev board and configured in C
Completed in Fall of 2022

Volume control (all teams). Increase and decrease the output volume.  The volume information is accessible through upVolume()/downVolume() (in sound.c) or getVolume()/setVolume() (in the supplied sound.obj). Note that the volume's range is 0 to 32.
Play previous song (all teams). While a song is playing, end the song and start playing the last song played. This works for up to three previous songs.
Select song using numeric entry (team choice). While in PAUSE mode, the numeric keys are used to enter the number of a song on the card (1 through the number of songs on the card). The numbers keys are “accumulated” into a value, with the “E” key acting as an “Enter” key and the “C” key acting as a “Clear” key. For example, when the combination “1” → “3” → “E” is pressed, the thirteenth song (song number 12) will start playing. Typing “E” without entering any value, or entering an invalid song number, switches back to PLAY mode.
Shuffle mode (team choice). The default mode is to play songs sequentially, starting at song 0, then song 1, then song 2, etc., through song n-1, then repeat at song 0. In shuffle mode, the next song is selected at random. To help with this, the C library functions srand() and rand() are available in stdlib.h. Note that using shuffle will likely have an effect on the implementation of Play previous song and Play next song.
Display elapsed play time (team choice).  While a song is playing, show in real time the minutes and seconds since play started.
Display additional status information (team choice).  While a song is playing, show at least two other pieces of information such as volume level, PLAY/PAUSE status, current song number, etc.
Favorites playlist add and remove songs to your favorites playlist, choose to play only your songs or the entire music folder
Timed mode, play your music for 5 or 10 minutes before it automatically stops
