
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


/****************************************
 *			Library Dependencies		*
 ****************************************/
#include <avr/pgmspace.h>
#include <string.h>
#include <uzebox.h>
#include <stdint.h>
#include "platz.h"

/****************************************
 *				Utils					*
 ****************************************/
#define MIN(_x,_y)  ((_x)<(_y) ? (_x) : (_y))
#define MAX(_x,_y)  ((_x)>(_y) ? (_x) : (_y))
#define ABS(_x)		(((_x) > 0) ? (_x) : -(_x))
#define NORMALIZE(x) (((x) > 0) ? 1 : ((x) < 0) ? -1 : 0)
// Faster to prove the negative case
#define PT_NOT_IN_RECT(p,r) (((p).x < ((r).left)) || ((p).x >= ((r).right)) || ((p).y < ((r).top)) || ((p).y >= ((r).btm)))

// Constants
#define SCRL_WID			256
#define SCRL_WID_SHFT		8		// 2^8 = 256
// The number of speeds that should be updated each frame (2 for the player's speeds and the remaining for moving platforms)
#define TICK_COUNT			(2+MAX_VIS_SLICES*MAX_MOVING_PLATFORMS)

#ifndef OVERLAY_LINES
	#define OVERLAY_LINES 0
#endif

/****************************************
 *			Type declarations			*
 ****************************************/

typedef struct platforms {
	u8			slice[MAX_VIS_SLICES];						// World slice pointers
	u8 			count[MAX_MOVING_PLATFORMS];				// # of platforms in each slice
	velocity 	v[MAX_VIS_SLICES][MAX_MOVING_PLATFORMS];	// Individual platform velocities
	platform 	p[MAX_VIS_SLICES][MAX_MOVING_PLATFORMS];	// Platform data
} platforms;

typedef struct platformsCommon {
	u8 vtiles;					// Offset to vertical platform tiles
	u8 htiles;					// Offset to horizontal platform tiles
	u8 vstepped;				// Offset to vertical stepped platform tile (only paints square)
	u8 hstepped;				// Offset to horizontal stepped platform tile (only paints square)
} platformsCommon;

typedef struct animatedBgs {
	u8 			count[MAX_VIS_SLICES];
	bgAnimIndex	ids[MAX_VIS_SLICES][MAX_ANIMATED_BGS];
	bgInner 	bgs[MAX_VIS_SLICES][MAX_ANIMATED_BGS];
#ifdef MUTABLE_BGS
	bgInner		mutBgs[MAX_VIS_SLICES][MAX_ANIMATED_BGS];
#endif
	animation 	anims[MAX_VIS_SLICES][MAX_ANIMATED_BGS];
} animatedBgs;

#if MAX_PROJECTILES&MAX_PROJECTILE_CLASSES
// Note: Could have an animation to display based on the projectile and the surface it hits. Could
// also have a tile/map to display at the collision point (like a burn mark for a fireball). Query
// vram array for the tile that was hit (as Outer BG hides it) so we know how to react/which overlay tile to show.

typedef struct projectileClass {
	u8			type;					// Determines how we treat collisions/deflections/animations/etc
	velocity 	vx;
	velocity 	vy;
} projectileClass;

typedef struct projectile {
	pt				loc;
	char			xDir;				// Needed because we re-use projectileClass.vx for many projectiles
	char			yDir;				// Needed because we re-use projectileClass.vy for many projectiles
	projectileClass *pc;
} projectile;

projectileClass projClasses[MAX_PROJECTILE_CLASSES];
projectile projs[MAX_PROJECTILES];
#endif

/****************************************
 *			File-level variables		*
 ****************************************/
trigCallback trigCb;				// Triggerable bg callback function (client-defined logic)
mutCallback mutCb;					// Mutable bg callback function (client-defined logic)
queryCallback queryCb;				// Queryable bg callback function (client-defined logic)
const animation *animBgTbl;			// Animated bgs' location in flash
const bgAnimIndex *bgAnimDir;		// Animated BG directory in flash
PGM_P *bgMaps;						// Background maps in flash (BGP flag)  **BUG**
const object *objTbl;				// Objects' tile maps in flash
const bgInner *bgiTbl;				// Non-collidable decorative bgs in flash
const bgOuter *bgoTbl;				// Collidable bg containers in flash
const bgDirectory *bgDir;			// Bg directory in flash
const platformDirectory *platDir;	// Moving platform headers
const platform *platTbl;			// Moving platforms' attributes
#if MAX_ANIMATED_BGS
	animatedBgs	animBgs;			// Animated bgs container
#endif

#if MAX_MOVING_PLATFORMS
	platformsCommon cp;				// Common moving platform attributes
	platforms mp;					// Dynamically loaded collection of moving platforms
#endif

u8 wsp;								// World Slice Pointer (points to our current drawing canvas)
u8 wspMax;							// Max value that wsp can validly point to (equal to user's level size-1)
u8 csp;								// Collision Slice Pointer (points to our current collision bounds)
u8 xPrev;							// Playerable character's previous x position
#ifdef VIEWPORT_SLACK
	u8 vpSlack = VIEWPORT_SLACK;	// Viewport slack - offset from center before scrolling begins
#else
	u8 vpSlack = 0;
#endif
u8 vpAnchor;						// Viewport anchor
u8 *ticks[TICK_COUNT];				// Holds pointers to speed frames for updating each game frame
char prevScrDirX;					// The previous scrolling direction
char scrDirX;						// The current scrolling direction
char scrDirAdj;						// Seam drawing directional adjustment
u8 prevScrX;						// The previous scroll position in pixels
u8 scrXMod;							// The current scroll position in tiles


/****************************************
 *		Static function prototypes		*
 ****************************************/

#if MAX_MOVING_PLATFORMS
	void PlatzLoadMovingPlatforms(u8 sp, char dir);
	void PlatzMoveMovingPlatforms(void);
	void PlatzDrawMovingPlatforms(u8);
#endif

#if MAX_ANIMATED_BGS
	void PlatzLoadAnimatedBgs(u8 sp, char dir);
	void PlatzAnimateBgs(void);
	void PlatzDrawAnimatedBgs(void);
#endif
u8 PlatzDetectMovingPlatformCollisions(platzActor *a, char *xDelta);
u8 DetectBgCollisions(platzActor *a, u8 enTrig);
void PlatzDrawColumn(u8 paintX, char dir);
void PlatzUpdateCollisionPointer(platzActor *a);
void PlatzScroll(char xDelta);

/****************************************
 *			Function definitions		*
 ****************************************/

// Trigger callback is a user-defined function to handle game logic when triggers fire
void PlatzSetTriggerCallback(trigCallback tcb) {
	trigCb = tcb;
}

// Mutator callback is a user-defined function to handle mutable background events
void PlatzSetMutCallback(mutCallback mcb) {
	mutCb = mcb;
}

// Query callback is a user-defined function to handle queryable background collisions
void PlatzSetQueryCallback(queryCallback qb) {
	queryCb = qb;
}

#if MAX_MOVING_PLATFORMS
// Contains the data that describes each platform's dimensions and movement
void PlatzSetMovingPlatformTable(const platform *p) {
	platTbl = p;
}

// Points slices to their respective platforms
void PlatzSetMovingPlatformDirectory(const platformDirectory *pDir) {
	platDir = pDir;
}
#endif

#if MAX_ANIMATED_BGS
// Contains data for how to animate a bg
void PlatzSetAnimatedBgTable(const animation *a) {
	animBgTbl = a;
}

void PlatzSetAnimatedBgDirectory(const bgAnimIndex *bgad) {
	bgAnimDir = bgad;
}
#endif

// Points to maps in the bg map table
void PlatzSetMapsTable(PGM_P *m) {
	bgMaps = m;
}

// Contains data for how to display object maps for each slice
void PlatzSetObjectTable(const object *obj) {
	objTbl = obj;
}

// Inner bgs contain display data
void PlatzSetInnerBgTable(const bgInner *bgi) {
	bgiTbl = bgi;
}

// Outer bgs contain collision data and point to their collection of inner bgs
void PlatzSetOuterBgTable(const bgOuter *bgo) {
	bgoTbl = bgo;
}

