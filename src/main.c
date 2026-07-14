#include <gb/gb.h>
#include <rand.h>
#include <stdint.h>
#include <string.h>

#define SCREEN_TILES_W 20u
#define SCREEN_TILES_H 18u
#define PLAYER_Y 128u
#define PLAYER_MIN_X 8u
#define PLAYER_MAX_X 136u
#define OBJECT_COUNT 6u

#define SPRITE_SHIP_LEFT 0u
#define SPRITE_SHIP_RIGHT 1u
#define SPRITE_STAR 2u
#define SPRITE_METEOR 3u

#define TILE_BLANK 16u
#define TILE_BORDER 17u
#define TILE_DUST 18u
#define TILE_DIGIT_0 19u
#define TILE_A 29u
#define TILE_C 30u
#define TILE_E 31u
#define TILE_G 32u
#define TILE_H 33u
#define TILE_M 34u
#define TILE_O 35u
#define TILE_P 36u
#define TILE_R 37u
#define TILE_S 38u
#define TILE_T 39u
#define TILE_V 40u
#define TILE_HEART 41u
#define TILE_D 42u
#define TILE_I 43u
#define TILE_U 44u

#define OBJ_STAR 0u
#define OBJ_METEOR 1u

#define ROW(bits) bits, bits

typedef struct ObjectStore {
    uint8_t x[OBJECT_COUNT];
    uint8_t y[OBJECT_COUNT];
    uint8_t speed[OBJECT_COUNT];
    uint8_t delay[OBJECT_COUNT];
    uint8_t type[OBJECT_COUNT];
} ObjectStore;

/* Four 8x8, two-bit Game Boy sprite tiles: a two-tile ship, star, meteor. */
const uint8_t sprite_tiles[] = {
    0x00,0x00, 0x01,0x01, 0x03,0x02, 0x07,0x05,
    0x1F,0x12, 0x3F,0x24, 0x7F,0x5B, 0xFF,0xA4,
    0x00,0x00, 0x80,0x80, 0xC0,0x40, 0xE0,0xA0,
    0xF8,0x48, 0xFC,0x24, 0xFE,0xDA, 0xFF,0x25,
    0x18,0x18, 0x99,0x81, 0x5A,0x42, 0x3C,0x24,
    0xFF,0xDB, 0x3C,0x24, 0x5A,0x42, 0x99,0x81,
    0x3C,0x3C, 0x7E,0x42, 0xDB,0x99, 0xFF,0xA5,
    0xFF,0x99, 0xDB,0xC3, 0x7E,0x42, 0x3C,0x3C
};

/* Background/UI tiles. Each row is duplicated into both bit planes. */
const uint8_t background_tiles[] = {
    ROW(0x00),ROW(0x00),ROW(0x00),ROW(0x00),ROW(0x00),ROW(0x00),ROW(0x00),ROW(0x00),
    ROW(0xFF),ROW(0x81),ROW(0x81),ROW(0x81),ROW(0x81),ROW(0x81),ROW(0x81),ROW(0xFF),
    ROW(0x00),ROW(0x08),ROW(0x00),ROW(0x00),ROW(0x80),ROW(0x00),ROW(0x02),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x6E),ROW(0x76),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x18),ROW(0x38),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x7E),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x06),ROW(0x0C),ROW(0x30),ROW(0x60),ROW(0x7E),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x06),ROW(0x1C),ROW(0x06),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x0C),ROW(0x1C),ROW(0x3C),ROW(0x6C),ROW(0x7E),ROW(0x0C),ROW(0x0C),ROW(0x00),
    ROW(0x7E),ROW(0x60),ROW(0x7C),ROW(0x06),ROW(0x06),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x1C),ROW(0x30),ROW(0x60),ROW(0x7C),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x7E),ROW(0x06),ROW(0x0C),ROW(0x18),ROW(0x30),ROW(0x30),ROW(0x30),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x66),ROW(0x3E),ROW(0x06),ROW(0x0C),ROW(0x38),ROW(0x00),
    ROW(0x18),ROW(0x3C),ROW(0x66),ROW(0x66),ROW(0x7E),ROW(0x66),ROW(0x66),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x60),ROW(0x60),ROW(0x60),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x7E),ROW(0x60),ROW(0x60),ROW(0x7C),ROW(0x60),ROW(0x60),ROW(0x7E),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x60),ROW(0x6E),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x7E),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x00),
    ROW(0x63),ROW(0x77),ROW(0x7F),ROW(0x6B),ROW(0x63),ROW(0x63),ROW(0x63),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x7C),ROW(0x66),ROW(0x66),ROW(0x7C),ROW(0x60),ROW(0x60),ROW(0x60),ROW(0x00),
    ROW(0x7C),ROW(0x66),ROW(0x66),ROW(0x7C),ROW(0x6C),ROW(0x66),ROW(0x66),ROW(0x00),
    ROW(0x3C),ROW(0x66),ROW(0x60),ROW(0x3C),ROW(0x06),ROW(0x66),ROW(0x3C),ROW(0x00),
    ROW(0x7E),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x00),
    ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x18),ROW(0x00),
    ROW(0x00),ROW(0x66),ROW(0xFF),ROW(0xFF),ROW(0x7E),ROW(0x3C),ROW(0x18),ROW(0x00),
    ROW(0x78),ROW(0x6C),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x6C),ROW(0x78),ROW(0x00),
    ROW(0x3C),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x18),ROW(0x3C),ROW(0x00),
    ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x66),ROW(0x3C),ROW(0x00)
};

