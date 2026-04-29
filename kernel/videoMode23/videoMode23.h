/*
 * videoMode23.h
 *
 * Video Mode 23: Arduboy-native vertical work buffer + horizontal scanout
 *
 * 128x64, 1bpp
 *
 * Double buffer model:
 *	sBuffer[1024]        (C draw buffer, native Arduboy vertical page layout)
 *	front_buffer[1024]   (displayed buffer, internal horizontal row-major)
 *
 * Work-buffer layout (native Arduboy):
 *	byte index = x + ((y >> 3) * 128)
 *	bit mask   = 1 << (y & 7)
 *
 * Display():
 *	converts sBuffer into the horizontal front buffer once per frame
 *
 * Color semantics (Arduboy2-style):
 *	0 = BLACK  (clear)
 *	1 = WHITE  (set)
 *	2 = INVERT (toggle)
 */

#ifndef VIDEOMODE23_H
#define VIDEOMODE23_H

#include <uzebox.h>


/* ------------------------------------------------------------------------- */
/* Mode constants */
/* ------------------------------------------------------------------------- */

#ifndef VM23_WIDTH
#define VM23_WIDTH			128
#endif

#ifndef VM23_HEIGHT
#define VM23_HEIGHT			64
#endif

#ifndef VM23_STRIDE
#define VM23_STRIDE			(VM23_WIDTH / 8)	/* 16 */
#endif

#ifndef VM23_BUFFER_SIZE
#define VM23_BUFFER_SIZE	(VM23_STRIDE * VM23_HEIGHT)	/* 1024 */
#endif

/* Arduboy-style colors */
#ifndef BLACK
#define BLACK	0
#endif
#ifndef WHITE
#define WHITE	1
#endif
#ifndef INVERT
#define INVERT	2
#endif

/* ------------------------------------------------------------------------- */
/* Framebuffer (draw buffer) */
/* ------------------------------------------------------------------------- */

/* C-side draw buffer in native Arduboy vertical page layout. Display() converts it into the internal horizontal front buffer. */
extern u8 sBuffer[VM23_BUFFER_SIZE];

/* ------------------------------------------------------------------------- */
/* Core display control */
/* ------------------------------------------------------------------------- */

void Display(void);
void ArduboyDisplay(void);

void ClearVram(void);
void ArduboyClear(void);

/* Clears only the internal displayed buffer (front buffer). */
void ClearDisplayBuffer(void);

/* Palette for scanout: fg used when bit=1, bg used when bit=0. */
void SetPalette(u8 fg, u8 bg);

/* Optional invert flag (scanout-time; current scanout path may ignore). */
void SetInvert(u8 enabled);

/* ------------------------------------------------------------------------- */
/* Cursor + text */
/* ------------------------------------------------------------------------- */

void ArduboySetCursor(s16 x, s16 y);

/* Return cursor x/y as s16 in r25:r24 from ASM; C prototypes: */
s16 ArduboyGetCursorX(void);
s16 ArduboyGetCursorY(void);

/* Arduboy-style text color/background controls (for ports using setTextColor). */
void ArduboySetTextColor(u8 color);			/* BLACK/WHITE/INVERT */
void ArduboySetTextBackground(u8 color);		/* BLACK/WHITE/INVERT */

/* Print helpers */
void ArduboyPrint(const char* text);
void ArduboyPrintU8(u8 value);
void ArduboyPrintU16(u16 value);

/* Single-character output (like arduboy.write('L')). Returns 1. */
u8 ArduboyWrite(u8 ch);

/* ------------------------------------------------------------------------- */
/* Drawing primitives */
/* ------------------------------------------------------------------------- */

/* Pixel */
void DrawPixel(s16 x, s16 y, u8 color);
void ArduboyDrawPixel(s16 x, s16 y, u8 color);

/* Fast lines are page-aware on in-bounds cases and fall back for clipped cases. */
void DrawFastHLine(s16 x, s16 y, u8 w, u8 color);
void ArduboyDrawFastHLine(s16 x, s16 y, u8 w, u8 color);

void DrawFastVLine(s16 x, s16 y, u8 h, u8 color);
void ArduboyDrawFastVLine(s16 x, s16 y, u8 h, u8 color);

/*
 * Bitmaps:
 * IMPORTANT: In this mode, ArduboyDrawBitmap expects a *native Arduboy vertical*
 * 1bpp bitmap in page layout: width bytes per 8-pixel page, stored in PROGMEM.
 * Total bytes = w * ((h+7)>>3). Zero bits are transparent.
 */
void DrawBitmap(s16 x, s16 y, const u8* bmp, u8 w, u8 h, u8 color);
void ArduboyDrawBitmap(s16 x, s16 y, const u8* bmp, u8 w, u8 h, u8 color);

/* Opaque overwrite bitmap (native Arduboy vertical page layout), stored in PROGMEM.
 * Page-aligned in-bounds draws use a direct byte path. */
void DrawOverwriteRaw(s16 x, s16 y, const u8* bmp, u8 w, u8 h);

/*
 * Sprites:
 * Format matches Arduboy Sprites class header:
 *  [w][h][frame0...]
 * and frame data must be native Arduboy vertical page layout for this mode.
 */
void SpritesDrawOverwrite(s16 x, s16 y, const u8* spr, u8 frame);

/*
 * Plus-mask sprites:
 *  [w][h][frame0 rows: img0,mask0,img1,mask1,...]
 * native Arduboy vertical page layout, PROGMEM.
 * Page-aligned in-bounds draws use a direct byte path.
 */
void SpritesDrawPlusMask(s16 x, s16 y, const u8* spr, u8 frame);

/* Shapes */
void ArduboyFillRect(s16 x, s16 y, u8 w, u8 h, u8 color);
void ArduboyDrawRect(s16 x, s16 y, u8 w, u8 h, u8 color);

void ArduboyDrawLine(s16 x0, s16 y0, s16 x1, s16 y1, u8 color);

void ArduboyDrawCircle(s16 x, s16 y, u8 r, u8 color);
void ArduboyFillCircle(s16 x, s16 y, u8 r, u8 color);

#endif /* VIDEOMODE23_H */