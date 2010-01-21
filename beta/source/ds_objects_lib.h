/*
Copyright (c) 2008 Rodrigo Roman and Ramon Roman (rrc2soft)
(Original Game "Knytt Stories" (c) Nicklas "Nifflas" Nygren)

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


/* Library to manage objects and common behaviour */
#ifndef _DS_G_OBJECTSLIB_
#define _DS_G_OBJECTSLIB_

#include "ds_global.h"

/* Starts Object - Init variables. */
int ds_objects_lib_initObject(u8 bank, u8 obj, ds_t_object *object); 

/* Starts Object - Preloads its image/sprite */
int ds_objects_lib_initObjectImage(u8 bank, u8 obj, ds_t_object *object);

/* Starts CUSTOM Object - Preloads its image/sprite, but manage special CO quirks */
int ds_objects_lib_initCustomObjectImage(u8 bank, u8 obj, ds_t_object *object);

/* Returns 1 if the object is an event or an item */
int ds_objects_lib_iseventitem(u8 type);

/* Returns 1 if the object is near juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_isNearJuni(ds_t_object *object, int pix, int cvirtual);

/* Returns the distance object <-> Juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_distanceJuni(ds_t_object *object, int cvirtual);

/* Returns a "corrected" distance object <-> Juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) 
	Why "corrected"? to extend the range of certain objects (e.g. dissapearing cubes!)  */
int ds_objects_lib_distanceJuniCorrected(ds_t_object *object, int cvirtual);

/* Returns a "corrected" phytagorean distance object <-> Juni. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) 
	Why "corrected"? to extend the range of certain objects (e.g. dissapearing cubes!)  */
int ds_objects_lib_distancePhyJuniCorrected(ds_t_object *object, int cvirtual);

/* Returns the distance object <-> Juni X. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_distanceJuniX(ds_t_object *object, int cvirtual, int absolute); 

/* Returns the distance object <-> Juni Y. Virtual == 1 if we consider the "cvirtual" coordinates (hologram) */
int ds_objects_lib_distanceJuniY(ds_t_object *object, int cvirtual, int absolute);

/* Check if an object is over an object */
int ds_objects_lib_stepObject(ds_t_object *object);

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/

/* Behaviour - executes ghost nature (alpha cycle) */
void ds_objects_lib_beh_ghost(ds_t_object *object, int minA, int maxA, int rndA);

/* Behaviour - Inits ghost nature (alpha cycle) */
void ds_objects_lib_beh_ghostIni(ds_t_object *object, int ghost, int minA, int maxA, int rndA);

/* Behaviour - executes ghost nature (float style) */
void ds_objects_lib_beh_ghostFloat(ds_t_object *object);

/* Behaviour: "signal" dialog */
void ds_objects_lib_beh_dialogue(ds_t_object *object, int type);

/* Behaviour: "shift" - change map! */
void ds_objects_lib_beh_shift(ds_t_object *object, int type);

/* Behaviour: Make 4 Following particles. */
int ds_objects_lib_par_make4Following(ds_t_object *object, int sec, int waitTime, int speedX, int speedY, int gravity, int gravityCorr, int maxspeedX, int maxspeedY);

/* Behaviour: Set parameters to a particle following. Speeds are in x1024 */
//int ds_objects_lib_beh_particleFollowing_Init(ds_t_object *object, int waitTime, int speedX, int speedY, int gravity, int gravityCorr, int maxspeedX, int maxspeedY);

/* Behaviour: Particle Following Juni */
//int ds_objects_lib_beh_particleFollowing(ds_t_object *object);

/* Behaviour: Set parameters to an entity following. Speeds are in x1024 */
int ds_objects_lib_beh_entityFollowing_Init(ds_t_object *object, int waitTime, int speedX, int speedY, int gravity, int gravityCorr, int maxspeedX, int maxspeedY);