static ObjectStore objects;
static uint8_t background_map[SCREEN_TILES_W * SCREEN_TILES_H];
static uint8_t player_x;
static uint16_t score;
static uint16_t high_score;
static uint8_t lives;
static uint8_t frame;
static uint8_t previous_keys;

static uint8_t letter_tile(char c) {
    if ((c >= '0') && (c <= '9')) return TILE_DIGIT_0 + (uint8_t)(c - '0');
    switch (c) {
        case 'A': return TILE_A;
        case 'C': return TILE_C;
        case 'D': return TILE_D;
        case 'E': return TILE_E;
        case 'G': return TILE_G;
        case 'H': return TILE_H;
        case 'I': return TILE_I;
        case 'M': return TILE_M;
        case 'O': return TILE_O;
        case 'P': return TILE_P;
        case 'R': return TILE_R;
        case 'S': return TILE_S;
        case 'T': return TILE_T;
        case 'U': return TILE_U;
        case 'V': return TILE_V;
        default: return TILE_BLANK;
    }
}

static void draw_text(uint8_t x, uint8_t y, const char *text) {
    uint8_t tile;
    while (*text) {
        tile = letter_tile(*text++);
        set_bkg_tile_xy(x++, y, tile);
    }
}

static void sound_init(void) {
    NR52_REG = 0x80u;
    NR50_REG = 0x77u;
    NR51_REG = 0xFFu;
}

static void sound_pickup(void) {
    NR10_REG = 0x16u;
    NR11_REG = 0x80u;
    NR12_REG = 0xF2u;
    NR13_REG = 0xA0u;
    NR14_REG = 0x87u;
}

static void sound_crash(void) {
    NR41_REG = 0x10u;
    NR42_REG = 0xF3u;
    NR43_REG = 0x35u;
    NR44_REG = 0xC0u;
}

static void sound_music_tick(void) {
    static const uint16_t notes[4] = { 0x62Cu, 0x6A4u, 0x70Bu, 0x762u };
    uint16_t note = notes[(frame >> 4u) & 3u];
    NR21_REG = 0x40u;
    NR22_REG = 0x52u;
    NR23_REG = (uint8_t)note;
    NR24_REG = 0x80u | (uint8_t)(note >> 8u);
}

static void sound_game_over(void) {
    uint8_t i;
    static const uint16_t notes[4] = { 0x6A4u, 0x62Cu, 0x562u, 0x4A0u };
    for (i = 0u; i != 4u; ++i) {
        NR21_REG = 0x80u;
        NR22_REG = 0xA3u;
        NR23_REG = (uint8_t)notes[i];
        NR24_REG = 0x80u | (uint8_t)(notes[i] >> 8u);
        delay(120u);
    }
}

static void clear_background(void) {
    memset(background_map, TILE_BLANK, sizeof(background_map));
    set_bkg_tiles(0u, 0u, SCREEN_TILES_W, SCREEN_TILES_H, background_map);
}

static void draw_title(void) {
    uint8_t keys;
    HIDE_SPRITES;
    clear_background();
    draw_text(4u, 3u, "STAR CATCHER");
    draw_text(3u, 8u, "PRESS START");
    draw_text(3u, 12u, "CATCH STARS");
    draw_text(2u, 14u, "AVOID METEORS");
    SHOW_BKG;
    waitpadup();
    do {
        keys = joypad();
        initarand((uint16_t)DIV_REG | ((uint16_t)LY_REG << 8u));
        vsync();
    } while (!(keys & (J_START | J_A)));
    waitpadup();
}

static void update_hud(void) {
    uint8_t digits[3];
    uint16_t shown = score;
    uint8_t i;
    if (shown > 999u) shown = 999u;
    digits[2] = TILE_DIGIT_0 + (uint8_t)(shown % 10u);
    shown /= 10u;
    digits[1] = TILE_DIGIT_0 + (uint8_t)(shown % 10u);
    shown /= 10u;
    digits[0] = TILE_DIGIT_0 + (uint8_t)shown;
    set_bkg_tiles(6u, 0u, 3u, 1u, digits);
    for (i = 0u; i != 3u; ++i) {
        set_bkg_tile_xy(16u + i, 0u, (i < lives) ? TILE_HEART : TILE_BLANK);
    }
}

