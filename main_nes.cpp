#include <cstdint>

#include <nes.h>

const auto X = 2z;

const uint8_t INT_SZ = sizeof(int);
const uint8_t PTR_SZ = sizeof(char*);
const uint8_t SZ_SZ = sizeof(decltype(X));

static auto ppu_wait_vblank(void) {
    while (!(PPU.status & 0x80))
    ;
}

static auto ppu_write_addr(unsigned short addr) {
    (void)PPU.status;
    PPU.vram.address = addr >> 8;
    PPU.vram.address = addr & 0xff;
}

auto main() -> int {
    // Enable BG rendering.
    ppu_wait_vblank();
    PPU.mask = 0b00001000;

    char color = 0;
    for (;;) {
        // Wait for 0.5 second.
        for (int i = 0; i < 30; ++i)
          ppu_wait_vblank();
  
        // Increment the palette color 0.
        ppu_write_addr(0x3f00u);
        PPU.vram.data = ++color;
    }
}