
/*
 *  Platz - A platformer framework for the Uzebox (supports VIDEO_MODE 3)
 *  Copyright (C) 2009 Paul McPhee
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLATZ_H
#define PLATZ_H

/****************************************
 *			  	 Defines				*
 ****************************************/

#ifndef MAX_ANIMATED_BGS		// Maximum animated bgs per world slice
	#define MAX_ANIMATED_BGS 0
#endif

#ifndef MAX_MOVING_PLATFORMS	// Maximum moving platforms per world slice
	#define MAX_MOVING_PLATFORMS 0
#endif

#ifndef VIEWPORT_SLACK			// How far from the viewport anchor should the player move before we scroll
	#define VIEWPORT_SLACK 0
#endif

#ifdef DUAL_AXIS_SCROLLING
	#define MAX_VIS_SLICES 2	// 4 - Not yet supported
#else
	#define MAX_VIS_SLICES 2
#endif

#ifndef SLICE_SEQ_LEN
	#define SLICE_SEQ_LEN 0
#endif

#define GET_VEL(v)	(((v).frames&(v).mod)?(v).disp:(v).modDisp)		// Takes a velocity struct v


#define inline_set_tile(x,y,tileId) 	\
	asm (								\
		"mov r24,%2" "\n\t"				\
		"ldi r25,%4" "\n\t"				\
		"ldi %A3,lo8(vram)" "\n\t"		\
		"ldi %B3,hi8(vram)" "\n\t"		\
		"mul %1,r25" "\n\t"				\
		"clr r25" "\n\t"				\
		"add r0,%0" "\n\t"				\
		"adc r1,r25" "\n\t"				\
		"add %A3,r0" "\n\t"				\
		"adc %B3,r1" "\n\t"				\
		"subi r24,%5" "\n\t"			\
		"st %a3,r24" "\n\t"				\
		"clr r1"						\
		: /* no outputs */				\
		: "r" (x),						\
		  "r" (y),						\
		  "r" (tileId),					\
		  "e" (vram),					\
		  "M" (VRAM_TILES_H),			\
		  "M" (255-(RAM_TILES_COUNT-1))	\
		: "r24",						\
		  "r25"							\
	)

// Constants
#define L_INTERSECT				1
#define R_INTERSECT				2
#define V_INTERSECT				(L_INTERSECT|R_INTERSECT)
#define T_INTERSECT				4
#define B_INTERSECT				8
#define H_INTERSECT				(T_INTERSECT|B_INTERSECT)
#define AXIS_X					1		// Platform axes
#define AXIS_Y					2		//
#define ORI_LRUD				0		// Trigger orientations - Left/Right Up/Down
#define ORI_RLDU				1		// Right/Left Down/Up
#define PF_ZERO					0xff	// Invalid platform directory index - no platforms exist in this slice
#define MAX_SLICES				0xfe	// 0xff left for invalid flag
#define DIR_RIGHT 				1
#define DIR_LEFT				-1
#define DIR_DOWN				1
#define DIR_UP					-1
#define DIR_NONE				0
#define PLATZ_SCRN_WID			240
#define PLATZ_SCRN_HGT			224
#define MP_SMOOTH				0x40
#define MP_STEPPED 				0x80
	// Mutatable bg event codes
#define PLATZ_MUT_EV_DRAW		1
#define PLATZ_MUT_EV_ANIM		2
#define PLATZ_MUT_EV_COLLISION	4

// Level slice bit-flags
	// Outer
#define BGC		0x01	// Collidable
#define BGT		0x02	// Triggerable
#define BGI		0x04	// Invisible
#define BGPRJ	0x08	// Projectile Barrier
#define BGQ		0x10	// Queryable

	// Inner
#define BGP		0x01	// Patterned
#define BGA		0x02	// Animated (automatically repeats like a pattern)

	// Common
#define BGM		0x80	// Mutable





/****************************************
 *			Type declarations			*
 ****************************************/

//typedef uint8_t u8;
//typedef uint16_t u16;
//typedef uint32_t u32;

typedef struct pt {
	u8	x;
	u8	y;
} pt;

typedef struct pt16 {
	int	x;
	int	y;
} pt16;

typedef struct rect {
	u8 left;
	u8 right;
	u8 top;
	u8 btm;
} rect;

typedef struct rect16 {
	int left;
	int right;
	int top;
	int btm;
} rect16;

typedef struct line16 {
	pt16 p1;
	pt16 p2;
} line16;

typedef struct velocity {		// Single axis velocity
	char 	vel;				// Base velocity - implemented as mod, modDisp and disp
	char	dir;				// Direction of travel - useful for when spd == 0
	u8		frames;				// Counts the game frames for use by mod
	u8 		mod;				// Fractional speed adjustment (2^n-1)
	char	modDisp;			// Special case displacement when frames&mod == 0
	char	disp;				// Typical displacement
} velocity;

typedef struct animation {
	u8			size;			// wid*hgt
	u8			wid;			// Width of each frame (in tiles)
	u8			hgt;			// Height of each frame (in tiles)
	u8			frameCount;		// # of frames in an animation cycle
	u8 			currFrame;		// The frame that is currently displayed
	u8			disp;			// Displacement counter
	u8			dpf;			// Displacement per frame (scales frame rate to movement speed)
	u8			synch;			// Flag to indicate animation should be synchronized with others of similar type
	const char	*frames;		// Stored in flash
} animation;

