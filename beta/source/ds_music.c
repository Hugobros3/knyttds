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

/* How to convert music:
   
   - Launch Audacity.
   - Tracks -> Stereo to Mono
   - Tracks -> Resample (22050)
   - Project Rate (22050)
   - Export -> Mp3 -> Bit Rate (Constant), Quality (48 kbps), Channel Mode (Stereo)
   - (Edit Metadata - OK)
   
*/

/* How to convert audio:
   
   - Launch Audacity.
   - Tracks -> Stereo to Mono
   - Tracks -> Resample (11025)
   - Project Rate (11025)
   - Export -> RAW (8 bit signed, 11025, mono)
   - (Edit Metadata - OK)
   
*/

/* 
	- Check this error!!!!!!!!!!!!
	AS_MP3Stop is broken: It doesn't set mp3file to NULL, so FILE_CLOSE is called twice 
	on the same file handle if you stop and then play another mp3.	
*/

// Includes
#include <PA9.h>
#include "ds_util_bit.h"
#include "ds_global.h"
#include "ds_util.h"

//-------------------------------------------------------------------------------------------------
// INTERNAL TYPES AND VARIABLES
//-------------------------------------------------------------------------------------------------

// Volume and others
#define DS_C_MUSIC_MUSVOL 112
#define DS_C_MUSIC_AMBVOL 112
#define DS_C_MUSIC_KNYTTCH 3
#define DS_C_MUSIC_SOUNDCH 3
#define DS_C_MUSIC_MP3CH 1
#define DS_C_MUSIC_RAWCH 2

// Music Format
#define DS_C_MUSIC_MP3 0
#define DS_C_MUSIC_RAW 1

// Music Type
#define DS_C_MUSIC_MUS 0
#define DS_C_MUSIC_AMB 1

typedef struct ds_tt_musicSlot {
   int number;
   char dir[255];
   int type;
   int format;
} ds_t_musicSlot;  

typedef struct ds_tt_musicList {
   ds_t_musicSlot music[DS_C_MUSIC_KNYTTCH];
   int musicOn[DS_C_MUSIC_KNYTTCH];
   int len;
   int maxmp3;
} ds_t_musicList;  

typedef struct ds_tt_channel {
   ds_t_musicSlot slot;
   u8 *rawContent;
   u32 rawSize;
   int phyChannel;
   int mark;
} ds_t_channel;  

typedef struct ds_tt_channelList {
   ds_t_channel cMP3[DS_C_MUSIC_MP3CH];
   ds_t_channel cRAW[DS_C_MUSIC_RAWCH];
} ds_t_channelList;   

ds_t_musicList _musicList;
ds_t_channelList _channelList;

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS - SLOT & LIST
//-------------------------------------------------------------------------------------------------

void _ds_music_slotInit(ds_t_musicSlot *slot) {
   slot->number = 0; // Saying just this is OK -> no music assigned
}   

int _ds_music_getDataSub(ds_t_musicSlot *slot, char *DIR_M, char *FILE_M, int onlymp3) {
   sprintf(slot->dir,"%s%s/%s%d.mp3",ds_global_world.dir,DIR_M,FILE_M,slot->number);
	if (ds_util_fileExists(slot->dir)) {
	   slot->format = DS_C_MUSIC_MP3;
	   return 1;
	}	
	sprintf(slot->dir,"%s%s/%s%d.mp3",DS_DIR_MAIN,DIR_M,FILE_M,slot->number);
	if (ds_util_fileExists(slot->dir)) {
	   slot->format = DS_C_MUSIC_MP3;
	   return 1;
	}	
   sprintf(slot->dir,"%s%s/%s%d.raw",ds_global_world.dir,DIR_M,FILE_M,slot->number);
	if ((ds_util_fileExists(slot->dir)) && (!onlymp3)) {
	   slot->format = DS_C_MUSIC_RAW;
	   return 1;
	}	
	sprintf(slot->dir,"%s%s/%s%d.raw",DS_DIR_MAIN,DIR_M,FILE_M,slot->number);
	if ((ds_util_fileExists(slot->dir)) && (!onlymp3)) {
	   slot->format = DS_C_MUSIC_RAW;
	   return 1;
	}	

	/*
	if (slot->type == DS_C_MUSIC_MUS) {
  	   slot->format = DS_C_MUSIC_MP3;
	   return 1;
	} else {
  	   slot->format = DS_C_MUSIC_RAW;
	   return 1;
	} */     
	
	return 0; // No file!!!!!! (Is OK, we just don't play music)
}   