// A list of where to look for what for each world slice (except platforms, which manage themselves - this may change)
void PlatzSetBgDirectory(const bgDirectory *bgd) {
	bgDir = bgd;
}

void PlatzSetViewport(u8 anchor, u8 slack) {
	vpSlack = slack;
	vpAnchor = anchor;
}

u8 PlatzGetCollisionPointer(void) {
	return csp;
}

u8 PlatzGetWorldSlicePointer(void) {
	return wsp;
}


void PlatzInit(platzActor *a, u8 sliceCount) {
	// Init movement counter
	ticks[0] = &(a->vx.frames);
	ticks[1] = &(a->vy.frames);

#if MAX_MOVING_PLATFORMS
	for (u8 i = 0; i < MAX_VIS_SLICES; i++) {
		for (u8 j = 0; j < MAX_MOVING_PLATFORMS; j++) {
			ticks[2+i*MAX_MOVING_PLATFORMS+j] = &(mp.v[i][j].frames);
		}
	}
#endif

	vpAnchor = a->sprx;
	wspMax = sliceCount-1;
}

#if MAX_MOVING_PLATFORMS
// Initialises global moving platforms structure
void PlatzSetMovingPlatformTiles(u8 hTilesIndex, u8 vTilesIndex, u8 shTilesIndex, u8 svTilesIndex) {
	cp.htiles = hTilesIndex;
	cp.vtiles = vTilesIndex;
	cp.hstepped = shTilesIndex;
	cp.vstepped = svTilesIndex;
}
#endif


// Moves the actor to slice sp and centers the viewport on their sprite position
void PlatzMoveToSlice(platzActor *a, u8 sp) {
	u8 i,j,k,l,wid,hgt,objBegin,objEnd,xOffset,scrX = a->loc.x-vpAnchor;
	bgOuter bgo;
	bgInner bgi,bgm;
	object obj;
	bgDirectory bgd;
	rect vp = (rect){0,32,0,28};	// Init to squared viewport
	rect robj;

	a->sprx = vpAnchor;
	xPrev = a->loc.x;
	prevScrDirX = a->vx.dir;
	scrDirX = a->vx.dir;
	scrDirAdj = (a->vx.dir == DIR_RIGHT)?0:-TILE_WIDTH;
	wsp = csp = sp;

#if MAX_ANIMATED_BGS|MAX_MOVING_PLATFORMS
	u8 sp2;
	char dir;

	if (a->loc.x < vpAnchor) {
		sp2 = (sp)?sp-1:wspMax;
		dir = DIR_LEFT;
	} else {
		sp2 = (sp < wspMax)?sp+1:0;
		dir = DIR_RIGHT;
	}

	#if MAX_ANIMATED_BGS
		PlatzLoadAnimatedBgs(sp,dir);
		PlatzLoadAnimatedBgs(sp2,dir);
	#endif

	#if MAX_MOVING_PLATFORMS
		PlatzLoadMovingPlatforms(sp,dir);
		PlatzLoadMovingPlatforms(sp2,dir);
	#endif
#endif

	// Center viewport on actor
	Screen.scrollX = prevScrX = scrX;
	scrXMod = scrX>>3;
	// Draw sky
	ClearVram();

	for (i = 0; i < MAX_VIS_SLICES; i++) {
		memcpy_P(&bgd,bgDir+sp,sizeof(bgDirectory));

		if (a->loc.x < vpAnchor) {
			if (i) {
				vp.left = scrX>>3;
				vp.right = SCRL_WID>>3;
			} else {
				vp.left = 0;
				vp.right = scrX>>3;
				sp = (sp)?sp-1:wspMax;
				wsp = (a->vx.dir == DIR_RIGHT)?csp:((csp)?csp-1:wspMax);
			}
		} else {
			if (i) {
				vp.left = 0;
				vp.right = scrX>>3;
			} else {
				vp.left = scrX>>3;
				vp.right = SCRL_WID>>3;
				sp = (sp < wspMax)?sp+1:0;
				wsp = (a->vx.dir == DIR_RIGHT)?((csp < wspMax)?csp+1:0):csp;
			}
		}

		// Draw bgs
		for (j = 0; j < bgd.bgoCount; j++) {
			memcpy_P(&bgo,bgoTbl+bgd.bgoIndex+j,sizeof(bgOuter));

			if ((bgo.type&BGI) == 0) {
				for (k = 0; k < bgo.count; k++) {
					if (PlatzRectsIntersect(&bgo.r,&vp)) {
						memcpy_P(&bgi,bgiTbl+bgo.index+k,sizeof(bgInner));

						if ((bgi.type&BGM) && mutCb) {
							++k;
							memcpy_P(&bgm,bgiTbl+bgo.index+k,sizeof(bgInner));

							if (mutCb(PLATZ_MUT_EV_DRAW,&bgi,&bgm,&bgo) == 0)
								continue;
						}

						if ((bgi.type&BGA) == 0) {
							xOffset = MAX(bgi.r.left,vp.left)-bgi.r.left;
							bgi.r.left = MAX(bgi.r.left,vp.left);
							bgi.r.right = MIN(bgi.r.right,vp.right);

							if (bgi.type&BGP) {
								PlatzFillMap(&bgi.r,xOffset,0,(const char*)pgm_read_word(&(bgMaps[bgi.tile])),2);
							} else {
								PlatzFill(&bgi.r,bgi.tile);
							}
						}
					}
				}
			}
		}

		// Draw objects
		for (j = 0; j < bgd.objCount; j++) {
			memcpy_P(&obj,objTbl+bgd.objOffset+j,sizeof(object));
			wid = pgm_read_byte(&(obj.map[0]));
			hgt = pgm_read_byte(&(obj.map[1]));
			robj.left = obj.begin.x;
			robj.top = obj.begin.y;
			robj.right = robj.left+wid;
			robj.btm = robj.top+hgt;

			if (PlatzRectsIntersect(&robj,&vp)) {
				objBegin = MAX(robj.left,vp.left)-robj.left;
				objEnd = MIN(robj.right,vp.right)-robj.left;

				for (k = 0; k < hgt; k++) {
					for (l = objBegin; l < objEnd; l++) {
						//SetTile(robj.left+l,robj.top+k,pgm_read_byte(&(obj.map[k*wid+l+2])));
						inline_set_tile(robj.left+l,robj.top+k,pgm_read_byte(&(obj.map[k*wid+l+2])));
					}
				}
			}
		}
	}
}

// Adjusts the player's bounding box size, anchor point and trigger point in the event that their sprite
// dimensions have changed. Will fail safely if the increased size would result in an overlap with a bg
u8 PlatzSetBoundingBoxDimensions(platzActor *a, u8 wid, u8 hgt) {
	platzActor aTemp = *a;

	aTemp.bbx = wid>>1;
	aTemp.bby = hgt>>1;
	char xVel = (a->vx.dir > 0)?a->bbx-aTemp.bbx:aTemp.bbx-a->bbx;
	char yVel = (a->vy.dir > 0)?a->bby-aTemp.bby:aTemp.bby-a->bby;

	xVel <<= 2;
	yVel <<= 2;
	while (ABS(xVel)&3) (xVel>0)?++xVel:--xVel;	// Ensure modDisp and disp are equal so
	while (ABS(yVel)&3) (yVel>0)?++yVel:--yVel; // that GET_VEL will be independent of frames
	PlatzSetVelocity(&aTemp.vx,xVel,&aTemp.trLoc.x);
	PlatzSetVelocity(&aTemp.vy,yVel,&aTemp.trLoc.y);

	if (DetectBgCollisions(&aTemp,0)) {
		// Cannot switch size safely
		return 0;
	}

	xVel >>= 2;
	yVel >>= 2;
	a->loc.x += xVel;
	a->loc.y += yVel;
	a->trLoc.x -= xVel;
	a->trLoc.y -= yVel;
	a->sprx += xVel;
	a->bbx = aTemp.bbx;
	a->bby = aTemp.bby;
	return 1;
}

