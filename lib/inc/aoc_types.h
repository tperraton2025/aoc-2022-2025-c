#ifndef AOC_TYPES
#define AOC_TYPES

#include <stddef.h>
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define us size_t

typedef enum
{
    AOC_2D_DIR_UP = 0,
    AOC_2D_DIR_DOWN,
    AOC_2D_DIR_RIGHT,
    AOC_2D_DIR_LEFT,
    AOC_2D_DIR_MAX,
} AOC_2D_DIR;

static const char *const AOC_2D_DIR_TXT[4] = {"UP", "DOWN", "RIGHT", "LEFT"};

typedef enum
{
    AOC_2D_DIR_FLAG_UP = 0b0001,
    AOC_2D_DIR_FLAG_DOWN = 0b0010,
    AOC_2D_DIR_FLAG_RIGHT = 0b0100,
    AOC_2D_DIR_FLAG_LEFT = 0b1000,
    AOC_2D_DIR_FLAG_MAX,
} AOC_2D_DIR_FLAG;

#endif