/* Behaviour: Entity Following (Floating) Juni. */
int ds_objects_lib_beh_entityFollowing(ds_t_object *object, int slowCollided);

/* Behaviour: Make 4 Flying Bullets. */
int ds_objects_lib_par_make4FlyingBullets(ds_t_object *object, int sec, int speed, int waitTime);

/* Behaviour: Make 6 Flying Bullets. */
int ds_objects_lib_par_make6FlyingBullets(ds_t_object *object, int sec, int speed, int waitTime);

/* Behaviour: Make 8 Flying Bullets. */
int ds_objects_lib_par_make8FlyingBullets(ds_t_object *object, int sec, int speed, int waitTime);

/* Behaviour: Set parameters to a flying bullet */
int ds_objects_lib_beh_particleFlyingBullet_Init(ds_t_object *object, int waitTime, int direction, int speed);

/* Behaviour: Flying Bullet */
int ds_objects_lib_beh_particleFlyingBullet(ds_t_object *object);

/* Behaviour: Launch a bee bullet */
int ds_objects_lib_beh_beeBullet(ds_t_object *object, int indexBullet, int sec, int sec60, int maxBullet);

/* Behaviour: Make 4 particles fall. NOTE that SpeedY is given in 1=1024 */
int ds_objects_lib_par_make4ParticleFalling(ds_t_object *object, int sec, int maxSpeedX, int maxSpeedY, int particles);

/* Behaviour: Make 2 particles fall. NOTE that SpeedY is given in 1=1024 */
int ds_objects_lib_par_make2ParticleFalling(ds_t_object *object, int sec, int maxSpeedX, int maxSpeedY, int particles);

/* Behaviour: Launch particles. NOTE that SpeedY is given in 1=1024 */
int ds_objects_lib_par_launchParticles(ds_t_object *object, int sec, int when, int maxSpeedX, int maxSpeedY, int particles);

/* Behaviour: Set parameters to a particle falling */
int ds_objects_lib_beh_particleFalling_Init(ds_t_object *object, int waitTime, int speedX, int speedY, int gravity, int maxspeedY);

/* Behaviour: Particle falling */
int ds_objects_lib_beh_particleFalling(ds_t_object *object);

/* Behaviour: MMF2 Particle - Init */
int ds_objects_lib_beh_particleMMF2_Init(ds_t_object *object, int speedX, int speedY, 
						int gravity, int deceleration, int spriteMov);
						
/* Behaviour: MMF2 Particle - Generic Movement */
void ds_objects_lib_beh_particleMMF2_Move(ds_t_object *object, int speedX, int speedY, int coll);

/* Behaviour: MMF2 Particle, Speed */
int ds_objects_lib_beh_particleMMF2_isStopped(ds_t_object *object);

/* Behaviour: MMF2 Particle */
int ds_objects_lib_beh_particleMMF2(ds_t_object *object, u8 slowdown);

/* Behaviour: MMF2 Particle Following Juni */
int ds_objects_lib_beh_particleMMF2Follow(ds_t_object *object, u8 slowdown);

/* Behaviour: Just Cycle */
int ds_objects_lib_beh_cycle(ds_t_object *object, int waitTime);

/* Behaviour: Cycle, looking at juni */
int ds_objects_lib_beh_cycleLook(ds_t_object *object, int waitTime);

/* Behaviour: Animation w/o cycle. */
int ds_objects_lib_beh_anim(ds_t_object *object, int waitTime, int startanim);

/* Behaviour: Cycle AND Die*/
int ds_objects_lib_beh_cycleDie(ds_t_object *object, int waitTime);

/* Behaviour: Cycle back and forth */
int ds_objects_lib_beh_cycleBackForth(ds_t_object *object, int waitTime);

/* Behaviour: fly around the screen */
void ds_objects_lib_beh_flyAround(ds_t_object *object, int Speed, 
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int followJuni, int distanceJuni, int trembleSprite);