int _ds_music_getData(int m,ds_t_musicSlot *slot,int type, int onlymp3) {
   // First, inits slot
   _ds_music_slotInit(slot);
   
   // Music = 0? No music should be played (exists)
   if (m == 0)
   	return 0;
   slot->number = m;
   slot->type = type;
   	
   // OK, now we get the dir and type of music. MP3? RAW? KNYTT directory? Game directory?
   if (slot->type == DS_C_MUSIC_MUS)
   	return _ds_music_getDataSub(slot, DS_DIR_MUSIC, "Song", onlymp3);
   else
		return _ds_music_getDataSub(slot, DS_DIR_AMBIANCE, "Ambi", onlymp3);   
}   

ds_t_musicList *_ds_music_listInit() {
   int i;
   for (i=0; i < DS_C_MUSIC_KNYTTCH; i++) {
   	_ds_music_slotInit(&_musicList.music[i]);
   	_musicList.musicOn[i] = 0;
	}   
   _musicList.len = 0;
   _musicList.maxmp3 = DS_C_MUSIC_MP3CH;
   
   return &_musicList;
}

void _ds_music_listEmpty(ds_t_musicList *list) {
   // Nothing to do, this list is pointing to a local structure...
}

void _ds_music_listAdd(ds_t_musicList *list, ds_t_musicSlot *slot) {
   // Zero: If the mp3 channels are full, this music should not be admitted into the list.
   if ((slot->format == DS_C_MUSIC_MP3) && (list->maxmp3 == 0))
   	return; // We can only accept DS_C_MUSIC_MP3CH mp3 into the system
   
   // First, find an empty place
   int i = 0;
   while (i < DS_C_MUSIC_KNYTTCH) {
      if (list->musicOn[i] == 0)
      	break;
      i++; // Yes, I know everything can be done inside a for... but now i want clarity :-P
	}    
	if (i == DS_C_MUSIC_KNYTTCH)
		return;
		
	// Now, copy the contents in the new place
	list->music[i] = *slot;
	list->musicOn[i] = 1;
	list->len++;
	
	// Also, update the mp3 priorities
	if (list->music[i].format == DS_C_MUSIC_MP3)
		list->maxmp3--;
}      

int _ds_music_listLen(ds_t_musicList *list) {
   return list->len;
}

ds_t_musicSlot *_ds_music_listGet(ds_t_musicList *list, int ele) {
   int i = 0;
   int actele = -1;
   
   while (i < DS_C_MUSIC_KNYTTCH) {
      if (list->musicOn[i] == 1) {
         actele++; 
         if (actele == ele)
         	return &(list->music[i]);
      }   
      i++;
	}
	return NULL;
}

void _ds_music_listDel(ds_t_musicList *list, int ele) {
   int i = 0;
   int actele = -1;
   
   while (i < DS_C_MUSIC_KNYTTCH) {
      if (list->musicOn[i] == 1) {
         actele++; 
         if (actele == ele) {
				if (list->music[i].format == DS_C_MUSIC_MP3)
					list->maxmp3++;
   			_ds_music_slotInit(&(list->music[i]));
   			list->musicOn[i] = 0;
   			list->len--;
            return;
         }   
      }   
      i++;
	}
}

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS - CHANNELS
//-------------------------------------------------------------------------------------------------

// AS_SoundDefaultPlay((u8*)crash, (u32)crash_size, 127, 64, false, 0)
// AS_SoundDefaultPlay(u8 data,u32 size,u8 volume,u8 pan,u8 loop,u8 prio)