// Similar to kernel's MapSprite, but no wid/hgt header to read from flash
void PlatzMapSprite(u8 index, u8 wid, u8 hgt, const char *map, u8 spriteFlags) {
	u8 x,y,xStart,xEnd,xStep;

	if (spriteFlags&SPRITE_FLIP_X) {
		xStart = wid-1;
		xEnd = 255;
		xStep = -1;
	} else {
		xStart = 0;
		xEnd = wid;
		xStep = 1;
	}

	for (y = 0; y < hgt; y++) {
		for (x = xStart; x < xEnd; x += xStep,index++) {
			sprites[index].tileIndex = pgm_read_byte(&(map[(y*wid)+x]));
			sprites[index].flags = spriteFlags;
		}
	}
}

// Moves a sprite array's position offscreen so that it is not drawn
void PlatzHideSprite(u8 spriteIndex, u8 wid, u8 hgt) {
	for (int i = 0; i < (wid*hgt); i++)
		MoveSprite(spriteIndex+i,SCREEN_TILES_H<<3,0,1,1);
	//MoveSprite(spriteIndex,SCREEN_TILES_H<<3,SCREEN_TILES_V<<3,wid,hgt);
}


// Similar to kernel's Fill, but takes a rect
void PlatzFill(const rect *r, u8 tileId) {
	u8 x,y;

	for (y = r->top; y < r->btm; y++) {
		for (x = r->left; x < r->right; x++) {
			//SetTile(x,y,tileId);
			inline_set_tile(x,y,tileId);
		}
	}
}

// Fills a region with a specified pattern. Pattern can be 2-dimensional. Handles partial draws (i.e. when scrolling).
// SLOW_BG_PATTERNS (non 2^n) is MUCH slower (especially for large fill areas)
#if 0 // Replaced by PlatzFillMap
void PlatzFillPattern(const rect *r, u8 patWid, u8 patHgt, u8 patIndex, u8 patOffsetX, const char *map) {
	u8 x,y,fillWid = r->right-r->left,fillHgt = r->btm-r->top;
	u8 xPat,yPat;

	for (y = 0, yPat = 0; y < fillHgt; y++,yPat++) {
		if (yPat == patHgt)
			yPat = 0;

		for (x = 0,xPat = patOffsetX; x < fillWid; x++,xPat++) {
			if (xPat == patWid)
				xPat = 0;
			//SetTile(x+r->left,y+r->top,pgm_read_byte(&(map[patIndex+xPat+yPat*patWid])));
			inline_set_tile(x+r->left,y+r->top,pgm_read_byte(&(map[patIndex+xPat+yPat*patWid])));
		}
	}
}
#endif

// Draws a rectangular region of a map
void PlatzFillMap(const rect *r, u8 xOffset, u8 yOffset, const char *map, int dataOffset) {
	u8 x,y,wid = r->right-r->left,hgt = r->btm-r->top,mapWid = pgm_read_byte(&(map[0])),mapHgt = pgm_read_byte(&(map[1]));
	u8 xMap, yMap;

	// Don't burden non-repeating maps with possible mod overhead. Also allows for non 2^n maps
	// while still having SLOW_BG_PATTERNS undefined
	if ((xOffset > mapWid) || (yOffset > mapHgt)) {
		#ifndef SLOW_BG_PATTERNS
			xOffset &= mapWid-1;
			yOffset &= mapHgt-1;
		#else
			xOffset %= mapWid;
			yOffset %= mapHgt;
		#endif
	}

	// Do in vertical strips as this is mostly called by PlatzDrawColumn
	for (x = 0, xMap = xOffset; x < wid; x++,xMap++) {
		if (xMap == mapWid)
			xMap = 0;
		for (y = 0, yMap = yOffset; y < hgt; y++,yMap++) {
			if (yMap == mapHgt)
				yMap = 0;
			inline_set_tile(x+r->left,y+r->top,pgm_read_byte(&(map[xMap+yMap*mapWid+dataOffset])));
		}
	}
}

// Draws a column of tiles based on the bgs supplied to platz and paintX. Handles initial frame of animated
// bgs in case their animation timer is not in sync. Columns can consist of normal, patterned or animated bgs
// and objects. Usually used at the scrolling edge
void PlatzDrawColumn(u8 paintX, char dir) {
	u8 i,j,k,mpTile,mpTop,mpBtm;
#if MAX_ANIMATED_BGS
	u8 iAnimBg;
#endif
	bgOuter bgo;
	bgInner bgi,bgm;
	object obj;
	bgDirectory bgd;
	memcpy_P(&bgd,bgDir+wsp,sizeof(bgDirectory));

	// Paint sky
	PlatzFill(&(rect){paintX,paintX+1,0,SCREEN_TILES_V-OVERLAY_LINES},0);

	// Paint bgs
	for (i = 0; i < bgd.bgoCount; i++) {
		memcpy_P(&bgo,bgoTbl+bgd.bgoIndex+i,sizeof(bgOuter));

		if ((bgo.type&BGI) == 0) {	// Only draw visible bgs
			if ((bgo.r.left <= paintX) && (bgo.r.right > paintX)) {	// Can skip all inner bgs if outer does not need painting
				for (j = 0; j < bgo.count; j++) {
					memcpy_P(&bgi,bgiTbl+bgo.index+j,sizeof(bgInner));

					if ((bgi.r.left <= paintX) && (bgi.r.right > paintX)) {
						if ((bgi.type&BGM) && mutCb) {
							memcpy_P(&bgm,bgiTbl+bgo.index+j+1,sizeof(bgInner));

							if (mutCb(PLATZ_MUT_EV_DRAW,&bgi,&bgm,&bgo) == 0) {
								++j;	// Skip mutable bg payload
								continue;
							}
						}

						if (bgi.type&BGA) {
#if MAX_ANIMATED_BGS
							if (dir == DIR_RIGHT)
								iAnimBg = 1;
							else
								iAnimBg = 0;

							// Inactive mutable bgs will have been culled above during callback
							for (k = 0; k < animBgs.count[iAnimBg]; k++) {
								if ((animBgs.ids[iAnimBg][k].iOuter == i) && (animBgs.ids[iAnimBg][k].iInner == j))
									PlatzFillMap(&(rect){paintX,paintX+1,animBgs.bgs[iAnimBg][k].r.top,animBgs.bgs[iAnimBg][k].r.btm},paintX-animBgs.bgs[iAnimBg][k].r.left,0,
											animBgs.anims[iAnimBg][k].frames,animBgs.anims[iAnimBg][k].currFrame*animBgs.anims[iAnimBg][k].size+2);
							}
#endif
						} else if (bgi.type&BGP) {
							PlatzFillMap(&(rect){paintX,paintX+1,bgi.r.top,bgi.r.btm},paintX-bgi.r.left,0,(const char*)pgm_read_word(&(bgMaps[bgi.tile])),2);
						} else {
							PlatzFill(&(rect){paintX,paintX+1,bgi.r.top,bgi.r.btm},bgi.tile);
						}
					}

					if (bgi.type&BGM)	// Skip mutable bg payload for unpainted BGM bgs
						++j;
				}
			}
		}
	}

	// Paint objects
	for (i = 0; i < bgd.objCount; i++) {
		memcpy_P(&obj,objTbl+bgd.objOffset+i,sizeof(object));

		if ((obj.begin.x <= paintX) && ((obj.begin.x+pgm_read_byte(&(obj.map[0]))) > paintX)) {
			PlatzFillMap(&(rect){paintX,paintX+1,obj.begin.y,obj.begin.y+pgm_read_byte(&(obj.map[1]))},paintX-obj.begin.x,0,obj.map,2);
		}
	}

	// Paint stepped moving platforms
	i = (mp.slice[0] == wsp)?0:1;

	for (j = 0; j < mp.count[i]; j++) {
		if (mp.p[i][j].clrTile&MP_STEPPED) {
			if (((mp.p[i][j].r.left>>3) <= paintX) && ((mp.p[i][j].r.right>>3) > paintX)) {
				mpTop = mp.p[i][j].r.top>>3;
				mpBtm = mp.p[i][j].r.btm>>3;

				if (mp.p[i][j].axis == AXIS_X) {
					mpTile = cp.hstepped;
				} else {
					mpTile = cp.vstepped;

					// Negative vel can draw one tile off if not multiple of tile height
					if ((mp.v[i][j].dir < 0) && (mp.p[i][j].r.top&7)) {
						++mpTop;
						++mpBtm;
					}
				}

				PlatzFill(&(rect){paintX,paintX+1,mpTop,mpBtm},mpTile);
			}
		}
	}
}