static void draw_playfield(void) {
    uint8_t x, y;
    for (y = 0u; y != SCREEN_TILES_H; ++y) {
        for (x = 0u; x != SCREEN_TILES_W; ++x) {
            if ((x == 0u) || (x == 19u) || (y == 1u) || (y == 17u))
                background_map[(uint16_t)y * SCREEN_TILES_W + x] = TILE_BORDER;
            else
                background_map[(uint16_t)y * SCREEN_TILES_W + x] =
                    (((x * 3u + y * 5u) & 7u) == 0u) ? TILE_DUST : TILE_BLANK;
        }
    }
    for (x = 0u; x != SCREEN_TILES_W; ++x)
        background_map[x] = TILE_BLANK;
    set_bkg_tiles(0u, 0u, SCREEN_TILES_W, SCREEN_TILES_H, background_map);
    draw_text(1u, 0u, "SCORE");
    update_hud();
}

static void respawn_object(uint8_t i, uint8_t initial) {
    objects.x[i] = 10u + (uint8_t)(rand() % 137u);
    objects.y[i] = 8u;
    objects.delay[i] = initial ? (uint8_t)(i * 24u + 8u) : (uint8_t)(rand() & 31u);
    objects.speed[i] = 1u + (uint8_t)((score / 12u) > 2u ? 2u : (score / 12u));
    objects.type[i] = ((rand() & 7u) < (score > 15u ? 3u : 2u)) ? OBJ_METEOR : OBJ_STAR;
    set_sprite_tile(2u + i, objects.type[i] == OBJ_STAR ? SPRITE_STAR : SPRITE_METEOR);
    move_sprite(2u + i, objects.x[i] + 8u, 0u);
}

static uint8_t overlaps_player(uint8_t i) {
    return (objects.y[i] + 7u >= PLAYER_Y) && (objects.y[i] <= PLAYER_Y + 7u) &&
           (objects.x[i] + 7u >= player_x) && (objects.x[i] <= player_x + 15u);
}

static void start_game(void) {
    uint8_t i;
    player_x = 72u;
    score = 0u;
    lives = 3u;
    frame = 0u;
    previous_keys = 0u;
    draw_playfield();
    set_sprite_tile(0u, SPRITE_SHIP_LEFT);
    set_sprite_tile(1u, SPRITE_SHIP_RIGHT);
    move_sprite(0u, player_x + 8u, PLAYER_Y + 16u);
    move_sprite(1u, player_x + 16u, PLAYER_Y + 16u);
    for (i = 0u; i != OBJECT_COUNT; ++i) respawn_object(i, 1u);
    SHOW_SPRITES;
}

static void toggle_pause(void) {
    draw_text(7u, 8u, "PAUSE");
    waitpadup();
    waitpad(J_START);
    waitpadup();
    draw_playfield();
}

static void game_loop(void) {
    uint8_t i;
    uint8_t keys;
    start_game();
    while (lives) {
        keys = joypad();
        if ((keys & J_LEFT) && (player_x > PLAYER_MIN_X)) player_x -= 2u;
        if ((keys & J_RIGHT) && (player_x < PLAYER_MAX_X)) player_x += 2u;
        if ((keys & J_START) && !(previous_keys & J_START)) toggle_pause();
        previous_keys = keys;
        move_sprite(0u, player_x + 8u, PLAYER_Y + 16u);
        move_sprite(1u, player_x + 16u, PLAYER_Y + 16u);

        for (i = 0u; i != OBJECT_COUNT; ++i) {
            if (objects.delay[i]) {
                --objects.delay[i];
                continue;
            }
            objects.y[i] += objects.speed[i];
            move_sprite(2u + i, objects.x[i] + 8u, objects.y[i] + 16u);
            if (overlaps_player(i)) {
                if (objects.type[i] == OBJ_STAR) {
                    ++score;
                    sound_pickup();
                } else {
                    --lives;
                    sound_crash();
                }
                update_hud();
                respawn_object(i, 0u);
            } else if (objects.y[i] > 144u) {
                respawn_object(i, 0u);
            }
        }
        if ((frame & 15u) == 0u) sound_music_tick();
        ++frame;
        vsync();
    }
}

static void game_over(void) {
    HIDE_SPRITES;
    if (score > high_score) high_score = score;
    clear_background();
    draw_text(6u, 5u, "GAME OVER");
    draw_text(6u, 8u, "SCORE");
    {
        uint16_t value = score > 999u ? 999u : score;
        uint8_t digits[3];
        digits[2] = TILE_DIGIT_0 + (uint8_t)(value % 10u); value /= 10u;
        digits[1] = TILE_DIGIT_0 + (uint8_t)(value % 10u); value /= 10u;
        digits[0] = TILE_DIGIT_0 + (uint8_t)value;
        set_bkg_tiles(12u, 8u, 3u, 1u, digits);
    }
    draw_text(4u, 12u, "PRESS START");
    sound_game_over();
    waitpadup();
    waitpad(J_START | J_A);
    waitpadup();
}

void main(void) {
    DISPLAY_OFF;
    set_sprite_data(0u, 4u, sprite_tiles);
    set_bkg_data(TILE_BLANK, 29u, background_tiles);
    SPRITES_8x8;
    OBP0_REG = 0xE4u;
    BGP_REG = 0xE4u;
    sound_init();
    DISPLAY_ON;

    draw_title();
    while (1) {
        game_loop();
        game_over();
    }
}