void _ds_music_channelInit(ds_t_channel *mChannel, int memCheck) {
   if (memCheck) {
      if (mChannel->rawContent != NULL)
      	free(mChannel->rawContent);
   }   
   mChannel->rawContent = NULL;
   mChannel->rawSize = 0;
   _ds_music_slotInit(&(mChannel->slot));
   mChannel->phyChannel = -1;
   //mChannel->mark = 0;
}

void _ds_music_channelStop(ds_t_channel *mChannel) {
   // Is there any sound in this channel?
   if (mChannel->slot.number == 0)
   	return;
   	
   // Stop the channel!
   switch (mChannel->slot.format) {
      case DS_C_MUSIC_MP3:
         // Stop an MP3...
         if (mChannel->rawContent != NULL)
         	AS_MP3Stop();
         break;
      case DS_C_MUSIC_RAW:
         // Stop a RAW file...
         if (mChannel->rawContent != NULL)
         	AS_SoundStop(mChannel->phyChannel);
		   break;   
   }  
	
	// Empty the channel...
	_ds_music_channelInit(mChannel, 1); 
} 

int _ds_music_channelLoad(ds_t_channel *mChannel) {
   // First, Load the contents from file
	FILE *file;
	file = fopen(mChannel->slot.dir,"rb");
   if (file == NULL) {
      // No file? Then, do not play music ;-)
      _ds_music_channelInit(mChannel, 1);
      //PA_OutputText(1,0,17,"Getting ...-Playing-NO FILE.  ");
      return 0;
   }
   // File size...
	fseek(file,0,SEEK_END);
	mChannel->rawSize = ftell(file);
	rewind(file);
	// Loading...
	mChannel->rawContent = (u8*) malloc (sizeof(u8)*(mChannel->rawSize));
	//mChannel->rawContent = (u8*) memalign (4,sizeof(u8)*(mChannel->rawSize));
	if (mChannel->rawContent == NULL) {
      // No memory? Then, do not play music ;-)
      _ds_music_channelInit(mChannel, 1);
      //PA_OutputText(1,0,17,"Getting ...-Playing-NO MEM.  ");
      fclose(file);
      return 0;	   	
	}
	fread((void *)mChannel->rawContent,1,mChannel->rawSize,file);
	DC_FlushAll();
	
	// Closing...
	fclose(file);   
	
	return 1;
}

int _ds_music_channelPlay(ds_t_channel *mChannel) {
   // Now, begins playing the music
   switch (mChannel->slot.format) {
      case DS_C_MUSIC_MP3:
         // Play an MP3...
         if (mChannel->rawContent != NULL) {
         	AS_MP3DirectPlay(mChannel->rawContent,mChannel->rawSize);
         	//AS_MP3StreamPlay(mChannel->slot.dir);
         	
				AS_SetMP3Volume(DS_C_MUSIC_MUSVOL);         	
         	if (mChannel->slot.type == DS_C_MUSIC_AMB)
         		AS_SetMP3Loop(1);
         	else 
         		AS_SetMP3Loop(0);
       	}  	
         break;
      case DS_C_MUSIC_RAW:
         // Play a RAW file...
         if (mChannel->rawContent != NULL) {
            int lp = (mChannel->slot.type == DS_C_MUSIC_AMB);
         	mChannel->phyChannel =
				  AS_SoundDefaultPlay(mChannel->rawContent,mChannel->rawSize,DS_C_MUSIC_AMBVOL,64,lp,0);
       	}  	
		   break;   
   }     
   return 1;
} 

void _ds_music_channelListInit() {
   int i;
   for (i=0; i<DS_C_MUSIC_MP3CH; i++) {
      _ds_music_channelInit(&(_channelList.cMP3[i]),0);
   }   
   for (i=0; i<DS_C_MUSIC_RAWCH; i++) {
		_ds_music_channelInit(&(_channelList.cRAW[i]),0);
   }   
}

