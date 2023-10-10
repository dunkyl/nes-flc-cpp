#include <string.h>
#include <inttypes.h>

#include <array>

#include <neslib.h>
#include <nesdoug.h>

typedef struct {
    size_t len;
    char* string;
} string_with_len;

#define STRING_WITH_LEN(literal) {.len = (sizeof(literal) - 1), .string = literal}


struct TextAt
{
    int x;
    int y;
    char* text;
};

struct BgTilesAt
{
    int x;
    int y;
    int w;
    int h;
    int tile_id;
};

// draw a single line of text into vram
void v_text(const TextAt& it) {
    vram_adr(NTADR_A(it.x, it.y));
    size_t len = strlen(it.text);
    vram_write(it.text, len);
}

// draw a rectangle of tiles into vram
void v_tiles(const BgTilesAt& it) {
    const auto STRIDE = 0x10z;
    int row_offset = it.tile_id;
    for (int row = 0; row < it.h; ++row) {
        // move to front of row
        vram_adr(NTADR_A(it.x, it.y+row));
        // put each tile
        for (int i = 0; i < it.w; ++i) {
            vram_put(row_offset+i);
        }
        row_offset += STRIDE;
    }
}

void wait_frames(int count) {
    for (int i = 0; i < count; ++i) {
        ppu_wait_nmi();
    }
}

const int FRAMES_PER_SECOND = 60;

void defer_v_text(int addr, string_with_len text) {
    static std::array<char, 255> vram_update_buf;
    // const auto at = NTADR_A(0, 0);
    const auto m = MSB(addr);
    const auto l = LSB(addr);
    vram_update_buf[0] = m | NT_UPD_HORZ;
    vram_update_buf[1] = l;
    vram_update_buf[2] = text.len;
    memcpy(vram_update_buf.begin()+3, text.string, text.len);
    vram_update_buf[2+text.len+1] = NT_UPD_EOF;
    set_vram_update(vram_update_buf.begin());
    ppu_wait_nmi();
}

void type_v_text(int addr, string_with_len text) {
    static std::array<char, 4> vram_update_buf;
    // const auto at = NTADR_A(0, 0);
    const uint8_t m = MSB(addr);
    const uint8_t l = LSB(addr);
    vram_update_buf[0] = m;
    vram_update_buf[3] = NT_UPD_EOF;
    for (uint8_t i = 0; i < text.len; ++i) {
        vram_update_buf[1] = l+i;
        vram_update_buf[2] = text.string[i];
        set_vram_update(vram_update_buf.begin());
        ppu_wait_nmi();
    }
    vram_update_buf[0] = NT_UPD_EOF;
}

void wait_for_a_press(){
    while ( !(pad_poll(0) & PAD_A)) {
        ppu_wait_nmi();
    }
}

void small_a_prompt() {
    v_tiles({.x = 29, .y=26, .w=2, .h=2, .tile_id=0x08});
    v_text({.x = 23, .y=28, .text="Continue"});

    vram_adr(NAMETABLE_A + 0x3c0 + 55);
    vram_put(1 << 6 | 1 << 4);
}

void slide1() {
    // pal_fade_to(4, 0);
    // ppu_off();
    
    v_text({.x = 7, .y=19, .text="Press A to start."});
    v_tiles({.x = 15, .y=14, .w=2, .h=2, .tile_id=0x08});

    // (bottomright << 6) | (bottomleft << 4) | (topright << 2) | (topleft << 0)
    vram_adr(NAMETABLE_A + 0x3c0 + 27);
    vram_put(1 << 6);
    vram_put(1 << 4);

    ppu_on_all();
    pal_fade_to(0, 4);
    // type_v_text(NTADR_A(1, 1), STRING_WITH_LEN("Press A to start."));
    wait_for_a_press();
}

void slide2() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    vram_fill(0, 0x2000);

    v_tiles({.x = 13, .y=3, .w=6, .h=6, .tile_id=0x8a});
    vram_adr(NAMETABLE_A + 0x3c0 + 3);
    vram_fill(0xaa, 2);
    vram_adr(NAMETABLE_A + 0x3c0 + 11);
    vram_fill(0xaa, 2);
    vram_adr(NAMETABLE_A + 0x3c0 + 19);
    vram_fill(2 << 2 | 2 << 0, 2);

    small_a_prompt();

    ppu_on_all();
    pal_fade_to(0, 4);

    type_v_text(NTADR_A(8, 12), STRING_WITH_LEN("FLC++ Project #1"));
    type_v_text(NTADR_A(9, 14), STRING_WITH_LEN("by Maroue Reus"));
    type_v_text(NTADR_A(6, 18), STRING_WITH_LEN("NES Basic Slideshow"));

    wait_for_a_press();
}