/* Behaviour: fly around the screen, but try to *chof* Juni! This is a particular case of the flyAround method */
void ds_objects_lib_beh_flyAroundFaller(ds_t_object *object, int Speed, 
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int distanceJuni, int trembleSprite);

/* Behaviour: fly randomly */
void ds_objects_lib_beh_flyRandom(ds_t_object *object, int Speed, int waitingMovement, int waitingSpriteMovement, int waitingStop, int activeJuni);

/* Behaviour: move left and right (INI) */
void ds_objects_lib_beh_moveLeftRight_ini(ds_t_object *object, int face);

/* Behaviour: move left and right */
void ds_objects_lib_beh_moveLeftRight(ds_t_object *object, int minSpeed, int maxSpeed, int accIni, int accFin,
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int followJuni, int distanceJuni, int face, 
											int checkcollision);
											
/* Behaviour: float left and right (INI) */
void ds_objects_lib_beh_floatLeftRight_ini(ds_t_object *object, int face, int dir);

/* Behaviour: float left and right */
void ds_objects_lib_beh_floatLeftRight(ds_t_object *object, int minSpeed, int maxSpeed, int probSlower, int distSlower,
											int minDist, int maxDist, 
											int minWaitingTime, int maxWaitingTime, int waitingMovement, int waitingSpriteMovement,
											int collideWall, int waitAfterCollide);
											
/* Behaviour: jump "intelligently" - INI */
void ds_objects_lib_beh_jumpJuniJump_ini(ds_t_object *object);   

/* Behaviour: jump "intelligently" */
void ds_objects_lib_beh_jumpJuniJump(ds_t_object *object, int distJuni, int speedJump,
											int distJump, 
											int waitingMovementSurface, int waitingMovementJump);
											
/* Behaviour: jump in a sequence */
void ds_objects_lib_beh_jumpSequence(ds_t_object *object, 
											int sY1, int sY2, int sY3, int sY4, int sY5,
											int maxSpeedY,
											int gravity,
											int sequenceRandom, 
											int waitingTime);										

/* Behaviour: follow a predefinded Path */
void ds_objects_lib_beh_followPath(ds_t_object *object, int speed, int sizeArray,
											int *arrMov, int *arrDir,
											int waitingMovement, int waitingSpriteMovement);

/* Behaviour: follow a predefinded Path, changing Left-Right */
void ds_objects_lib_beh_followPathLR(ds_t_object *object, int speed, int sizeArray,
											int *arrMov, int *arrDir,
											int waitingMovement, int waitingSpriteMovement,
											int softMov);
											
void ds_objects_lib_beh_followPixelPathLR(ds_t_object *object, int sizeArray, 
											int* arrSpeed, int *arrMovX, int *arrMovY,
											int waitingSpriteMovement,
											int canStop);
											
/* Behaviour: do N gestures */
void ds_objects_lib_beh_knyttNgestures(ds_t_object *object, int sizeArray, int *arrG, int waitingTime, int waitingSpriteTime, int LR);											
											
/* Behaviour: Flower behaviour. Returns 1 if we need to shoot a particle */
int ds_objects_lib_beh_flower(ds_t_object *object, int startCond, int startVal, int endCond, int endVal, 
											int waitingSeedTime, int waitingTime);

/* Behaviour: Spike Behavour. Float towards an area */
int ds_objects_lib_beh_spikeFloat(ds_t_object *object, int minMov, int maxMov);

/* Behaviour: Trap Behaviour. Wait till Shoot. Return 1 if shoot. */
int ds_objects_lib_beh_trapShoot(ds_t_object *object, int near, int cont, int waitingSprite);

/* Behaviour: Disk Behaviour. Moves. Return 1 if particle is launched. 
		Moves on X (useX) or Y (!useX)
		NOTE that is changed by the OneInstance method or external means. */
int ds_objects_lib_beh_disk(ds_t_object *object, int useX,
									 int changeSpeed, 
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int changeDirection,
									 int waitingSprite, 
									 int spritesM, int spritesG);
									 		