ds_t_channel *_ds_music_channelListGetFree(ds_t_musicSlot *mSlot) {
   int i;
   switch (mSlot->format) {
      case DS_C_MUSIC_MP3:
         for (i=0; i<DS_C_MUSIC_MP3CH; i++) {
            if (_channelList.cMP3[i].slot.number == 0) 
				   return &(_channelList.cMP3[i]);
         }   
         break;
      case DS_C_MUSIC_RAW:
         for (i=0; i<DS_C_MUSIC_RAWCH; i++) {
            if (_channelList.cRAW[i].slot.number == 0)
				   return &(_channelList.cRAW[i]);
         }   
         break;
   }   
   return NULL;
}
   
void _ds_music_channelListAdd(ds_t_musicSlot *mSlot) {
	ds_t_channel *mChannel;
	//PA_OutputText(1,0,17,"Getting %d-            ",mSlot->number);
	mChannel = _ds_music_channelListGetFree(mSlot);
	if (mChannel == NULL)
		return;
	//PA_OutputText(1,0,17,"Getting %d-Playing-    ",mSlot->number);
	mChannel->slot = *mSlot;
	if (_ds_music_channelLoad(mChannel)) {
		if (_ds_music_channelPlay(mChannel)) {
			//	PA_OutputText(1,0,17,"Getting %d-Playing-OK. ",mSlot->number);
		}	
	}
}        

void _ds_music_channelListPurgeInit() {
   int i;
   
   for (i=0; i<DS_C_MUSIC_MP3CH; i++) {
      _channelList.cMP3[i].mark = 0;
   }   
   for (i=0; i<DS_C_MUSIC_RAWCH; i++) {
      _channelList.cRAW[i].mark = 0;
   }   
}   

int _ds_music_channelListExistMark(ds_t_musicSlot *mSlot) {
   int i;
   
   switch (mSlot->format) {
      case DS_C_MUSIC_MP3:
         for (i=0; i<DS_C_MUSIC_MP3CH; i++) {
            if ((_channelList.cMP3[i].slot.number == mSlot->number) &&
                (_channelList.cMP3[i].slot.type == mSlot->type)) 
				{
				   _channelList.cMP3[i].mark = 1;
				   return 1;
            }
         }   
         break;
      case DS_C_MUSIC_RAW:
         for (i=0; i<DS_C_MUSIC_RAWCH; i++) {
            if ((_channelList.cRAW[i].slot.number == mSlot->number) &&
                (_channelList.cRAW[i].slot.type == mSlot->type)) 
				{
				   _channelList.cRAW[i].mark = 1;
				   return 1;
            }
         }   
         break;
   }   
   return 0;
}   

void _ds_music_channelListPurgeDo() {
   int i;
   
   for (i=0; i<DS_C_MUSIC_MP3CH; i++) {
      if (!_channelList.cMP3[i].mark) {
		   _ds_music_channelStop(&(_channelList.cMP3[i]));
		}   
   }   
   for (i=0; i<DS_C_MUSIC_RAWCH; i++) {
      if (!_channelList.cRAW[i].mark) {
		   _ds_music_channelStop(&(_channelList.cRAW[i]));
		}   
   }   
}   

//-------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS - "RAW" CHANNELS
// Note: The "RAW" channels use a structure that is completely different
// 	from the normal Music/Ambiance structures. This is mainly due to a... glup... 
//  ...design error where I didn't thought from the beginning on the coexistence
//	between music/ambiance and raw sounds -_-.
//-------------------------------------------------------------------------------------------------


// Volume
#define DS_C_MUSIC_SNDVOL 127

// Raw Channels
#define DS_C_MUSIC_RAWCHMAX 20 
		// NOTE! The max. number of channels for the DS is 16, but I don't know where to 
		// get the constant value ;-). Then, why 20? I am sure it is safe to put 16,
		// but must check ASlib sources to see if there are situations when it does not returns
		// -1..15.
		// ... OK, I am a lazy coward :-).
#define DS_C_MUSIC_RAWBUFFMAX 15

typedef struct ds_tt_bufferRAW {
	char name[256];
	u8 *rawContent;
	u32 rawSize;
	int priority;   // The priority of this sound, linked to the "times used" mechanism.
} ds_t_bufferRAW;  

typedef struct ds_tt_channelRAW {
	int buffer;     // Which sound we are playing
	int phyChannel; // The physical channel that stores this sound (-1 if not playing)
} ds_t_channelRAW;  


