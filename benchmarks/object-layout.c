#include <gb/gb.h>
#include <stdint.h>
#include <stdio.h>

#define OBJECT_COUNT 6u
#define BENCHMARK_FRAMES 8u
#define PLAYER_X 72u
#define PLAYER_Y 128u

#ifndef OBJECT_LAYOUT_SOA
#define OBJECT_LAYOUT_SOA 0
#endif

#if OBJECT_LAYOUT_SOA
typedef struct ObjectStore {
    uint8_t x[OBJECT_COUNT];
    uint8_t y[OBJECT_COUNT];
    uint8_t speed[OBJECT_COUNT];
    uint8_t delay[OBJECT_COUNT];
    uint8_t type[OBJECT_COUNT];
} ObjectStore;

static ObjectStore objects;

#define OBJECT_X(i) objects.x[i]
#define OBJECT_Y(i) objects.y[i]
#define OBJECT_SPEED(i) objects.speed[i]
#define OBJECT_DELAY(i) objects.delay[i]
#define OBJECT_TYPE(i) objects.type[i]
#else
typedef struct Object {
    uint8_t x;
    uint8_t y;
    uint8_t speed;
    uint8_t delay;
    uint8_t type;
} Object;

static Object objects[OBJECT_COUNT];

#define OBJECT_X(i) objects[i].x
#define OBJECT_Y(i) objects[i].y
#define OBJECT_SPEED(i) objects[i].speed
#define OBJECT_DELAY(i) objects[i].delay
#define OBJECT_TYPE(i) objects[i].type
#endif

static volatile uint8_t result;

static void initialize_objects(void) {
    uint8_t i;

    for (i = 0u; i != OBJECT_COUNT; ++i) {
        OBJECT_X(i) = 24u + i * 20u;
        OBJECT_Y(i) = 104u + i * 5u;
        OBJECT_SPEED(i) = 1u + (i & 1u);
        OBJECT_DELAY(i) = i & 1u;
        OBJECT_TYPE(i) = i & 1u;
    }
}

static uint8_t overlaps_player(uint8_t i) {
    return (OBJECT_Y(i) + 7u >= PLAYER_Y) && (OBJECT_Y(i) <= PLAYER_Y + 7u) &&
           (OBJECT_X(i) + 7u >= PLAYER_X) && (OBJECT_X(i) <= PLAYER_X + 15u);
}

static void update_objects(void) {
    uint8_t i;

    for (i = 0u; i != OBJECT_COUNT; ++i) {
        if (OBJECT_DELAY(i)) {
            --OBJECT_DELAY(i);
            continue;
        }

        OBJECT_Y(i) += OBJECT_SPEED(i);
        result += OBJECT_X(i) + OBJECT_Y(i);

        if (overlaps_player(i)) {
            result += OBJECT_TYPE(i);
            OBJECT_Y(i) = 8u;
            OBJECT_DELAY(i) = i;
        } else if (OBJECT_Y(i) > 144u) {
            OBJECT_Y(i) = 8u;
        }
    }
}

void main(void) {
    uint8_t frame_count;
    uint8_t elapsed_div_ticks;

    DISPLAY_OFF;
    initialize_objects();
    result = 0u;

    disable_interrupts();
    DIV_REG = 0u;
    for (frame_count = 0u; frame_count != BENCHMARK_FRAMES; ++frame_count) {
        update_objects();
    }
    elapsed_div_ticks = DIV_REG;
    enable_interrupts();

    DISPLAY_ON;
#if OBJECT_LAYOUT_SOA
    printf("SOA\n");
#else
    printf("AOS\n");
#endif
    printf("%u FRAMES\n%u DIV TICKS\nCHECK %u\n",
           (uint16_t)BENCHMARK_FRAMES,
           (uint16_t)elapsed_div_ticks,
           (uint16_t)result);

    while (1) {
        vsync();
    }
}
