#include "neslib.h"
#include "nesdoug.h" 

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
// there's some oddities in the palette code, black must be 0x0f, white must be 0x30
 
typedef unsigned char uint8;
 
#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster

unsigned char i;
uint8 x = 4;
uint8 y = 4;
int p0;
uint8 j = 0;

const unsigned char text[]="Hello NES!"; // zero terminated c string

const unsigned char palette[]={
    0x0C, 0x14, 0x23, 0x37,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
}; 

void clearPutText(uint8 x, uint8 y) {
     // screen off
	// pal_bg(palette); //	load the BG palette
    vram_adr(NTADR_A(x-1,y-1));
    for (i=0; i<13; i++) {
        vram_put(' ');
    }
    vram_adr(NTADR_A(x-1,y));
    vram_put(' ');
    vram_write(text,sizeof(text));
    vram_put(' ');
    vram_adr(NTADR_A(x-1,y+1));
    for (i=0; i<13; i++) {
        vram_put(' ');
    }
}

void clearPutTextBuf(uint8 x, uint8 y) {
     // screen off
	// pal_bg(palette); //	load the BG palette
    // vram_adr(NTADR_A(x-1,y-1));
    for (i=0; i<13; i++) {
        one_vram_buffer(' ', NTADR_A(x-1+i,y-1));
    }
    // vram_adr(NTADR_A(x-1,y));
    one_vram_buffer(' ', NTADR_A(x-1,y));
    // vram_put(' ');
    // vram_write(text,sizeof(text));
    multi_vram_buffer_horz(text, sizeof(text), NTADR_A(x,y));
    one_vram_buffer(' ', NTADR_A(x+sizeof(text),y));
    // vram_put(' ');
    // vram_adr(NTADR_A(x-1,y+1));
    // for (i=0; i<13; i++) {
    //     vram_put(' ');
    // }
    for (i=0; i<13; i++) {
        one_vram_buffer(' ', NTADR_A(x-1+i,y+1));
    }
}


int main (void) {

    ppu_off();
    pal_bg(palette);

    clearPutText(x,y);

    ppu_on_all();

    set_vram_buffer();

	while (1){
		// infinite loop
		// game code can go here later.
        // if (j < 200) {
        //     ++j;
        //     continue;
        // }
        // j = 0;
        ppu_wait_nmi();

        ppu_wait_nmi();
        

        p0 = pad_poll(0);

        if (p0 & PAD_LEFT) {
            --x;
        }
        else if (p0 & PAD_RIGHT) {
            ++x;
        }
        
        if (p0 & PAD_DOWN) {
            ++y;
        } else if (p0 & PAD_UP) {
            --y;
        } 

        clearPutTextBuf(x,y);
        // ppu_off();
        // clearPutText(x,y);
        // ppu_on_all();
	}
}