ds_t_bufferRAW _rawBuffer[DS_C_MUSIC_RAWBUFFMAX];
ds_t_channelRAW _rawList[DS_C_MUSIC_RAWCHMAX];

int _rawBufferSize;

void _ds_music_rawInit() {
	int i;
	// Init the rawList - the list of channels and musics that are being played
	for (i = 0; i < DS_C_MUSIC_RAWCHMAX; i++) {
		_rawList[i].buffer = -1;
		_rawList[i].phyChannel = -1;
	}
	// Init the rawBuffer - the list that contains the music that is being played
	for (i = 0; i < DS_C_MUSIC_RAWBUFFMAX; i++) {
		_rawBuffer[i].rawContent = NULL;
		_rawBuffer[i].rawSize = 0;
		_rawBuffer[i].priority = 0;
		sprintf(_rawBuffer[i].name,"-");
	}
}

void _ds_music_rawReset() {
	int i;
	// Go channel by channel and stop every sound
	for (i = 0; i < DS_C_MUSIC_RAWCHMAX; i++) {
		if (_rawList[i].phyChannel != -1) {
			AS_SoundStop(_rawList[i].phyChannel);
		}
	}
	// Traverse the buffer and delete the sounds
	for (i = 0; i < DS_C_MUSIC_RAWBUFFMAX; i++) {
		if (_rawBuffer[i].rawContent != NULL) {
			free(_rawBuffer[i].rawContent);
			_rawBuffer[i].rawContent = NULL;
		}
	}	
	// Re-Inits everything
	_ds_music_rawInit();
}

int _ds_music_rawBufferLoad(char *raw, ds_t_bufferRAW *ele, int prio) {
	FILE *file;
	
	// Get real file
	sprintf(ds_global_string,"%s%s/%s.raw",DS_DIR_MAIN,DS_DIR_RAWMUSIC,raw);
	if (!ds_util_fileExists(ds_global_string)) {
		return -1;
	}	

	// Open file
	file = fopen(ds_global_string,"rb");
	if (file == NULL) {
	  // No file? Then, do not play ;-)
	  return -1;
	}
	// File size...
	fseek(file,0,SEEK_END);
	ele->rawSize = ftell(file);
	rewind(file);
	
	// Loading...
	ele->rawContent = (u8*) malloc (sizeof(u8)*(ele->rawSize));
	if (ele->rawContent == NULL) {
		// No memory? Then, do not play ;-)
		fclose(file);
		return -1;	   	
	}
	fread((void *)ele->rawContent,1,ele->rawSize,file);
	DC_FlushAll();
	
	sprintf(ele->name,"%s",raw);

	// Closing...
	fclose(file);   
	return 1;
}

int _ds_music_rawBufferReserve(char *raw, int prio) {
	int buff;
	ds_t_bufferRAW *ele;
	int i;
	
	// FIXME Add memory limit
	
	// First, search a buffer that contains the specific sound
	buff = -1;
	for (i = 0; i < DS_C_MUSIC_RAWBUFFMAX; i++) {
		if (_rawBuffer[i].rawContent != NULL) {
			if (PA_CompareText(_rawBuffer[i].name, raw)) {
				buff = i;
				_rawBuffer[buff].priority += (prio + 1);
				break;
			}
		}
	}	
	if (buff != -1) {
		// If sound is found, returns the index of the sound 
		return buff;
	}
	
	// If not, search a free buffer that will be used to store the new sound!
	buff = -1;
	for (i = 0; i < DS_C_MUSIC_RAWBUFFMAX; i++) {
		if (_rawBuffer[i].rawContent == NULL) {
			buff = i;
			_rawBuffer[buff].priority = prio * 5;
			break;
		}
	}	
	// If there is no free buffer... OK, we will have to create one :-)
	if (buff == -1) {
		// Select the buffer with the smaller priority
		int minprio = 32000;
		for (i = 0; i < DS_C_MUSIC_RAWBUFFMAX; i++) {
			if (_rawBuffer[i].rawContent != NULL) { // Always true, but better safe than sorry
				if (_rawBuffer[i].priority < minprio) {
					buff = i;
					minprio = _rawBuffer[buff].priority;
				}
			}
		}	
		if (buff == -1)
			ds_global_errorHalt("ASSERTION: \n Bad buffer mgnt.");
		// Stop sounds that are using this buffer
		for (i = 0; i < DS_C_MUSIC_RAWCHMAX; i++) {
			if ((_rawList[i].phyChannel != -1) && (_rawList[i].buffer == buff)) {
				AS_SoundStop(_rawList[i].phyChannel);
				_rawList[i].phyChannel = -1;
				_rawList[i].buffer = -1;
			}
		}
		
		// Clean the buffer
		free(_rawBuffer[buff].rawContent);
		_rawBuffer[buff].rawContent = NULL;
		_rawBuffer[buff].priority = prio * 5;
		_rawBuffer[buff].rawSize = 0;
		sprintf(_rawBuffer[buff].name,"-");

	}
	// OK, now get the buffer and load it. If OK, return the buffer index
	ele = &(_rawBuffer[buff]);
	if (!_ds_music_rawBufferLoad(raw,ele,prio)) {
		return -1;
	}
	return buff;
}