void slide3() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    vram_fill(0, 0x2000);

    v_tiles({.x = 12, .y=3, .w=8, .h=5, .tile_id=0xb0});
    
    small_a_prompt();

    ppu_on_all();
    pal_fade_to(0, 4);

    type_v_text(NTADR_A(2, 10), STRING_WITH_LEN("MOS Systems in 1975"));
    type_v_text(NTADR_A(3, 11), STRING_WITH_LEN("\x01\x02 Metal Oxide Semiconductor"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 14), STRING_WITH_LEN("Known for (variations):"));
    type_v_text(NTADR_A(2, 15), STRING_WITH_LEN("- NES (you are here!)"));
    type_v_text(NTADR_A(2, 16), STRING_WITH_LEN("- Commodore 64"));
    type_v_text(NTADR_A(2, 17), STRING_WITH_LEN("- Atari 2600"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 20), STRING_WITH_LEN("Contemporary with Intel 8080"));
    type_v_text(NTADR_A(3, 21), STRING_WITH_LEN("and Motorola 6800"));

    type_v_text(NTADR_A(2, 24), STRING_WITH_LEN("~ last thing before MOS"));
    type_v_text(NTADR_A(2, 25), STRING_WITH_LEN(" slowly faded away"));

    wait_for_a_press();
}

void slide4() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    vram_fill(0, 0x2000);

    v_tiles({.x = 12, .y=3, .w=8, .h=5, .tile_id=0xb0});
    
    small_a_prompt();

    ppu_on_all();
    pal_fade_to(0, 4);

    type_v_text(NTADR_A(2, 10), STRING_WITH_LEN("- 8-bit registers"));
    type_v_text(NTADR_A(2, 12), STRING_WITH_LEN("- No OS"));
    type_v_text(NTADR_A(2, 13), STRING_WITH_LEN("  -> No malloc"));
    type_v_text(NTADR_A(2, 14), STRING_WITH_LEN("  -> No operator new"));
    type_v_text(NTADR_A(2, 15), STRING_WITH_LEN("  -> No printf"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 17), STRING_WITH_LEN("- just 1 MHz"));
    type_v_text(NTADR_A(2, 18), STRING_WITH_LEN("  -> only k's of ops per frame"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 20), STRING_WITH_LEN("- special PPU for drawing"));
    type_v_text(NTADR_A(2, 22), STRING_WITH_LEN("- limited color and content"));

    wait_for_a_press();
}

void slide5() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    bank_bg(1);
    vram_fill(0, 0x2000);

    v_text({.x = 2, .y=2, .text="Programming"}); 
    v_tiles({.x = 16, .y=2, .w=16, .h=8, .tile_id=0x80});
    vram_adr(NAMETABLE_A + 0x3c0 + 0);
    vram_fill(0xaa, 24);

    small_a_prompt();

    ppu_on_all();
    pal_fade_to(0, 4);

    type_v_text(NTADR_A(2, 12), STRING_WITH_LEN("Traditionally with assembly"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 14), STRING_WITH_LEN("C was still young, pre-std"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 16), STRING_WITH_LEN("1983: NES Release JP"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 18), STRING_WITH_LEN("1980ish: 8080 \"Small C\""));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 20), STRING_WITH_LEN("1998: CC65"));

    wait_for_a_press();

    type_v_text(NTADR_A(2, 22), STRING_WITH_LEN("2017: LLVM-MOS"));

    type_v_text(NTADR_A(2, 24), STRING_WITH_LEN("Full modern optimizing"));

    type_v_text(NTADR_A(2, 25), STRING_WITH_LEN("Any LLVM language"));

    wait_for_a_press();
}

void slide6() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    bank_bg(1);
    vram_fill(0, 0x2000);

    v_text({.x = 2, .y=2, .text="This slideshow"}); 
    v_tiles({.x = 16, .y=2, .w=16, .h=8, .tile_id=0x80});
    vram_adr(NAMETABLE_A + 0x3c0 + 0);
    vram_fill(0xaa, 24);

    small_a_prompt();

    ppu_on_all();
    pal_fade_to(0, 4);

    type_v_text(NTADR_A(2, 12), STRING_WITH_LEN("C++ 23:"));
    type_v_text(NTADR_A(2, 14), STRING_WITH_LEN(" - still in draft stage"));
    type_v_text(NTADR_A(2, 15), STRING_WITH_LEN(" - implemented"));

    wait_for_a_press();
}

