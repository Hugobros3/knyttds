/*
Copyright (c) 2009 Rodrigo Roman and Ramon Roman (rrc2soft)
(Original Game "Knytt Stories" copyright (c) Nicklas "Nifflas" Nygren)

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of "rrc2soft" nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Music Engine, manages the music (& sounds) of our game */
#ifndef _DS_MUSIC_
#define _DS_MUSIC_

/* Initializes the music engine */
int ds_music_init();   

/* Resets the music engine */
int ds_music_reset();

/* Tell the system to play ONLY a certain music */
int ds_music_playOnlyMusic(int m);
 
/* Tell the system to play a certain music and ambiance (both are usually together!) 
	If onlymp3 is ON, the only music that is added is the mp3 sound. */
int ds_music_playMusicAndAmbiance(int m, int a1, int a2, int onlymp3);

/* Tell the music subsystem that he must play a normal sound */
int ds_music_playSound(char *raw, int loop, int prio);

/* Tell the music subsystem that he must play a normal sound in a specific channel */
int ds_music_playSoundChannel(char *raw, int ch, int loop, int prio);

/* Stops a certain channel */
int ds_music_stopSoundChannel(int ch);

/* Manages some aspects related to the music */
void ds_music_manage();

#endif