int _ds_music_rawPlay(char *raw, int ch, int loop, int prio) {
	int buff;
	ds_t_bufferRAW *ele;
	SoundInfo sound;
	int realch;
	
	// First, check the buffer that can play this sound
	buff = _ds_music_rawBufferReserve(raw, prio);
	if (buff == -1) {
		return -1;
	}
	ele = &(_rawBuffer[buff]);
	
	// Now, play the sound...
	sound.data = ele->rawContent;
	sound.size = ele->rawSize;
	sound.format = AS_PCM_8BIT;
	sound.rate = 11025;
	sound.volume = DS_C_MUSIC_SNDVOL;
	sound.pan = 64; // FIXME - Change sound according to the position of the sound!
	sound.loop = loop;
	sound.priority = prio; // priority is used for the audio system and for the buffer system
	sound.delay = AS_SURROUND;
	if (ch == -1) {
		realch = AS_SoundDefaultPlay(
			sound.data,sound.size,sound.volume,sound.pan,sound.loop,sound.priority);
		if (realch == -1) {
			return -1;
		}
	} else {
		realch = ch;
		AS_SoundDirectPlay(realch,sound);
	}
	
	// ...and store it on a specific channel
	_rawList[realch].buffer = buff;
	_rawList[realch].phyChannel = realch;
	
	return realch;
}
  
//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Initializes the music engine */
int ds_music_init() {
   // Starts the music engine
	PA_VBLFunctionInit(AS_SoundVBL);
	//AS_Init(AS_MODE_MP3 | AS_MODE_SURROUND | AS_MODE_16CH);
	AS_Init(AS_MODE_MP3 | AS_MODE_16CH);
	//AS_SetDefaultSettings(AS_PCM_8BIT, 11025, AS_SURROUND);
	AS_SetDefaultSettings(AS_PCM_8BIT, 11025, AS_NO_DELAY);
	
	// Empties the music channels
	_ds_music_channelListInit();
	
	// Empties the raw channels
	_ds_music_rawInit();
		
	return 1;
}   

/* Resets the music engine */
int ds_music_reset() {
	// Stop ALL RAW sounds
	_ds_music_rawReset();
	return 1;
}