void slide7() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    bank_bg(0);
    vram_fill(0, 0x2000);
    v_text({.x = 1, .y=1, .text="Graphics made with"}); 
    v_text({.x = 9, .y=2, .text="YY-CHR.NET"}); 
    v_tiles({.x = 21, .y=1, .w=4, .h=3, .tile_id=0x86});

    v_tiles({.x = 8, .y=6, .w=16, .h=16, .tile_id=0x00});


    vram_adr(NAMETABLE_A + 0x3c0 + 0);
    vram_fill(0xaa, 24);

    small_a_prompt();

    ppu_on_all();
    pal_fade_to(0, 4);

    wait_for_a_press();
}


void slide8() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    vram_fill(0, 0x2000);
    v_text({.x = 1, .y=1, .text="Code made with VS Code"}); 


    // draw box
    vram_adr(NTADR_A(1, 3));
    vram_fill(0x83, 29);
    vram_adr(NTADR_A(1, 27));
    vram_fill(0x93, 29);
    for (int i = 0; i < 23; ++i) {
        vram_adr(NTADR_A(1, 4+i));
        vram_put(0x84);
        vram_adr(NTADR_A(30, 4+i));
        vram_put(0x94);
    }
    vram_adr(NTADR_A(1, 3)); vram_put(0x82);
    vram_adr(NTADR_A(30, 3)); vram_put(0x85);
    vram_adr(NTADR_A(1, 27)); vram_put(0x92);
    // vram_adr(NTADR_A(30, 25)); vram_put(0x95);

    // code colors
    // (bottomright << 6) | (bottomleft << 4) | (topright << 2) | (topleft << 0)
    vram_adr(NAMETABLE_A + 0x3c0 + 8);
    vram_put(2 << 2);
    for (int row = 0; row < 5; ++row) {
        const auto offset = row*8;
        vram_adr(NAMETABLE_A + 0x3c0 + 9 + offset);
        vram_put(3 << 2 | 3 << 6);
        vram_put(0xff);
        // vram_put(3 << 0 | 3 << 4);
    }
    vram_adr(NAMETABLE_A + 0x3c0 + 10);
    vram_put(3 << 0 | 3 << 4 | 3 << 6);
    vram_adr(NAMETABLE_A + 0x3c0 + 9);
    vram_put(3 << 2 | 3 << 6 | 2 << 0 | 2 << 4);


    small_a_prompt();

    ppu_on_all();
    pal_fade_to(0, 4);

    // code text
    type_v_text(NTADR_A(2, 4),  STRING_WITH_LEN("//..."));
    type_v_text(NTADR_A(2, 5),  STRING_WITH_LEN("int main() {"));
    type_v_text(NTADR_A(2, 6),  STRING_WITH_LEN("    //..."));
    type_v_text(NTADR_A(2, 7),  STRING_WITH_LEN("    slide1();"));
    type_v_text(NTADR_A(2, 9),  STRING_WITH_LEN("    slide2();"));
    type_v_text(NTADR_A(2, 11), STRING_WITH_LEN("    slide3();"));
    type_v_text(NTADR_A(2, 13), STRING_WITH_LEN("    slide4();"));
    type_v_text(NTADR_A(2, 15), STRING_WITH_LEN("    slide5();"));
    type_v_text(NTADR_A(2, 17), STRING_WITH_LEN("    slide6();"));
    type_v_text(NTADR_A(2, 19), STRING_WITH_LEN("    slide7();"));
    type_v_text(NTADR_A(2, 21), STRING_WITH_LEN("    slide8();"));
    type_v_text(NTADR_A(2, 23), STRING_WITH_LEN("    slide9();"));
    type_v_text(NTADR_A(2, 25), STRING_WITH_LEN("}"));

    wait_for_a_press();
}

void slide9() {
    wait_frames(30);
    pal_fade_to(4, 0);
    ppu_off();
    vram_fill(0, 0x2000);

    ppu_on_all();
    pal_fade_to(0, 4);

    type_v_text(NTADR_A(14, 10), STRING_WITH_LEN("END"));

    type_v_text(NTADR_A(11, 14), STRING_WITH_LEN("Thank you"));

    wait_for_a_press();
}

auto main() -> int {

    static std::array<char, 255> vram_update_buf;

    static char palette[16] = { // universal: deep blue
              0x01, 0x05, 0x15, 0x35, // pink
        0x01,       0x0b, 0x2a, 0x3b, // green
        0x01,       0x00, 0x31, 0x30, // blue and grey
        0x01,       0x0f, 0x0f, 0x27, // ... gold
        };
    pal_bg(palette);

    slide1();

    slide2();

    slide3();

    slide4();

    slide5();

    slide6();

    slide7();

    slide8();

    slide9();
}
