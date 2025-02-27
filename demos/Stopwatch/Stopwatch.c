// Uzebox stopwatch

// By Dan MacDonald
//       2023

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uzebox.h>

#include "data/tileset.inc"

const char *numbers[10] = {n0, n1, n2, n3, n4, n5, n6, n7, n8, n9};

int frames, seconds, minutes, hours, hundreds, btnprev, btnheld = 0;

bool active = false;

const char boing[] PROGMEM ={
    0,PC_WAVE,8,
    0,PC_ENV_VOL,0xE0,
    0,PC_ENV_SPEED,-20,
    2,PC_NOTE_DOWN,6,
    2,PC_NOTE_DOWN,6,
    2,PC_NOTE_CUT,0,
    0,PATCH_END
};

const struct PatchStruct patches[] PROGMEM = {
    {0, NULL, boing, 0, 0},
};

// DrawDigits is a function to easily draw a double digit number between 00 and 99 using the large numbers.
// xoffset represents the first column to use for the first (10s) number of the pair.

void DrawDigits(int number, int xoffset);

void DrawDigits(int number, int xoffset) {
    // First we draw the units (rightmost) digit. It gets drawn at xoffset + 5 tiles
    DrawMap2((xoffset + 5), 8, (numbers[number % 10]));
    // Draw the tens (leftmost) digit or a zero
    DrawMap2(xoffset, 8, (numbers[number / 10 % 10]));
}

void DrawColon(int xoffset);

void DrawColon(int xoffset) {
    SetTile(xoffset,9,14);
    SetTile(xoffset,11,14);
}

int main()
{
    SetSpritesTileTable(tileset);
    InitMusicPlayer(patches);
    SetTileTable(tileset);
    ClearVram();
    while (1)
    {
        WaitVsync(1); // This is key to keeping accurate time.

        Print(9,2,PSTR("UZEBOX STOPWATCH"));
        Print(9,4,PSTR("BY DAN MACDONALD"));

        Print(5,18,PSTR("PUSH START TO START/STOP"));
        Print(7,20,PSTR("PUSH SELECT TO RESET"));

        btnheld = ReadJoypad(0);

        if (btnheld != btnprev) {
            if (btnheld & BTN_START) {
                active = !active;
                TriggerNote(0, 0, 92, 127);
            }
            if (btnheld & BTN_SELECT) {
                if (active == false) {
                    frames = 0;
                    seconds = 0;
                    minutes = 0;
                    hours = 0;
                    hundreds = 0;
                    TriggerNote(0, 0, 80, 127);
                }
            }
            btnprev = btnheld;
        }


        if (active) {
            frames++;
            if (frames >= 60) {
                seconds++;
                frames=0;
            }
            if (seconds >= 60) {
                minutes++;
                seconds=0;
            }
            if (minutes >= 60) {
                hours++;
                minutes=0;
            }
            if (hours >= 100) {
                hundreds++;
                frames = 0;
                seconds = 0;
                minutes = 0;
                hours = 0;
            }
        }

        // Print hundreds of hours
        PrintInt(16,23,hundreds,false);

        // Print hours
        DrawDigits(hours,2);

        DrawColon(11);

        // Print minutes
        DrawDigits(minutes,12);

        DrawColon(21);

        // Print seconds
        DrawDigits(seconds,22);

        // Print jiffys/ticks/thirds/frames (1/60th seconds)
        PrintInt(16,15,frames,false);
    }
}