/* Tell the system to play ONLY a certain music */
int ds_music_playOnlyMusic(int m) {
   ds_t_musicSlot mSlot;
   ds_t_musicSlot *mSlotPnt;
   ds_t_musicList *mList;
   int i;
   int mListLen;
   
   // First! "optimization"
   if (!ds_global_optimizationMusic) {
      m = 0; // Sorry...
   }   
   
   mList = _ds_music_listInit();
   
   // 1st Iteration: Put mp3 on the list
   // Also, get the data for the tune: Number + , Format (MP3/RAW), Priority
   if (m != 0) {
      if (_ds_music_getData(m,&mSlot,DS_C_MUSIC_MUS,0)) // flag onlymp3 not needed
      	_ds_music_listAdd(mList, &mSlot);
   }   
   
   // 2nd Iteration: Delete (Delete channels (including ambiance) with old music)
   _ds_music_channelListPurgeInit();
   mListLen = _ds_music_listLen(mList);
   for (i=0; i < mListLen; i++) {
      mSlotPnt = _ds_music_listGet(mList, i);
      if (_ds_music_channelListExistMark(mSlotPnt)) {
         _ds_music_listDel(mList, i);
      }      
	}	 
	_ds_music_channelListPurgeDo();
   
   // 3rd Iteration: Add (Include the new music)
   //PA_OutputText(1,0,18,"                                 ");
   mListLen = _ds_music_listLen(mList);
   for (i=0; i < mListLen; i++) {
      mSlotPnt = _ds_music_listGet(mList, i);
      _ds_music_channelListAdd(mSlotPnt);
      //PA_OutputText(1,0,18,"Added %d                      ", i);
	}   
	
   // Das Ende
   _ds_music_listEmpty(mList);
   
   return 1;
}

/* Tell the system to play ONLY a certain ambiance */
int ds_music_playOnlyAmbiance(int a1) {
   ds_t_musicSlot mSlot;
   ds_t_musicSlot *mSlotPnt;
   ds_t_musicList *mList;
   int i;
   int mListLen;

   // First! "optimization"
   if (!ds_global_optimizationMusic) {
      a1 = 0;
   }   
   
   mList = _ds_music_listInit();
   
   // 1st Iteration: Put mp3 on the list
   // Also, get the data for every tune: Number + , Format (MP3/RAW), Priority
   if (a1 != 0) {
      if (_ds_music_getData(a1,&mSlot,DS_C_MUSIC_AMB,0)) // flag onlymp3 not needed
      	_ds_music_listAdd(mList, &mSlot);
   }   
   
   // 2nd Iteration: Delete (Delete channels (including ambiance) with old music)
   _ds_music_channelListPurgeInit();
   mListLen = _ds_music_listLen(mList);
   for (i=0; i < mListLen; i++) {
      mSlotPnt = _ds_music_listGet(mList, i);
      if (_ds_music_channelListExistMark(mSlotPnt)) {
         _ds_music_listDel(mList, i);
      }      
	}	 
	_ds_music_channelListPurgeDo();
   
   // 3rd Iteration: Add (Include the new music)
   mListLen = _ds_music_listLen(mList);
   for (i=0; i < mListLen; i++) {
      mSlotPnt = _ds_music_listGet(mList, i);
      _ds_music_channelListAdd(mSlotPnt);
	}   
	
   // Das Ende
   _ds_music_listEmpty(mList);
   
   return 1;
}
 
/* Tell the system to play a certain music and ambiance (both are usually together!) 
	If onlymp3 is ON, the only music that is added is the mp3 sound. */
int ds_music_playMusicAndAmbiance(int m, int a1, int a2, int onlymp3) {
   ds_t_musicSlot mSlot;
   ds_t_musicSlot *mSlotPnt;
   ds_t_musicList *mList;
   int i;
   int mListLen;

   // First! "optimization"
   if (!ds_global_optimizationMusic) {
      m = 0; // Sorry...
      a1 = 0;
      a2 = 0;
   }   
   
   mList = _ds_music_listInit();
   
   // 1st Iteration: Prioritize (only one mp3 available)
   // Also, get the data for every tune: Number + , Format (MP3/RAW), Priority
   if (m != 0) {
      if (_ds_music_getData(m,&mSlot,DS_C_MUSIC_MUS,onlymp3))
      	_ds_music_listAdd(mList, &mSlot);
   }   
   if (a1 != 0) {
      if (_ds_music_getData(a1,&mSlot,DS_C_MUSIC_AMB,onlymp3))
      	_ds_music_listAdd(mList, &mSlot);
   }   
   if (a2 != 0) {
      if (_ds_music_getData(a2,&mSlot,DS_C_MUSIC_AMB,onlymp3))
      	_ds_music_listAdd(mList, &mSlot);
   }   
   
   // 2nd Iteration: Delete (Delete those channels with old music)
   _ds_music_channelListPurgeInit();
   mListLen = _ds_music_listLen(mList);
   for (i=0; i < mListLen; i++) {
      mSlotPnt = _ds_music_listGet(mList, i);
      if (_ds_music_channelListExistMark(mSlotPnt)) {
         _ds_music_listDel(mList, i);
      }      
	}	 
	_ds_music_channelListPurgeDo();
   
   // 3rd Iteration: Add (Include the new music)
   mListLen = _ds_music_listLen(mList);
   for (i=0; i < mListLen; i++) {
      mSlotPnt = _ds_music_listGet(mList, i);
      _ds_music_channelListAdd(mSlotPnt);
	}   
	
   // Das Ende
   _ds_music_listEmpty(mList);
   
   return 1;
}

