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
#define DS_C_MUSIC_MUSVOL 127
#define DS_C_MUSIC_AMBVOL 127
#define DS_C_MUSIC_CNTVOL -30
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

// Raw Channels
#define DS_C_MUSIC_RAWCHMAX 12

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
ds_t_channel _rawList[DS_C_MUSIC_RAWCHMAX];
ds_t_channelList _channelList;
int juniMusicLoop; // Which Juni's sfx is looping

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

int _ds_music_channelPlay(ds_t_channel *mChannel) {
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
	if (_ds_music_channelPlay(mChannel)) {
		//	PA_OutputText(1,0,17,"Getting %d-Playing-OK. ",mSlot->number);
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
//-------------------------------------------------------------------------------------------------

void _ds_music_rawListInit() {
   int i;
   for (i=0; i<DS_C_MUSIC_RAWCHMAX; i++) {
      _ds_music_channelInit(&(_rawList[i]),0);
   }   
}

void _ds_music_rawListPut(int ch, ds_t_channel *info) {
   _ds_music_channelStop(&(_rawList[ch]));
   _rawList[ch] = *info;
}

// MUSIC (Temporal Sounds) -> Critters - DELETE
// MUSIC (Temporal Sounds) -> Juni - MAINTAIN
// MUSIC (Fixed Sounds) -> Juni - MAINTAIN
// MUSIC (Temporal Sounds) -> System - MAINTAIN
  
//-------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------

/* Initializes the music engine */
int ds_music_init() {
   // Starts the music engine
	PA_VBLFunctionInit(AS_SoundVBL);
	AS_Init(AS_MODE_MP3 | AS_MODE_SURROUND | AS_MODE_16CH);
	AS_SetDefaultSettings(AS_PCM_8BIT, 11025, AS_SURROUND);
	
	// Empties the music channels
	_ds_music_channelListInit();
	
	// Empties the raw channels
	_ds_music_rawListInit();
	
	// Reset's Juni's music
	juniMusicLoop = -1;
	
	return 1;
}   

/* Resets the music engine */
int ds_music_reset() {
   // Looping Juni? Stop!!!!!
   if (juniMusicLoop != -1) {
      AS_SoundStop(juniMusicLoop);
      juniMusicLoop = -1;
	}    
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

/* Tell the music subsystem that he must play a "Juni" Loop Sound */
int ds_music_playJuni(int juniState, int movStateX, int movStateY) {
   int sndLoop = -1;
   // Convert the Juni state into the parameters for the music
   // DS_C_JUNI_ST_STOP_
   if ((juniState == DS_C_JUNI_ST_WALK_R) || (juniState == DS_C_JUNI_ST_WALK_L)) {
      sprintf(ds_global_string,"Walk.raw");
      sndLoop = 1;
   } else   
   if ((juniState == DS_C_JUNI_ST_RUN_R) || (juniState == DS_C_JUNI_ST_RUN_L)) {
      sprintf(ds_global_string,"Run.raw");
      sndLoop = 1;
   } else   
   //DS_C_JUNI_ST_FALL_
   //DS_C_JUNI_ST_JUMP_
   if ((juniState == DS_C_JUNI_ST_CLIMB_R) || (juniState == DS_C_JUNI_ST_CLIMB_L)) {
      if (movStateY == DS_C_JUNI_MOVST_Y_CLIMB) {
      	sprintf(ds_global_string,"Climb.raw");
      	sndLoop = 1;
    	} else   	
      if (movStateY == DS_C_JUNI_MOVST_Y_SLIDE) {
      	sprintf(ds_global_string,"Slide.raw");
      	sndLoop = 1;
    	}
   } else   
   //DS_C_JUNI_ST_FLY_
   
   // Stop Music if there is something playing
   //AS_SoundVBL
   
   // No music, byebye
   if (sndLoop == -1) {
   }   
   	return -1;
   	
   
}   

void ds_music_manage() {
	// <DEBUG>
	//PA_OutputText(1,0,13,"([%d]%d) F:%d       ",_channelList.cMP3[0].slot.number,_channelList.cMP3[0].slot.type,_channelList.cMP3[0].rawSize);
	//PA_OutputText(1,0,14,"([%d]%d) F:%d       ",_channelList.cRAW[0].slot.number,_channelList.cRAW[0].slot.type,_channelList.cRAW[0].rawSize);
	//PA_OutputText(1,0,15,"([%d]%d) F:%d       ",_channelList.cRAW[1].slot.number,_channelList.cRAW[1].slot.type,_channelList.cRAW[1].rawSize);
}   

/* ScrapPad */

/* OLD CODE

typedef struct ds_tt_music {
   int name; // Name of the music that is playing rigt now. 0 = nothing. -1 = special (destroy!!!!!)
   int vol; // Volume of the music. Also used as a counter when vol<0
} ds_t_music;

typedef struct ds_tt_musicContainer {
   ds_t_music actualMusic;
   ds_t_music newMusic;
} ds_t_musicContainer;    

ds_t_musicContainer _ds_music_state;
char mspath[255];


void _ds_music_initMusic(ds_t_music *music) {
   music->name = 0;
   music->vol = 0;
}

void _ds_music_insertMusic(ds_t_music *music, int name) {
   music->name = name;
   music->vol = DS_C_MUSIC_MAXVOL;
}   

void _ds_music_volumeMusic(int vol) {
   if ((vol >= 0) && (vol <=128))
   	AS_SetMP3Volume(vol);
}

void _ds_music_playMusic(ds_t_music *music) {
   // Actual mode: Streams music from "HDD"
   sprintf(mspath,"%s%s/Song%d.mp3",ds_global_world.dir,DS_DIR_MUSIC,music->name);
	if (!ds_util_fileExists(mspath))
		sprintf(mspath,"%s%s/Song%d.mp3",DS_DIR_MAIN,DS_DIR_MUSIC,music->name);

   AS_MP3StreamPlay(mspath);
   _ds_music_volumeMusic(music->vol);
}

void _ds_music_stopMusic() {
   AS_MP3Stop();
}

// Tell the system to stop playing music, fading
int ds_music_stopMusic() {
   // First case: (_,*) [empty music] -> (_,_)
	if (_ds_music_state.actualMusic.name == 0) {
		_ds_music_initMusic(&_ds_music_state.actualMusic);
   	_ds_music_initMusic(&_ds_music_state.newMusic);
	} else
	// Second case: (A,*) -> (A,Del)
	if (_ds_music_state.actualMusic.name != 0) {
   	_ds_music_initMusic(&_ds_music_state.newMusic);
		_ds_music_state.newMusic.name = -1;
	} 
}

// Tell the system to stop playing music, ALTOGETHER 
int ds_music_stopForceMusic() {
   // Always, there will be no new music
  	_ds_music_initMusic(&_ds_music_state.newMusic);
	if (_ds_music_state.actualMusic.name != 0) {
	   _ds_music_stopMusic();
  		_ds_music_initMusic(&_ds_music_state.actualMusic);
	}
}

// Tell the system to play a certain music 
int ds_music_playMusic(int tune) {
   // First case: (_,_) [empty music] -> Play(N') + (N',_)
	if ((_ds_music_state.actualMusic.name == 0) && (_ds_music_state.newMusic.name == 0)) {
	   _ds_music_insertMusic(&_ds_music_state.actualMusic,tune);
	   _ds_music_playMusic(&_ds_music_state.actualMusic);
	} else
	// Second case: (A,_) [actual music is ON] -> (A,_) [A = N'] | (A,N') [otherw]
	if ((_ds_music_state.actualMusic.name != 0) && (_ds_music_state.newMusic.name == 0)) {
	   if (_ds_music_state.actualMusic.name != tune) {
	      // New tune!!!
	      _ds_music_insertMusic(&_ds_music_state.newMusic,tune);
	   }   
	} else   
	// Third case: (A,N) [new music on queue] -> (A,N) [N = N'] | (A,_) [A = N'] | (A,N') [otherw]
	if ((_ds_music_state.actualMusic.name != 0) && (_ds_music_state.newMusic.name != 0)) {
	   if (_ds_music_state.newMusic.name == tune) {
	      // Do nothing, keep (A,N)
	   } else  
	   if (_ds_music_state.actualMusic.name == tune) {
	      _ds_music_initMusic(&_ds_music_state.newMusic); // No need to change!!!! (A,_)
	   } else {
	      _ds_music_insertMusic(&_ds_music_state.newMusic,tune); // We need to change... (A,N')
	   }   
	} else
	// Fouth case: (_,N) [music wants to be played...] -> (_,N') <On this state N is not played yet>
	if ((_ds_music_state.actualMusic.name == 0) && (_ds_music_state.newMusic.name != 0)) {
	   _ds_music_insertMusic(&_ds_music_state.newMusic,tune); // We will play the new tune! (_,N')
	}   
}

// Manage the music engine
int ds_music_manage() {
   // First case: (_,_) [empty music]
	if ((_ds_music_state.actualMusic.name == 0) && (_ds_music_state.newMusic.name == 0)) {
	   // Nothing to do!
	} else
	// Second case: (A,_) [actual music is ON] -> Fade In(A) [Vol<MAX] | ... [otherw]
	if ((_ds_music_state.actualMusic.name != 0) && (_ds_music_state.newMusic.name == 0)) {
	   // Fade in?
	   if (_ds_music_state.actualMusic.vol < DS_C_MUSIC_MAXVOL) {
	      _ds_music_state.actualMusic.vol++;
	      _ds_music_volumeMusic(_ds_music_state.actualMusic.vol);
	   }
		// Nothing more...     
	} else   
	// Third case: (A,N) [new music on queue] -> Fade Out(A) [Vol>0] | Stop(A) + (_,N) [otherw]
	if ((_ds_music_state.actualMusic.name != 0) && (_ds_music_state.newMusic.name != 0)) {
	   // Fade out?
	   if (_ds_music_state.actualMusic.vol > DS_C_MUSIC_CNTVOL) {
	      _ds_music_state.actualMusic.vol--;
	      if (_ds_music_state.actualMusic.vol >= 0) {
	         _ds_music_volumeMusic(_ds_music_state.actualMusic.vol);
	      }    
	   } else {
	      _ds_music_stopMusic();
	      _ds_music_initMusic(&_ds_music_state.actualMusic); // Init A, so (A,N) -> (_,N)
		}        
	} else
	// Fouth case: (_,N) [music wants to be played...] -> Play(N) + (N,_) [N != Del] | (_,_) [otherw]
	if ((_ds_music_state.actualMusic.name == 0) && (_ds_music_state.newMusic.name != 0)) {
	   if (_ds_music_state.newMusic.name == -1) {
	      // Ups! Da end...
		   _ds_music_initMusic(&_ds_music_state.actualMusic);
   		_ds_music_initMusic(&_ds_music_state.newMusic);
	   } else {
	      _ds_music_state.actualMusic = _ds_music_state.newMusic; // (N,N)
	      _ds_music_initMusic(&_ds_music_state.newMusic); // (N,_)
	      _ds_music_playMusic(&_ds_music_state.actualMusic); 
		}      
	}   
	
	// <DEBUG>
	PA_OutputText(1,0,13,"A (%d) V:%d",_ds_music_state.actualMusic.name,_ds_music_state.actualMusic.vol);
	PA_OutputText(1,0,14,"A (%d) V:%d",_ds_music_state.newMusic.name,_ds_music_state.newMusic.vol);
}

*/