// Source: Sedgewick
// PlatzLinesIntersect helper
char PlatzCcw(const pt16 *p0, const pt16 *p1, const pt16 *p2) {
    long int dx1, dx2, dy1, dy2;
    dx1 = p1->x-p0->x; dy1 = p1->y-p0->y;
    dx2 = p2->x-p0->x; dy2 = p2->y-p0->y;

    if (dx1*dy2 > dy1*dx2) return 1;
    if (dx1*dy2 < dy1*dx2) return -1;
    if ((dx1*dx2 < 0) || (dy1*dy2 < 0)) return -1;
    if ((dx1*dx1+dy1*dy1) < (dx2*dx2+dy2*dy2)) return 1;
    return 0;
}


// Source: Sedgewick
// Determines if two line regions intersect
u8 PlatzLinesIntersect(const line16 *l1, const line16 *l2) {
	return ((PlatzCcw(&l1->p1, &l1->p2, &l2->p1)*PlatzCcw(&l1->p1, &l1->p2, &l2->p2)) <= 0)
        && ((PlatzCcw(&l2->p1, &l2->p2, &l1->p1)*PlatzCcw(&l2->p1, &l2->p2, &l1->p2)) <= 0);
}

// Determines if two rect16's overlap
inline u8 PlatzRectsIntersect16(const rect16 *r1, const rect16 *r2) {
	if ((r1->btm < r2->top) || (r1->right < r2->left) || (r1->left > r2->right) || (r1->top > r2->btm))
		return 0;
	return 1;
/*
	if (r1->btm < r2->top) return 0;
	if (r1->right < r2->left) return 0;
	if (r1->left > r2->right) return 0;
	if (r1->top > r2->btm) return 0;
	return 1;
*/
}

// Determines if two rect's overlap (8-bit)
inline u8 PlatzRectsIntersect(const rect *r1, const rect *r2) {
	if ((r1->btm < r2->top) || (r1->right < r2->left) || (r1->left > r2->right) || (r1->top > r2->btm))
		return 0;
	return 1;

/*
	if (r1->btm < r2->top) return 0;
	if (r1->right < r2->left) return 0;
	if (r1->left > r2->right) return 0;
	if (r1->top > r2->btm) return 0;
	return 1;
*/
}

#if MAX_MOVING_PLATFORMS
// Detects collisions between the player and moving platform. Also "sticks" players to moving platforms
// when the players are not otherwise moving.
u8 PlatzDetectMovingPlatformCollisions(platzActor *a, char *xDelta) {
	u8 i,j,retVal = 0;
	char vel;
	char xVel = GET_VEL(a->vx);
	char yVel = GET_VEL(a->vy);
	rect r = {a->loc.x-a->bbx+xVel,a->loc.x+a->bbx+xVel,a->loc.y-a->bby+yVel,a->loc.y+a->bby+yVel};

#ifdef BCDASH
	if (queryCb) {
		retVal = 1;
		queryCb(0,&retVal);

		if (retVal)
			retVal = 0;
		else
			return retVal;
	}
#endif

	for (i = 0; i < MAX_VIS_SLICES; i++) {
		if (mp.slice[i] != csp) {
			continue;
		}

		for (j = 0; j < mp.count[i]; j++) {
			vel = GET_VEL(mp.v[i][j]);

			if (mp.p[i][j].axis == AXIS_X) {
				if (vel) {
					if (PlatzRectsIntersect(&r,&mp.p[i][j].r)) {
						if (a->vx.dir == mp.v[i][j].dir) {
							if (ABS(a->vx.vel) < ABS(mp.v[i][j].vel)) {
								PlatzSetVelocity(&a->vx,mp.v[i][j].vel,&a->trLoc.x);
							}
						} else {
							PlatzSetVelocity(&a->vx,mp.v[i][j].vel,&a->trLoc.x);
						}

						// Determine exact intersect type
						r.right -= xVel;

						if (r.right < mp.p[i][j].r.left) {
							retVal |= L_INTERSECT;
						} else {
							retVal |= R_INTERSECT;
						}
					} else {
						r.btm++;
						if ((xVel == 0) && PlatzRectsIntersect(&r,&mp.p[i][j].r)) {
							// Stick player to platform surface
							*xDelta += vel;
						}
						r.btm--;
					}
				}
			} else {
				if ((r.right >= mp.p[i][j].r.left) && (r.left < mp.p[i][j].r.right)) {
					if ((r.top >= mp.p[i][j].r.top) && (r.top <= mp.p[i][j].r.btm)) {
						// Platform above player
						a->loc.y = mp.p[i][j].r.btm+a->bby+1+ABS(vel);

						if (yVel < vel) {
							PlatzSetVelocity(&a->vy,mp.v[i][j].vel,&a->trLoc.y);
						}
						retVal = B_INTERSECT;
					} else {
						// Platform below player
						if (vel > 0) {
							if ((r.btm <= (mp.p[i][j].r.btm-vel-2)) && (r.btm >= (mp.p[i][j].r.top-vel-2))) {
								a->loc.y = mp.p[i][j].r.top-a->bby-1;

								if (yVel > 0) {
									PlatzSetVelocity(&a->vy,0,&a->trLoc.y);
								}
								retVal = T_INTERSECT;
							}
						} else {
							if ((r.btm <= (mp.p[i][j].r.btm-1)) && (r.btm >= (mp.p[i][j].r.top-1))) {
								a->loc.y = mp.p[i][j].r.top-a->bby-1;

								if (yVel > 0) {
									PlatzSetVelocity(&a->vy,0,&a->trLoc.y);
								}
								retVal = T_INTERSECT;
							}
						}
					}
				}
			}
		}
	}
	return retVal;
}
#endif


// Relevant to DetectBgCollisions() only
#define DBGC_MODE_PLATS		0
#define DBGC_MODE_NO_SEAM	1
#define DBGC_MODE_SEAM_CURR	2
#define DBGC_MODE_SEAM_NEXT	3

