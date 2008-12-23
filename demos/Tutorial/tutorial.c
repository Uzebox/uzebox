#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "kernel/uzebox.h"

#include "data/fonts.pic.inc"

const char strHello[] PROGMEM ="HELLO WORLD FROM THE UZEBOX!";

int main(){

   SetFontTable(fonts);
   ClearVram();
   Print(8,12,strHello);

   while(1);

} 