typedef struct bgAnimIndex {
	u8 iOuter;					// Outer bg index of bg animation
	u8 iInner;					// Inner bg index of bg animation
} bgAnimIndex;

typedef struct bgInner {
	u8 type;					// 0|BGA|BGP|BGM
	u8 tile;
	rect r;
} bgInner;

typedef struct bgOuter {
	u8 type;					// 0|BGC|BGT*|BGI|BGM
	u8 count;					// # of inner bgs in this outer bg
	u16 index;					// The position in bgiTbl at which the inner bgs begin
	rect r;
} bgOuter;

typedef struct object {			// Non-interactive objects (may be option interactivity in a future release)
	pt			begin;			// Position that top-left corner of object will be placed
	const char	*map;			// Object data
} object;

typedef struct bgDirectory {
	u16 objOffset;				// Index into pgmObjects flash array
	u8 objCount;				// The # of objects in the slice
	u16 bgoIndex;				// Index into pgmBgs flash array
	u8 bgoCount;				// The # of background elements in the slice
	u8 bgoBeginCount;			// The # of left-to-right seam collision bgs
	u8 bgoCommonCount;			// The # of right-to-left seam collision bgs common to begin/end
	u8 bgoEndIndex;				// Points to right-to-left seam collision bgs specific to end. Combined with bgoCommonIndex, prevents repeating bgs.
	u8 animCount;				// The # of animated background elements in the slice (always the first elements in the slice for easy loading)
	u8 animIndex;				// Index into pgmAnimDir
	u8 pdIndex;					// Index into platforms directory (PF_ZERO if none)
} bgDirectory;

typedef struct platform {
	u8		clrTile;			// How to paint vacated tiles (2 MSB reserved for mode due to compiler adding 1.2k below)
	//u8		mode;				// Smooth or stepped movement (adds 1.2k???)
	u8		min;				// Highest/Leftmost position of platform movement
	u8		max;				// Lowest/Rightmost position of platform movement
	u8		axis;				// Horizontally or vertically moving
	char	v;					// Velocity
	rect 	r;					// Platform bounds
} platform;

typedef struct platformDirectory {
	u8 index;					// Position in platTbl that these platforms can be found
	u8 count;					// # of platforms to retrieve
} platformDirectory;

typedef struct platzActor {
	pt 			loc;			// Player location within the current slice
	u8 			sprx;			// Center of sprite on x axis
	pt 			trLoc;			// Trigger location relative to loc
	u8 			bbx;			// Bounding box x radius
	u8 			bby;			// Bounding box y radius
	velocity 	vx;				// X-axis velocity
	velocity	vy;				// Y-axis velocity
} platzActor;

typedef void (*trigCallback)(u16,u8,char);
typedef u8 (*mutCallback)(u8,bgInner*,bgInner*,void*);
typedef void (*queryCallback)(bgOuter*,u8*);

/****************************************
 *			Function prototypes			*
 ****************************************/

// Platz main interface
u8 PlatzGetCollisionPointer(void);
u8 PlatzGetWorldSlicePointer(void);
void PlatzSetViewport(u8 anchor, u8 slack);
void PlatzSetAnchor(u8 anchor);
void PlatzInit(platzActor *a, u8 sliceCount);
u8 PlatzMove(platzActor *a);
void PlatzMoveToSlice(platzActor *a, u8 sp);
u8 PlatzSetBoundingBoxDimensions(platzActor *a, u8 wid, u8 hgt);
void PlatzSetVelocity(velocity *v, char val, u8 *trPos);
void PlatzTick(void);

#if MAX_MOVING_PLATFORMS
	void PlatzSetMovingPlatformTiles(u8 hTilesIndex, u8 vTilesIndex, u8 shTilesIndex, u8 svTilesIndex);
#endif

// Platz initialization
void PlatzSetTriggerCallback(trigCallback tcb);
void PlatzSetMutCallback(mutCallback mcb);
void PlatzSetQueryCallback(queryCallback qb);
void PlatzSetMovingPlatformTable(const platform *p);
void PlatzSetMovingPlatformDirectory(const platformDirectory *pd);
void PlatzSetMapsTable(PGM_P *m);
void PlatzSetAnimatedBgTable(const animation *a);
void PlatzSetAnimatedBgDirectory(const bgAnimIndex *bgad);
void PlatzSetObjectTable(const object *obj);
void PlatzSetInnerBgTable(const bgInner *bgi);
void PlatzSetOuterBgTable(const bgOuter *bgo);
void PlatzSetBgDirectory(const bgDirectory *bgd);

// Platz utilities
void PlatzFill(const rect *r, u8 tileId);
void PlatzFillMap(const rect *r, u8 xOffset, u8 yOffset, const char *map, int dataOffset);
char PlatzCcw(const pt16 *p0, const pt16 *p1, const pt16 *p2);
void PlatzHideSprite(u8 spriteIndex, u8 wid, u8 hgt);
u8 PlatzLinesIntersect(const line16 *l1, const line16 *l2);
void PlatzMapSprite(u8 index, u8 wid, u8 hgt, const char *map, u8 spriteFlags);
u8 PlatzRectsIntersect(const rect *r1, const rect *r2);
u8 PlatzRectsIntersect16(const rect16 *r1, const rect16 *r2);

#endif