/* Tell the music subsystem that he must play a normal sound */
int ds_music_playSound(char *raw, int loop, int prio) {
	return _ds_music_rawPlay(raw, -1, loop, prio);
}   

/* Tell the music subsystem that he must play a normal sound in a specific channel */
int ds_music_playSoundChannel(char *raw, int ch, int loop, int prio) {
	// Special test: I am playing this right now? If yes, do not play -:) - FIXME loops!
	if (ch != -1) {
		int b = _rawList[ch].buffer;
		if (b != -1) {
			if (PA_CompareText(_rawBuffer[b].name, raw)) {
				return ch;
			} 
		}
	}
	// OK, back to work
	return _ds_music_rawPlay(raw, ch, loop, prio);
}   

/* Stops a certain channel */
int ds_music_stopSoundChannel(int ch) {
	if (ch != -1) {
		if (_rawList[ch].phyChannel != -1) {
			AS_SoundStop(_rawList[ch].phyChannel);
			_rawList[ch].phyChannel = -1;
			_rawList[ch].buffer = -1;
		}
	}
	
	return 1;
}

/* Plays a special sound - after shift */
void ds_music_playSpecialSound(int snd) {
	switch (snd) {
		case 1:
			ds_music_playSound("Teleport", 0, 0);
			break;
		case 2:
			ds_music_playSound("Switch", 0, 0);
			break;
		case 3:
			ds_music_playSound("Door", 0, 0);
			break;
		case 4:
			ds_music_playSound("Electronic", 0, 0);
			break;
	}
}

void ds_music_manage() {
	int i;
	// Priority decay
	for (i = 0; i < DS_C_MUSIC_RAWBUFFMAX; i++) {
		if ((_rawBuffer[i].rawContent != NULL) && ((ds_global_tick % 60) == 0))
			_rawBuffer[i].priority--;
	}
#ifdef DEBUG_KSDS
	// <DEBUG>
	for (i = 0; i < DS_C_MUSIC_RAWCHMAX; i++) {
			PA_OutputText(1,i,13,"%d",(_rawList[i].phyChannel != -1)?1:0);
	}
	for (i = 0; i < DS_C_MUSIC_RAWBUFFMAX; i++) {
			PA_OutputText(1,i,14,"%d",(_rawBuffer[i].rawContent != NULL)?1:0);
	}
#endif
}   

/* ScrapPad */

/*

Implemented

<Juni's sounds>
Bounce
Bounce Platform
Bounce Lite
Bullet Split
Cannon Shot
Chomp
# Dark Forest Bullet - Unknown critter
DiscBullet
Door
Drop A
Drop B
Electronic
Elemental A
Elemental B
Fire Hit Lite
Fire Hit
Fire Shot (39 & 45)
Ghost Rock
Homing Hit (12)
Homing Shot
Laser Machine A
Laser Machine B
Liquid
Machine Turn A
Machine Turn B
Mega Split
Ninja
Powerup
Right Comb
Roller A
Roller B
Roller Hit
Save Spot
Selfdrop
Spider Run
Spike Up
Spike Down
Switch
Teleport
Tiny Shot
Tiny Hit
Tri Bullet Shot
Tri Bullet Shot B
Tri Bullet Hit
*/