// Detects collisions between the player and background elements and adjusts their speed to avoid these collisions.
// Also handles firing event triggers.
u8 DetectBgCollisions(platzActor *a, u8 enTrig) {
	u8 j,retVal = 0,colVal = 0,sp = 0,loops = 0,loopMax = 0,mode = 0;
#ifdef SMALL_WORLD
	u8 i,inc,start,fin;
#else
	u16 i,inc,start,fin;
#endif
#if MAX_MOVING_PLATFORMS
	u8 mpIndex = 0;
#endif
	char trig,collDir,overlap,xVel = GET_VEL(a->vx),yVel = GET_VEL(a->vy);
	rect16 rPre,rPost,rBg,rColl,rTrig;
	line16 lMove,lBg;
	pt pos = (pt){a->loc.x+a->trLoc.x,a->loc.y+a->trLoc.y};
	bgOuter bgo;
	bgDirectory bgd;
	bgInner bgi,bgm;

	// Pre-use rPost to save allocating another variable
	rPost.left = (int)(a->loc.y)+(int)yVel;

	if ((rPost.left+a->bby) > (PLATZ_SCRN_HGT-(OVERLAY_LINES<<3)-1)) {
		// Collision with bottom of screen
		yVel = PLATZ_SCRN_HGT-(OVERLAY_LINES<<3)-a->bby-a->loc.y-1;
		retVal |= T_INTERSECT;
	} else if ((rPost.left-a->bby) < 0) {
		// Collision with top of screen
		yVel = -(a->loc.y-a->bby);
		retVal |= B_INTERSECT;
	}

	rPre.left = a->loc.x-a->bbx;
	rPre.right = a->loc.x+a->bbx;
	rPre.top = a->loc.y-a->bby;
	rPre.btm = a->loc.y+a->bby;
	rPost.left = rPre.left+xVel;
	rPost.right = rPre.right+xVel;
	rPost.top = rPre.top+yVel;
	rPost.btm = rPre.btm+yVel;

	if ((rPre.left >= 0) && (rPost.left >= 0) && (rPre.right <= (SCRL_WID-1)) && (rPost.right <= (SCRL_WID-1))) {
		// Not on seam - test slice that player is on
		overlap = 0;
		loopMax = 2;
#if MAX_MOVING_PLATFORMS
		mode = DBGC_MODE_PLATS;
#else
		mode = DBGC_MODE_NO_SEAM;
#endif
		collDir = 0;
		sp = csp;
	} else {
		// On seam - test both slices
		loopMax = 2;
		mode = DBGC_MODE_SEAM_CURR;
	}

	// Only need to check through once on each slice IF bgs are tile-aligned AND GET_VEL <= MIN(TILE_HEIGHT,TILE_WIDTH)
	while (loops++ < loopMax) {
		if (mode == DBGC_MODE_SEAM_CURR) {
			if ((rPost.left < 0) || (rPost.right > (SCRL_WID-1))) {
				if (a->loc.x > (SCRL_WID>>1)) {
					rPost.left -= SCRL_WID;
					rPost.right -= SCRL_WID;
					collDir = 1;
					sp = (csp < wspMax)?csp+1:0;
				} else {
					rPost.left += SCRL_WID+1;	// SCRL_WID would force a false positive due to 0 == 256
					rPost.right += SCRL_WID+1;	// Doesn't occur when going right because max xLoc is 255
					collDir = -1;
					sp = (csp)?csp-1:wspMax;
				}

				if ((rPost.left > (SCRL_WID>>1)) && (rPre.left < (SCRL_WID>>1))) {
					overlap = 1;
				} else if ((rPost.left < (SCRL_WID>>1)) && (rPre.left > (SCRL_WID>>1))) {
					overlap = -1;
				} else {
					overlap = 0;
				}
			} else {
				++mode;
				continue;
			}
		} else if (mode == DBGC_MODE_SEAM_NEXT) {
			sp = csp;
			collDir = -collDir;
			rPost.left = rPre.left+xVel;
			rPost.top = rPre.top+yVel;
			rPost.right = rPre.right+xVel;
			rPost.btm = rPre.btm+yVel;

			if ((rPost.left > (SCRL_WID>>1)) && (rPre.left < (SCRL_WID>>1))) {
				overlap = 1;
			} else if ((rPost.left < (SCRL_WID>>1)) && (rPre.left > (SCRL_WID>>1))) {
				overlap = -1;
			} else {
				overlap = 0;
			}
		}

		memcpy_P(&bgd, bgDir+sp, sizeof(bgDirectory));
		inc = 1;

#if MAX_MOVING_PLATFORMS
		if (mode == DBGC_MODE_PLATS) {
			start = 0;

			if (mp.slice[0] == sp) {
				fin = mp.count[0];
				mpIndex = 0;
			} else if (mp.slice[1] == sp) {
				fin = mp.count[1];
				mpIndex = 1;
			} else {
				fin = 0;
			}
		} else if (collDir > 0) {
#else
		if (collDir > 0) {
#endif
			start = bgd.bgoIndex;
			fin = start+bgd.bgoBeginCount;
		} else if (collDir < 0) {
			//start = bgd.bgoIndex+bgd.bgoEndIndex;
			//fin = start+bgd.bgoEndCount;
			start = bgd.bgoIndex+bgd.bgoBeginCount-bgd.bgoCommonCount;

			if (bgd.bgoEndIndex == 0)	// Set to zero when no distinct end bgs
				fin = start+bgd.bgoBeginCount;
			else
				fin = bgd.bgoIndex+bgd.bgoCount;	// Manage begin/end section in for loop
		} else {
			if (a->vx.dir == DIR_RIGHT) {
				start = bgd.bgoIndex;
				fin = start+bgd.bgoCount;
			} else {
				start = bgd.bgoIndex+bgd.bgoCount-1;
				fin = bgd.bgoIndex-1;
				inc = -1;
			}
		}

		for (i = start, j = 0; i != fin; i+=inc) {
			colVal = 0;

			// When testing end seam, skip middle bgs
			if ((collDir < 0) && (i == bgd.bgoBeginCount))
				i = bgd.bgoEndIndex;

#if MAX_MOVING_PLATFORMS
			if (mode == DBGC_MODE_PLATS) {
				rBg.left = mp.p[mpIndex][j].r.left;
				rBg.right = mp.p[mpIndex][j].r.right;
				rBg.top = mp.p[mpIndex][j].r.top;
				rBg.btm = mp.p[mpIndex][j++].r.btm;
			} else {
				memcpy_P(&bgo,bgoTbl+i,sizeof(bgOuter));
				rBg.left = (bgo.r.left<<3);
				rBg.right = (bgo.r.right<<3);
				rBg.top = (bgo.r.top<<3);
				rBg.btm = (bgo.r.btm<<3);
			}
#else
			memcpy_P(&bgo,bgoTbl+i,sizeof(bgOuter));
			rBg.left = (bgo.r.left<<3);
			rBg.right = (bgo.r.right<<3);
			rBg.top = (bgo.r.top<<3);
			rBg.btm = (bgo.r.btm<<3);
#endif

			if (PlatzRectsIntersect16(&rPost,&rBg)) {
				rColl.left = MAX(rPost.left,rBg.left);
				rColl.right = MIN(rPost.right,rBg.right);
				rColl.top = MAX(rPost.top,rBg.top);
				rColl.btm = MIN(rPost.btm,rBg.btm);

				// Process triggers
				if ((mode != DBGC_MODE_PLATS) && (bgo.type&BGT)) {
					if (enTrig) {
						if (!PT_NOT_IN_RECT(pos,rBg)) {
							rTrig = rBg;

							if ((rTrig.btm-rTrig.top) < (rTrig.right-rTrig.left)) {
								rTrig.btm -= (rTrig.btm-rTrig.top)>>1;
							} else {
								rTrig.right -= (rTrig.right-rTrig.left)>>1;
							}

							if (!PT_NOT_IN_RECT(pos,rTrig)) {
								// Unused bgOuter.count in triggers used for firing orientation (not axis)
								trig = (bgo.count)?-1:1;
							} else {
								trig = (bgo.count)?1:-1;
							}

							trigCb(bgo.index,bgo.type,trig);
						}
					}
				} else if ((mode == DBGC_MODE_PLATS) || (bgo.type&(BGC|BGM))) {
					if ((mode != DBGC_MODE_PLATS) && (bgo.type&BGM) && mutCb) {	// Awkward - may try to rephrase these conditionals
						memcpy_P(&bgi,bgiTbl+bgo.index,sizeof(bgInner));
						memcpy_P(&bgm,bgiTbl+bgo.index+1,sizeof(bgInner));

						if (mutCb(PLATZ_MUT_EV_COLLISION,&bgi,&bgm,&bgo) == 0)
							continue;
					}

					if (xVel == 0) {
						colVal = H_INTERSECT;
					} else if (yVel == 0) {
						colVal = V_INTERSECT;
					} else {
						lMove.p1.x = (xVel > 0)?rPre.right+((int)overlap<<SCRL_WID_SHFT):rPre.left+((int)overlap<<SCRL_WID_SHFT);
						lMove.p1.y = (yVel > 0)?rPre.btm:rPre.top;
						lMove.p2.x = lMove.p1.x+xVel;
						lMove.p2.y = lMove.p1.y+yVel;

						if ((lMove.p1.x >= rBg.left) && (lMove.p1.x < rBg.right)) {
							colVal = 12;
						} else if ((lMove.p1.y >= rBg.top) && (lMove.p1.y < rBg.btm)) {
							colVal = 3;
						} else if ((PT_NOT_IN_RECT(lMove.p2,rBg)) && (((lMove.p1.x < rBg.left) || (lMove.p1.x >= rBg.right))
								&& ((lMove.p1.y >= rBg.btm) || (lMove.p1.y < rBg.top)))) {
							if (xVel > 0) {
								colVal = (rPost.right < rBg.right)?V_INTERSECT:H_INTERSECT;
							} else {
								colVal = (rPost.left > rBg.left)?V_INTERSECT:H_INTERSECT;
							}
						} else {
							lBg.p1.x = rBg.left;
							lBg.p2.x = rBg.right;

							if (lMove.p1.y < rBg.top) {
								lBg.p1.y = rBg.top;
								lBg.p2.y = rBg.top;
							} else {
								lBg.p1.y = rBg.btm;
								lBg.p2.y = rBg.btm;
							}

							if (PlatzLinesIntersect(&lMove,&lBg)) {
								// Top/Btm intersect
								colVal = H_INTERSECT;
							} else {
								// Left/Right intersect
								colVal = V_INTERSECT;
							}
						}
					}

					// Query collision callback
					if (queryCb && (bgo.type&BGQ))
						queryCb(&bgo,&colVal);

#ifdef BCDASH
					if (queryCb && (mode == DBGC_MODE_PLATS))
						queryCb(0,&colVal);
#endif

					if (colVal&V_INTERSECT) {
						xVel -= NORMALIZE(xVel)+NORMALIZE(xVel)*(rColl.right-rColl.left);
					} else if (colVal&H_INTERSECT) {
						yVel -= NORMALIZE(yVel)+NORMALIZE(yVel)*(rColl.btm-rColl.top);
					}

					rPost.left = rPre.left+xVel;
					rPost.top = rPre.top+yVel;
					rPost.right = rPre.right+xVel;
					rPost.btm = rPre.btm+yVel;

					if (mode == DBGC_MODE_SEAM_CURR) {
						if (a->loc.x > (SCRL_WID>>1)) {
							rPost.left -= SCRL_WID;
							rPost.right -= SCRL_WID;
							collDir = 1;
							sp = (csp < wspMax)?csp+1:0;
						} else {
							rPost.left += SCRL_WID;
							rPost.right += SCRL_WID;
							collDir = -1;
							sp = (csp)?csp-1:wspMax;
						}
					}

					// Determine exact intersect type
					if (colVal&V_INTERSECT) {
						// Horizontal
						if (rPre.right < rBg.left) {
							// Left intersect
							retVal |= L_INTERSECT;
						} else {
							// Right intersect
							retVal |= R_INTERSECT;
						}
					} else if (colVal&H_INTERSECT) {
						// Vertical
						if (rPre.btm < rBg.top) {
							// Top intersect
							retVal |= T_INTERSECT;
						} else {
							// Btm intersect
							retVal |= B_INTERSECT;
						}
					}
				}
			}
		}
		++mode;
	}

	// Commit altered speeds
	if (retVal&V_INTERSECT) {
		PlatzSetVelocity(&a->vx,xVel<<2,&a->trLoc.x);
	}

	if (retVal&H_INTERSECT) {
		PlatzSetVelocity(&a->vy,yVel<<2,&a->trLoc.y);
	}

	return retVal;
}


// 0.25 pixels per frame (ppf) increments - avoids floating point math and lib bloat
void PlatzSetVelocity(velocity *v, char val, u8 *trPos) {
	char dir = v->dir;
	u8 base,temp,vel;

	if (v->vel == 0) {
		v->frames = 0;
	}

	v->vel = val;
	vel = ABS(val);
	base = vel>>2;
	temp = vel&3;
	v->mod = (temp&1) ? 3 : 1;
	v->modDisp = ((temp == 1) || (temp == 2)) ? base + 1 : base;
	v->disp = (temp == 3) ? base + 1 : base;

	if (v->vel < 0) {
		v->dir = -1;
		v->modDisp *= -1;
		v->disp *= -1;
	} else if (v->vel > 0) {
		v->dir = 1;
	} // else 0 - leave it as it was

	// Keep trigger pos relative to actor's location/direction
	if ((trPos) && (v->dir != dir)) {
		*trPos *= -1;
	}
}

#if MAX_MOVING_PLATFORMS

#define PD_INDEX_OFFSET 11	// Byte offset of pdIndex in bgDirectory
// Loads all moving platforms for the visible slices. These currently manage their own pointers, but
// this is expensive - may change to bgDirectory pointing to relevant platforms
void PlatzLoadMovingPlatforms(u8 sp, char dir) {
	u8 i,src,dest,pdIndex;
	char *p = (char*)bgDir;	// Only want pdIndex from bg directory
	platformDirectory pd;

	if (dir == DIR_RIGHT) {
		src = 1;
		dest = 0;
	} else {
		src = 0;
		dest = 1;
	}

	// Preserve platforms that are still visible
	mp.slice[dest] = mp.slice[src];
	mp.count[dest] = mp.count[src];
	memcpy(mp.v[dest],mp.v[src],MAX_MOVING_PLATFORMS*sizeof(velocity));
	memcpy(mp.p[dest],mp.p[src],MAX_MOVING_PLATFORMS*sizeof(platform));
	// Check next slice for platforms
	pdIndex = pgm_read_byte(p+((sp*sizeof(bgDirectory))+PD_INDEX_OFFSET));

	if (pdIndex != PF_ZERO) {
		// Load platforms
		memcpy_P(&pd,platDir+pdIndex,sizeof(platformDirectory));
		mp.count[src] = pd.count;
		mp.slice[src] = sp;
		memcpy_P(&mp.p[src],platTbl+pd.index,pd.count*sizeof(platform));

		for (i = 0; i < pd.count; i++) {
			PlatzSetVelocity(&mp.v[src][i],mp.p[src][i].v,0);
		}
	} else {
		// Mark as no platforms
		mp.count[src] = 0;
		mp.slice[src] = sp;
	}
}

// Updates direction and position of platforms that are in ram
void PlatzMoveMovingPlatforms(void) {
	u8 i,j,*near,*far;
	char vel;

	for (i = 0; i < MAX_VIS_SLICES; i++) {
		for (j = 0; j < mp.count[i]; j++) {
			vel = GET_VEL(mp.v[i][j]);

			if (vel) {
				near = (mp.p[i][j].axis == AXIS_X)?&mp.p[i][j].r.left:&mp.p[i][j].r.top;
				far = (mp.p[i][j].axis == AXIS_X)?&mp.p[i][j].r.right:&mp.p[i][j].r.btm;

				if ((*near >= mp.p[i][j].max) || (*near <= mp.p[i][j].min)) {
					mp.v[i][j].vel *= -1;
					PlatzSetVelocity(&mp.v[i][j],mp.v[i][j].vel,0);
					vel = GET_VEL(mp.v[i][j]);
				}
				*near += vel;
				*far += vel;
			}
		}
	}
}


// Relevant to PlatzDrawMovingPlatforms() only
#define FLAG_NEAR 	1
#define FLAG_FAR	2

// Draws all platforms that should be at least partially visible
void PlatzDrawMovingPlatforms(u8 axis) {
	u8 i,j,k,offset,hgt,tile,*near,*far,scrX = Screen.scrollX>>3,tiles = 0,clear = 0,op = 0,len = 0;
	rect r,rc;

	for (i = 0; i < MAX_VIS_SLICES; i++) {
		for (j = 0; j < mp.count[i]; j++) {
#ifdef MP_HACK
			if (mp.p[i][j].axis != axis)	// Note: This and the need for an axis param are a hack fix to the vertical axis visual off-by-one bug
				continue;
#endif
			r.top = mp.p[i][j].r.top>>3;
			r.btm = mp.p[i][j].r.btm>>3;
			r.left = mp.p[i][j].r.left>>3;
			r.right = mp.p[i][j].r.right>>3;
			offset = (mp.p[i][j].axis == AXIS_X)?mp.p[i][j].r.left&7:mp.p[i][j].r.top&7;
			clear = 0;
			op = (offset)?1:0;

			if (op && (mp.p[i][j].clrTile&MP_STEPPED))	// Only draw stepped platforms when square
				continue;

			if (mp.p[i][j].axis == AXIS_X) {
				near = &rc.left;
				far = &rc.right;

				if (i == 0) {	// Left slice
					if (scrX <= (r.right+op)) {
						if (mp.v[i][j].dir == DIR_RIGHT) {
							if (scrX >= r.left) {
								r.left = scrX;
								r.right += op;
							} else {
								if (offset == 0) {
									if (mp.p[i][j].r.left != mp.p[i][j].min) {
										clear = FLAG_NEAR;
									}
								} else {
									r.right++;
								}
							}
						} else {
							if (offset == 0) {
								if (mp.p[i][j].r.left != mp.p[i][j].max) {
									clear = FLAG_FAR;
								}
							} else {
								r.right++;
							}

							if (scrX >= r.left) {
								r.left = scrX;
							}
						}
					} else {	// Platform offscreen
						continue;
					}
				} else {	// Right slice
					if (scrX >= r.left) {
						if (mp.v[i][j].dir == DIR_RIGHT) {
							if (offset == 0) {
								if (mp.p[i][j].r.left != mp.p[i][j].min) {
									clear = FLAG_NEAR;
								}
							}

							if (scrX < (r.right+op)) {
								r.right = scrX;
							} else if (offset) {
								r.right++;
							}
						} else {
							if (scrX <= (r.right+op)) {
								r.right = scrX;
							} else {
								if (offset == 0) {
									if (mp.p[i][j].r.left != mp.p[i][j].max) {
										clear = FLAG_FAR;
									}
								} else {
									r.right++;
								}
							}
						}
					} else {	// Platform offscreen
						continue;
					}
				}
			} else {
				near = &rc.top;
				far = &rc.btm;

				if (i == 0) {
					if (scrX <= r.right) {
						r.left = MAX(r.left,scrX);
					} else {
						continue;
					}
				} else {
					if (scrX >= r.left) {
						r.right = MIN(r.right,scrX);
					} else {
						continue;
					}
				}

				if (offset == 0) {
					if ((mp.v[i][j].dir > 0) && (mp.p[i][j].r.top != mp.p[i][j].min)) {
						clear = FLAG_NEAR;
					} else if ((mp.v[i][j].dir < 0) && (mp.p[i][j].r.top != mp.p[i][j].max)) {
						clear = FLAG_FAR;
					}
				} else {
					r.btm++;
				}
			}

			if (clear) {
				rc = r;

				if (clear&FLAG_NEAR) {
					clear = 1;
					*far = *near;
					(*near)--;
				} else {
					clear = 1;
					*near = *far;
					(*far)++;
				}
				PlatzFill(&rc,mp.p[i][j].clrTile&0x3f);
			}

			// Drawing - point to appropriate sides and tiles for this axis
			if (mp.p[i][j].axis == AXIS_X) {
				near = &r.left;
				far = &r.right;
				tiles = cp.htiles;
			} else {
				near = &r.top;
				far = &r.btm;
				tiles = cp.vtiles;
			}

			if (mp.p[i][j].clrTile&MP_STEPPED) {
				tiles = (mp.p[i][j].axis == AXIS_X)?cp.hstepped:cp.vstepped;
			}

			hgt = (*far)-(*near);
			*far = (*near)+1;
			len = (mp.p[i][j].r.right>>3)-(mp.p[i][j].r.left>>3)+((offset)?1:0);

			for (k = 0; k < hgt; k++) {
				if ((mp.p[i][j].axis == AXIS_X) && (hgt < len)) {
					if (offset == 0) {
						tile = tiles;
					} else if (hgt == 1) {
						if (i == 0) {
							// Right end of platform
							tile = tiles+15+offset-1;
						} else {
							// Left end of platform
							tile = tiles+offset;
						}
					} else {
						if (k == 0) {
							if (i == 0) {
								// Middle of platform
								tile = tiles+8+offset-1;
							} else {
								// Left end of platform
								tile = tiles+offset;
							}
						} else if ((k == (hgt-1)) && (i == 0)) {
							// Right end of platform
							tile = tiles+15+offset-1;
						} else {
							// Middle of platform
							tile = tiles+8+offset-1;
						}
					}
				} else {
					if (offset == 0) {
						tile = tiles;				// Square
					} else if (k == 0) {
						tile = tiles+offset;		// Near end
					} else if (k == (hgt-1)) {
						tile = tiles+15+offset-1;	// Far end
					} else {
						tile = tiles+8+offset-1;	// Middle
					}
				}

				PlatzFill(&r,tile);
				(*near)++;
				(*far)++;
			}
		}
	}
}
#endif // #if MAX_MOVING_PLATFORMS

#if MAX_ANIMATED_BGS

#define BGO_INDEX_OFFSET 	3	// Byte offset of bgoIndex in bgDirectory
#define AC_INDEX_OFFSET		9	// Byte offset of animCount in bgDirectory
#define AI_INDEX_OFFSET		10	// Byte offset of animIndex in bgDirectory
#define BGI_INDEX_OFFSET	2	// Byte offset of bgInner index in bgOuter

// Loads all animated bgs for the visible slices.
void PlatzLoadAnimatedBgs(u8 sp, char dir) {
	u8 i,j,animIndex,animCount,src,dest;
	char *byteGrab = (char*)bgDir;	// Only want a couple of bytes from bg directory
	u16 bgoIndex,bgiIndex;
	bgAnimIndex bgai;

	if (dir == DIR_RIGHT) {
		src = 1;
		dest = 0;
	} else {
		src = 0;
		dest = 1;
	}

	// Preserve animations that are still visible
	animBgs.count[dest] = animBgs.count[src];
	memcpy(animBgs.ids[dest],animBgs.ids[src],MAX_ANIMATED_BGS*sizeof(bgAnimIndex));
	memcpy(animBgs.bgs[dest],animBgs.bgs[src],MAX_ANIMATED_BGS*sizeof(bgInner));
#ifdef MUTABLE_BGS
	memcpy(animBgs.mutBgs[dest],animBgs.mutBgs[src],MAX_ANIMATED_BGS*sizeof(bgInner));
#endif
	memcpy(animBgs.anims[dest],animBgs.anims[src],MAX_ANIMATED_BGS*sizeof(animation));
	// Check next slice for animations
	animCount = pgm_read_byte(byteGrab+((sp*sizeof(bgDirectory))+AC_INDEX_OFFSET));

	if (animCount) {
		animBgs.count[src] = animCount;
		animIndex = pgm_read_byte(byteGrab+((sp*sizeof(bgDirectory))+AI_INDEX_OFFSET));
		bgoIndex = pgm_read_word(byteGrab+((sp*sizeof(bgDirectory))+BGO_INDEX_OFFSET));

		for (i = 0; i < animCount; i++) {
			memcpy_P(&bgai,bgAnimDir+animIndex+i,sizeof(bgAnimIndex));
			animBgs.ids[src][i] = bgai;
			byteGrab = (char*)(bgoTbl+bgoIndex+bgai.iOuter);
			bgiIndex = pgm_read_word(byteGrab+BGI_INDEX_OFFSET);
			memcpy_P(&animBgs.bgs[src][i],bgiTbl+bgiIndex+bgai.iInner,sizeof(bgInner));
#ifdef MUTABLE_BGS
			if (animBgs.bgs[src][i].type&BGM)
				memcpy_P(&animBgs.mutBgs[src][i],bgiTbl+bgiIndex+bgai.iInner+1,sizeof(bgInner));
#endif
			memcpy_P(&animBgs.anims[src][i],animBgTbl+animBgs.bgs[src][i].tile,sizeof(animation));

			if (animBgs.anims[src][i].synch) {
				for (j = 0; j < animBgs.count[dest]; j++) {
					if (animBgs.anims[dest][j].synch && (animBgs.anims[src][i].frames == animBgs.anims[dest][j].frames)) {
						animBgs.anims[src][i].currFrame = animBgs.anims[dest][j].currFrame;
						animBgs.anims[src][i].disp = animBgs.anims[dest][j].disp;
					}
				}
			}
		}
	} else {
		animBgs.count[src] = 0;
	}
}

// Draws new frames for animated bgs that are visible. Handles partial visibility
// for patterned and tiled animations.
void PlatzDrawAnimatedBgs(void) {
	u8 i,j,patOffsetX;
	rect r;
	bool redraw;
	u8 scrX = Screen.scrollX>>3;

	for (i = 0; i < MAX_VIS_SLICES; i++) {
		for (j = 0; j < animBgs.count[i]; j++) {
#ifdef MUTABLE_BGS
			if ((animBgs.bgs[i][j].type&BGM) && mutCb)
				if (mutCb(PLATZ_MUT_EV_ANIM,&animBgs.bgs[i][j],&animBgs.mutBgs[i][j],&animBgs.anims[i][j]) == 0)
					continue;
#endif
			if (animBgs.anims[i][j].disp == 0) {
				redraw = false;
				r = animBgs.bgs[i][j].r;

				if (i) {
					// Right slice
					if (scrX > r.left) {
						patOffsetX = 0;
						r.right = MIN(r.right,scrX);
						redraw = true;
					}
				} else {
					// Left slice
					if (scrX < r.right) {
#ifndef SLOW_BG_PATTERNS
						patOffsetX = (scrX > r.left)?(scrX-r.left)&(animBgs.anims[i][j].wid-1):0;
#else
						patOffsetX = (scrX > r.left)?(scrX-r.left)%(animBgs.anims[i][j].wid):0;
#endif
						r.left = MAX(r.left,scrX);
						redraw = true;
					}
				}

				// Draw new frame
				if (redraw) {
					PlatzFillMap(&r,patOffsetX,0,animBgs.anims[i][j].frames,animBgs.anims[i][j].currFrame*animBgs.anims[i][j].size+2);
				}
			}
		}
	}
}

// Increments frames for animated bgs that are in ram
void PlatzAnimateBgs(void) {
	u8 i,j;

	for (i = 0; i < MAX_VIS_SLICES; i++) {
		for (j = 0; j < animBgs.count[i]; j++) {
			if (++(animBgs.anims[i][j].disp) == animBgs.anims[i][j].dpf) {
				// Advance animation frames
				animBgs.anims[i][j].disp = 0;
				animBgs.anims[i][j].currFrame++;

				if (animBgs.anims[i][j].currFrame == animBgs.anims[i][j].frameCount) {
					animBgs.anims[i][j].currFrame = 0;
				}
			}
		}
	}
}
#endif // #if MAX_ANIMATED_BGS

// Maintains collision pointer as player moves across slice seams
void PlatzUpdateCollisionPointer(platzActor *a) {
	// Update collision slice pointer
	if ((a->loc.x>>3) == 0) {
		if ((xPrev>>3) == 31) {
			csp = (csp < wspMax)?csp+1:0;
		}
	} else if ((a->loc.x>>3) == 31) {
		if ((xPrev>>3) == 0) {
			csp = (csp)?csp-1:wspMax;
		}
	}
	xPrev = a->loc.x;
}

// Manages drawing on the scrolling edge
void PlatzScroll(char xDelta) {
	// Update world slice pointer
	u8 scrX = Screen.scrollX;

	if (scrX != prevScrX) {
		prevScrX = scrX;

		// Direction change
		if (scrDirX != prevScrDirX) {
			prevScrDirX = scrDirX;

			if (scrDirX == DIR_RIGHT) {
				wsp = (wsp < wspMax)?wsp+1:0;
				scrDirAdj = 0;
			} else if (scrDirX == DIR_LEFT) {
				wsp = (wsp)?wsp-1:wspMax;
				scrDirAdj = -TILE_WIDTH;
			}
		}

		// Tile column scroll boundary reached
		if ((scrX>>3) != scrXMod) {
			scrXMod = scrX>>3;

			if ((scrXMod == 31) && (scrDirX == DIR_LEFT)) {
				wsp = (wsp)?wsp-1:wspMax;
#if MAX_ANIMATED_BGS
				PlatzLoadAnimatedBgs(wsp,DIR_LEFT);
#endif

#if MAX_MOVING_PLATFORMS
				PlatzLoadMovingPlatforms(wsp,DIR_LEFT);
#endif
			}

			PlatzDrawColumn((u8)(scrX-xDelta+scrDirAdj)>>3,scrDirX);

			if ((scrXMod == 0) && (scrDirX == DIR_RIGHT)) {
				wsp = (wsp < wspMax)?wsp+1:0;
#if MAX_ANIMATED_BGS
				PlatzLoadAnimatedBgs(wsp,DIR_RIGHT);
#endif

#if MAX_MOVING_PLATFORMS
				PlatzLoadMovingPlatforms(wsp,DIR_RIGHT);
#endif
			}
		}
	}

#if MAX_MOVING_PLATFORMS
	PlatzDrawMovingPlatforms(AXIS_X);
#endif

#if MAX_ANIMATED_BGS
	PlatzDrawAnimatedBgs();
	PlatzAnimateBgs();
#endif
}

// When called each frame by Platz clients, will route content management,
// collision, drawing and scrolling processing
u8 PlatzMove(platzActor *a) {
	u8 collFlag = 0;
	char xVel = GET_VEL(a->vx),yVel = GET_VEL(a->vy),xDelta = 0;

	collFlag |= DetectBgCollisions(a,1);

#ifdef MP_HACK
	#if MAX_MOVING_PLATFORMS
		PlatzDrawMovingPlatforms(AXIS_Y);
	#endif
#endif

#if MAX_MOVING_PLATFORMS
	PlatzMoveMovingPlatforms();	// Have to do in here to ensure safe movement
	collFlag |= PlatzDetectMovingPlatformCollisions(a,&xDelta);
#endif

	// Update player and camera positions
	xVel = GET_VEL(a->vx);
	yVel = GET_VEL(a->vy);

	if (xVel) {
		xDelta = xVel;
	}

	if (xDelta) {
		a->loc.x += xDelta;
		scrDirX = (xDelta > 0)?DIR_RIGHT:DIR_LEFT;

		if ((xDelta > 0) && ((a->sprx+xDelta) < (vpAnchor+vpSlack))) {
			a->sprx += xDelta;
		} else if ((xDelta < 0) && ((a->sprx+xDelta) > (vpAnchor-vpSlack))) {
			a->sprx += xDelta;
		} else {
			Screen.scrollX += xDelta;
		}
	} else {
		scrDirX = DIR_NONE;
	}

	if (yVel) {
		a->loc.y += yVel;
	}

	PlatzUpdateCollisionPointer(a);
	PlatzScroll(xDelta);
	return collFlag;
}

// Progresses movement counters
void PlatzTick(void) {
	for (u8 i = 0; i < TICK_COUNT; i++) {
		*ticks[i] += 1;
	}
}

//////////////Projectiles are WIP///////////////////

#if MAX_PROJECTILES&MAX_PROJECTILE_CLASSES
u8 projClassCount;
u8 projCount;

// There's no RemoveProjectile function because it takes up more flash; makes it more difficult to use;
// and will probably not be used much anyway
char PlatzAddProjectileClass(u8 type, u8 vx, u8 vy) {
	if (projClassCount < MAX_PROJECTILE_CLASSES) {
		projClass[projClassCount].type = type;
		PlatzSetVelocity(&projClass[projClassCount].vx,vx,0);
		PlatzSetVelocity(&projClass[projClassCount].vy,vy,0);
		++projClassCount;
		return 1;
	}
	return 0;
}


char PlatzCreateProjectile(pt loc, char xDir, char yDir, u8 pClass) {
	if ((projCount < MAX_PROJECTILES) && (pClass < projClassCount)) {
		for (u8 i = 0; i < MAX_PROJECTILES; i++) {
			if (projs[i].pc == 0) {
				projs[i].loc = loc;
				projs[i].xDir = xDir;
				projs[i].yDir = yDir;
				projs[i].pc = &projClass[pClass];
				++projCount;
				return 1;
			}
		}
	}
	return 0;
}


void PlatzDestroyProjectile(projectile *p) {
	if (p->pc) {
		p->pc = 0;
		--projCount;
	}
}


void PlatzMoveProjectiles(void) {
	if (projCount) {	// Most of the time, there will be none
		for (u8 i = 0; i < MAX_PROJECTILES; i++) {
			if (projs[i].pc) {
				projs[i].loc.x += projs[i].xDir*ABS(GET_VEL(projs[i].pc->vx));
				projs[i].loc.y += projs[i].yDir*ABS(GET_VEL(projs[i].pc->vy));


			}
		}
	}
}
#endif