/* Behaviour: Disk-like Movement (L-R). Used by critters that need of disk-like movement.
	Moves on X (useX) or Y (!useX)
	Note: returns 1/2 (L/R) if followJuni && touches wall
	Note2: if cycleScreen, does not detect collision, and just moves. */
int ds_objects_lib_beh_diskMovementLR(ds_t_object *object, int useX, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int endR, int endL, int endRG, int endLG,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int followJuni,
									 int waitingSprite,
									 int cycleScreen,
									 int launchAttack);
									 									 
/* Behaviour: Disk-like Movement (L-R) with spikes. 
	Used by critters that need of disk-like movement, but show spikes whenever Juni
	is near. */
int ds_objects_lib_beh_diskMovementLR_Spike(ds_t_object *object, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int endR, int endL, int endRS, int endLS,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int distanceSpikes,
									 int followJuni,
									 int waitingSprite);

/* Behaviour: Disk-like Movement (B). Used by critters that need of disk-like movement. */
int ds_objects_lib_beh_diskMovementB(ds_t_object *object, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int end, int endG,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int followJuni,
									 int waitingSprite);

/* Behaviour: Disk-like Movement (B) with spikes. 
	Used by critters that need of disk-like movement, but show spikes whenever Juni
	is near. */
int ds_objects_lib_beh_diskMovementB_Spike(ds_t_object *object, 
									 int changeSpeed, int changeSpeedStopped,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int end, int endLS,
									 int changeDirection, int changeDirectionChance,
									 int cycleGesture,
									 int distanceSpikes,
									 int followJuni,									 
									 int waitingSprite);

/* Behaviour: Appear / Dissapear. May mark as harmful ;-) (2 implies always harmful) */
int ds_objects_lib_beh_appear(ds_t_object *object, int distNear, int distFar, 
		int harm, int waitTime, int stuckForever);
		
/* Behaviour: Particle Launch (Stop) Behaviour. Moves. Return 1 if particle is launched. */
int ds_objects_lib_beh_plaunchStop(ds_t_object *object, 
									 int countMax, int countFlash,
									 int waitingSprite);
									 
/* Behaviour: Slime-like movement. With deceleration */
int ds_objects_lib_beh_slimeMovementLR(ds_t_object *object, 
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int deceleration,
									 int waitingSprite);

/* Behaviour: Squirrel-like movement. With deceleration */
int ds_objects_lib_beh_squirrelMovementLR(ds_t_object *object, 
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int deceleration,
									 int countDir, int randDir,
									 int followJuni,									 
									 int waitingSprite);

/* Behaviour: Hunter Behaviour. Hunts Juni if conditions are met. */
int ds_objects_lib_beh_hunter(ds_t_object *object, 
									 int condYminus, int condYplus, int condTime, int condChance,
									 int speedX,
									 int followJuni, int keepOnRunning,
									 int waitingSprite, 
									 int iniRS, int iniLS, int endRS, int endLS,
									 int iniR, int iniL, int endR, int endL);

/* Behaviour: Launcher Movement (L-R). Critters with disk-like movement and particle launching. */
int ds_objects_lib_beh_launcherMovementLR(ds_t_object *object, 
									 int changeSpeed, int changeDirection,
									 int changeSpeedS, int changeSpeedR, int changeSpeedM,
									 int timeChange, int distChange,
									 int timeLaunch,
									 int endR, int endL, int endRG, int endLG,
									 int cycleGesture,
									 int followJuni,									 
									 int waitingSprite);
									 									 									 									 							 
/* Behaviour: Partile Flier */
int ds_objects_lib_beh_flyParticle(ds_t_object *object,
									int changeSpeed, int changeDirection,
									int maxSpeed);

/* Behaviour: Elemental */
int ds_objects_lib_beh_elemental(ds_t_object *object, int spMul, int waitingSpriteTime, int damage);

#endif
